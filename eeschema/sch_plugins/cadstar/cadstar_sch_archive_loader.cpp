/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2020 Roberto Fernandez Bautista <roberto.fer.bau@gmail.com>
 * Copyright (C) 2020 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file cadstar_sch_archive_loader.cpp
 * @brief Loads a csa file into a KiCad SCHEMATIC object
 */

#include <sch_plugins/cadstar/cadstar_sch_archive_loader.h>

#include <eda_text.h>
#include <lib_arc.h>
#include <lib_polyline.h>
#include <lib_text.h>
#include <sch_edit_frame.h> //COMPONENT_ORIENTATION_T
#include <sch_io_mgr.h>
#include <sch_screen.h>
#include <sch_sheet.h>
#include <schematic.h>
#include <trigo.h>
#include <wildcards_and_files_ext.h>


void CADSTAR_SCH_ARCHIVE_LOADER::Load( ::SCHEMATIC* aSchematic, ::SCH_SHEET* aRootSheet,
        SCH_PLUGIN::SCH_PLUGIN_RELEASER* aSchPlugin, wxFileName aLibraryFileName )
{
    Parse();

    LONGPOINT designLimit = Assignments.Settings.DesignLimit;

    //Note: can't use getKiCadPoint() due wxPoint being int - need long long to make the check
    long long designSizeXkicad = (long long) designLimit.x * KiCadUnitMultiplier;
    long long designSizeYkicad = (long long) designLimit.y * KiCadUnitMultiplier;

    // Max size limited by the positive dimension of wxPoint (which is an int)
    constexpr long long maxDesignSizekicad = std::numeric_limits<int>::max();

    if( designSizeXkicad > maxDesignSizekicad || designSizeYkicad > maxDesignSizekicad )
    {
        THROW_IO_ERROR( wxString::Format(
                _( "The design is too large and cannot be imported into KiCad. \n"
                   "Please reduce the maximum design size in CADSTAR by navigating to: \n"
                   "Design Tab -> Properties -> Design Options -> Maximum Design Size. \n"
                   "Current Design size: %.2f, %.2f millimeters. \n"
                   "Maximum permitted design size: %.2f, %.2f millimeters.\n" ),
                (double) designSizeXkicad / SCH_IU_PER_MM,
                (double) designSizeYkicad / SCH_IU_PER_MM,
                (double) maxDesignSizekicad / SCH_IU_PER_MM,
                (double) maxDesignSizekicad / SCH_IU_PER_MM ) );
    }

    // Assume the centre at 0,0 since we are going to be translating the design afterwards anyway
    mDesignCenter = { 0, 0 };

    mSchematic       = aSchematic;
    mRootSheet       = aRootSheet;
    mPlugin          = aSchPlugin;
    mLibraryFileName = aLibraryFileName;

    loadSheets();
    loadPartsLibrary();
    loadSchematicSymbolInstances();
    // TODO Load other elements!

    // For all sheets, centre all elements and re calculate the page size:
    for( std::pair<LAYER_ID, SCH_SHEET*> sheetPair : mSheetMap )
    {
        SCH_SHEET* sheet = sheetPair.second;

        // Calculate the new sheet size.
        EDA_RECT sheetBoundingBox;

        for( auto item : sheet->GetScreen()->Items() )
            sheetBoundingBox.Merge( item->GetBoundingBox() );

        wxSize targetSheetSize = sheetBoundingBox.GetSize();
        targetSheetSize.IncBy( Mils2iu( 400 ), Mils2iu( 400 ) );

        // Get current Eeschema sheet size.
        wxSize    pageSizeIU = sheet->GetScreen()->GetPageSettings().GetSizeIU();
        PAGE_INFO pageInfo   = sheet->GetScreen()->GetPageSettings();

        // Increase if necessary
        if( pageSizeIU.x < targetSheetSize.x )
            pageInfo.SetWidthMils( Iu2Mils( targetSheetSize.x ) );

        if( pageSizeIU.y < targetSheetSize.y )
            pageInfo.SetHeightMils( Iu2Mils( targetSheetSize.y ) );

        // Set the new sheet size.
        sheet->GetScreen()->SetPageSettings( pageInfo );

        pageSizeIU = sheet->GetScreen()->GetPageSettings().GetSizeIU();
        wxPoint sheetcentre( pageSizeIU.x / 2, pageSizeIU.y / 2 );
        wxPoint itemsCentre = sheetBoundingBox.Centre();

        // round the translation to nearest 100mil to place it on the grid.
        wxPoint translation = sheetcentre - itemsCentre;
        translation.x       = translation.x - translation.x % Mils2iu( 100 );
        translation.y       = translation.y - translation.y % Mils2iu( 100 );

        // Translate the items.
        std::vector<SCH_ITEM*> allItems;

        std::copy( sheet->GetScreen()->Items().begin(), sheet->GetScreen()->Items().end(),
                std::back_inserter( allItems ) );

        for( SCH_ITEM* item : allItems )
        {
            item->SetPosition( item->GetPosition() + translation );
            item->ClearFlags();
            sheet->GetScreen()->Update( item );
        }
    }

    wxLogMessage(
            _( "The CADSTAR design has been imported successfully.\n"
               "Please review the import errors and warnings (if any)." ) );
}


void CADSTAR_SCH_ARCHIVE_LOADER::loadSheets()
{
    const std::vector<LAYER_ID>& orphanSheets = findOrphanSheets();

    if( orphanSheets.size() > 1 )
    {
        int x = 1;
        int y = 1;

        for( LAYER_ID sheetID : orphanSheets )
        {
            wxPoint pos( x * Mils2iu( 1000 ), y * Mils2iu( 1000 ) );
            wxSize  siz( Mils2iu( 1000 ), Mils2iu( 1000 ) );

            loadSheetAndChildSheets( sheetID, pos, siz, mRootSheet );

            x += 2;

            if( x > 10 ) // start next row
            {
                x = 1;
                y += 2;
            }
        }
    }
    else if( orphanSheets.size() > 0 )
    {
        LAYER_ID rootSheetID = orphanSheets.at( 0 );

        wxFileName loadedFilePath = wxFileName( Filename );

        std::string filename = wxString::Format(
                "%s_%02d", loadedFilePath.GetName(), getSheetNumber( rootSheetID ) )
                                       .ToStdString();
        ReplaceIllegalFileNameChars( &filename );
        filename += wxT( "." ) + KiCadSchematicFileExtension;

        wxFileName fn( filename );
        mRootSheet->GetScreen()->SetFileName( fn.GetFullPath() );

        mSheetMap.insert( { rootSheetID, mRootSheet } );
        loadChildSheets( rootSheetID );
    }
    else
    {
        THROW_IO_ERROR( _( "The CADSTAR schematic might be corrupt: there is no root sheet." ) );
    }
}


void CADSTAR_SCH_ARCHIVE_LOADER::loadPartsLibrary()
{
    for( std::pair<PART_ID, PART> partPair : Parts.PartDefinitions )
    {
        PART_ID key  = partPair.first;
        PART    part = partPair.second;

        if( part.Definition.GateSymbols.size() == 0 )
            continue;

        LIB_PART* kiPart = new LIB_PART( part.Name );

        kiPart->SetUnitCount( part.Definition.GateSymbols.size() );

        for( std::pair<GATE_ID, PART::DEFINITION::GATE> gatePair : part.Definition.GateSymbols )
        {
            GATE_ID                gateID   = gatePair.first;
            PART::DEFINITION::GATE gate     = gatePair.second;
            SYMDEF_ID              symbolID = getSymDefFromName( gate.Name, gate.Alternate );

            if( symbolID.IsEmpty() )
            {
                wxLogWarning( wxString::Format(
                        _( "Part definition '%s' references symbol '%s' (alternate '%s') "
                           "which could not be found in the symbol library. The part has not "
                           "been loaded into the KiCad library." ),
                        part.Name, gate.Name, gate.Alternate ) );

                continue;
            }

            loadSymDefIntoLibrary( symbolID, &part, gateID, kiPart );
        }

        ( *mPlugin )->SaveSymbol( mLibraryFileName.GetFullPath(), kiPart );

        LIB_PART* loadedPart =
                ( *mPlugin )->LoadSymbol( mLibraryFileName.GetFullPath(), kiPart->GetName() );

        mPartMap.insert( { key, loadedPart } );
    }
}


void CADSTAR_SCH_ARCHIVE_LOADER::loadSchematicSymbolInstances()
{
    for( std::pair<SYMBOL_ID, SYMBOL> symPair : Schematic.Symbols )
    {
        SYMBOL sym = symPair.second;

        if( sym.IsComponent )
        {
            if( mPartMap.find( sym.PartRef.RefID ) == mPartMap.end() )
            {
                wxLogError( wxString::Format(
                        _( "Symbol '%s' references part '%s' which could not be found "
                           "in the library. The symbol was not loaded" ),
                        sym.ComponentRef.Designator, sym.PartRef.RefID ) );

                continue;
            }

            LIB_PART* kiPart                     = mPartMap.at( sym.PartRef.RefID );
            double    compOrientationTenthDegree = 0.0;

            SCH_COMPONENT* component =
                    loadSchematicSymbol( sym, kiPart, compOrientationTenthDegree );

            SCH_FIELD* refField = component->GetField( REFERENCE );
            refField->SetText( sym.ComponentRef.Designator );
            loadSymbolFieldAttribute(
                    sym.ComponentRef.AttrLoc, compOrientationTenthDegree, refField );
        }
        else if( sym.IsSymbolVariant )
        {
            if( Library.SymbolDefinitions.find( sym.SymdefID ) == Library.SymbolDefinitions.end() )
            {
                THROW_IO_ERROR( wxString::Format(
                        _( "Symbol ID '%s' references library symbol '%s' which could not be "
                           "found in the library. Did you export all items of the design?" ),
                        sym.ID, sym.PartRef.RefID ) );
            }

            SYMDEF_SCM libSymDef = Library.SymbolDefinitions.at( sym.SymdefID );

            if( libSymDef.Terminals.size() != 1 )
            {
                THROW_IO_ERROR( wxString::Format(
                        _( "Symbol ID '%s' is a signal reference or global signal but it has too "
                           "many pins. The expected number of pins is 1 but %d were found." ),
                        sym.ID, libSymDef.Terminals.size() ) );
            }

            if( sym.SymbolVariant.Type == SYMBOLVARIANT::TYPE::GLOBALSIGNAL )
            {
                SYMDEF_ID symID    = sym.SymdefID;
                LIB_PART* kiPart   = nullptr;
                wxString  partName = sym.SymbolVariant.Reference;

                partName = LIB_ID::FixIllegalChars( partName, LIB_ID::ID_SCH );

                if( mPowerSymMap.find( symID ) == mPowerSymMap.end()
                        || mPowerSymMap.at( symID )->GetName() != partName )
                {
                    kiPart = new LIB_PART( partName );
                    kiPart->SetPower();
                    loadSymDefIntoLibrary( symID, nullptr, "A", kiPart );
                    kiPart->GetValueField().SetText( partName );
                    kiPart->GetReferenceField().SetText( "#PWR" );
                    ( *mPlugin )->SaveSymbol( mLibraryFileName.GetFullPath(), kiPart );
                    mPowerSymMap.insert( { symID, kiPart } );
                }
                else
                {
                    kiPart = mPowerSymMap.at( symID );
                }

                double compOrientationTenthDegree = 0.0;

                SCH_COMPONENT* component =
                        loadSchematicSymbol( sym, kiPart, compOrientationTenthDegree );
            }
            else if( sym.SymbolVariant.Type == SYMBOLVARIANT::TYPE::SIGNALREF )
            {
                // There should only be one pin and we'll use that to set the position
                TERMINAL& symbolTerminal    = libSymDef.Terminals.begin()->second;
                wxPoint   terminalPosOffset = symbolTerminal.Position - libSymDef.Origin;

                SCH_GLOBALLABEL* netLabel = new SCH_GLOBALLABEL;
                netLabel->SetPosition( getKiCadPoint( sym.Origin + terminalPosOffset ) );
                netLabel->SetText( "SOMEGLOBALNET" );
                netLabel->SetTextSize( wxSize( Mils2iu( 50 ), Mils2iu( 50 ) ) );
                netLabel->SetLabelSpinStyle( getSpinStyle( sym.OrientAngle, sym.Mirror ) );

                if( libSymDef.Alternate.Lower().Contains( "in" ) )
                    netLabel->SetShape( PINSHEETLABEL_SHAPE::PS_INPUT );
                else if( libSymDef.Alternate.Lower().Contains( "bi" ) )
                    netLabel->SetShape( PINSHEETLABEL_SHAPE::PS_BIDI );
                else if( libSymDef.Alternate.Lower().Contains( "out" ) )
                    netLabel->SetShape( PINSHEETLABEL_SHAPE::PS_OUTPUT );
                else
                    netLabel->SetShape( PINSHEETLABEL_SHAPE::PS_UNSPECIFIED );

                mSheetMap.at( sym.LayerID )->GetScreen()->Append( netLabel );
            }
            else
            {
                wxASSERT_MSG( false, "Unkown Symbol Variant." );
            }
        }
        else
        {
            wxLogError( wxString::Format(
                    _( "Symbol ID '%s' is of an unknown type. It is neither a component or a "
                       "net (power /symbol. The symbol was not loaded." ),
                    sym.ID ) );
        }
    }
}


void CADSTAR_SCH_ARCHIVE_LOADER::loadSymDefIntoLibrary( const SYMDEF_ID& aSymdefID,
        const PART* aCadstarPart, const GATE_ID& aGateID, LIB_PART* aPart )
{
    wxCHECK( Library.SymbolDefinitions.find( aSymdefID ) != Library.SymbolDefinitions.end(), );

    SYMDEF_SCM symbol = Library.SymbolDefinitions.at( aSymdefID );

    //TODO add symbolName to KiCad part "unit"
    wxString symbolName = generateSymDefName( aSymdefID );
    int      gateNumber = getKiCadUnitNumberFromGate( aGateID );

    for( std::pair<FIGURE_ID, FIGURE> figPair : symbol.Figures )
    {
        FIGURE fig = figPair.second;

        loadLibrarySymbolShapeVertices( fig.Shape.Vertices, symbol.Origin, aPart, gateNumber );

        for( CUTOUT c : fig.Shape.Cutouts )
        {
            loadLibrarySymbolShapeVertices( c.Vertices, symbol.Origin, aPart, gateNumber );
        }
    }

    for( std::pair<TERMINAL_ID, TERMINAL> termPair : symbol.Terminals )
    {
        TERMINAL term    = termPair.second;
        wxString pinNum  = wxString::Format( "%d", term.ID );
        wxString pinName = wxEmptyString;

        if( aCadstarPart )
        {
            PART::DEFINITION::PIN csPin = getPartDefinitionPin( *aCadstarPart, aGateID, term.ID );

            pinName = csPin.Name;
            pinNum  = wxString::Format( "%d", csPin.ID );

            if( pinName.IsEmpty() )
            {
                if( !csPin.Identifier.IsEmpty() )
                    pinName = csPin.Identifier;
            }
        }

        LIB_PIN* pin = new LIB_PIN( aPart );

        pin->SetPosition( getKiCadLibraryPoint( term.Position, symbol.Origin ) );
        pin->SetLength( 0 ); //CADSTAR Pins are just a point (have no length)
        pin->SetShape( GRAPHIC_PINSHAPE::LINE );
        pin->SetUnit( gateNumber );
        pin->SetNumber( pinNum );

        if( !aCadstarPart )
            pin->SetName( pinName );
        else if( !aCadstarPart->Definition.HidePinNames )
            pin->SetName( pinName );

        int oDeg = (int) NormalizeAngle180( getAngleTenthDegree( term.OrientAngle ) );

        if( oDeg >= -450 && oDeg <= 450 )
            pin->SetOrientation( 'R' ); // 0 degrees
        else if( oDeg >= 450 && oDeg <= 1350 )
            pin->SetOrientation( 'U' ); // 90 degrees
        else if( oDeg >= 1350 || oDeg <= -1350 )
            pin->SetOrientation( 'L' ); // 180 degrees
        else
            pin->SetOrientation( 'D' ); // -90 degrees

        if( aPart->IsPower() )
        {
            pin->SetVisible( false );
            pin->SetType( ELECTRICAL_PINTYPE::PT_POWER_IN );
            pin->SetName( aPart->GetName() );
        }

        aPart->AddDrawItem( pin );
    }

    for( std::pair<TEXT_ID, TEXT> textPair : symbol.Texts )
    {
        TEXT      csText  = textPair.second;
        LIB_TEXT* libtext = new LIB_TEXT( aPart );
        libtext->SetText( csText.Text );
        libtext->SetUnit( gateNumber );
        libtext->SetPosition( getKiCadLibraryPoint( csText.Position, symbol.Origin ) );
        applyTextSettings( csText.TextCodeID, csText.Alignment, csText.Justification, libtext );
        aPart->AddDrawItem( libtext );
    }

    if( symbol.TextLocations.find( SYMBOL_NAME_ATTRID ) != symbol.TextLocations.end() )
    {
        TEXT_LOCATION textLoc = symbol.TextLocations.at( SYMBOL_NAME_ATTRID );
        LIB_FIELD*    field   = aPart->GetField( REFERENCE );
        loadLibraryFieldAttribute( textLoc, symbol.Origin, field );
        field->SetUnit( gateNumber );
    }

    if( symbol.TextLocations.find( PART_NAME_ATTRID ) != symbol.TextLocations.end() )
    {
        TEXT_LOCATION textLoc = symbol.TextLocations.at( PART_NAME_ATTRID );
        LIB_FIELD*    field   = aPart->GetField( FIELD1 );

        if( !field )
        {
            field = new LIB_FIELD( aPart, FIELD1 );
            std::vector<LIB_FIELD> partFields;
            aPart->GetFields( partFields );
            partFields.push_back( *field );
            aPart->SetFields( partFields );
        }

        field->SetName( "Part Name" );
        loadLibraryFieldAttribute( textLoc, symbol.Origin, field );

        if( aCadstarPart )
            field->SetText( aCadstarPart->Definition.Name );

        field->SetUnit( gateNumber );
    }
}


void CADSTAR_SCH_ARCHIVE_LOADER::loadLibrarySymbolShapeVertices(
        const std::vector<VERTEX>& aCadstarVertices, wxPoint aSymbolOrigin, LIB_PART* aPart,
        int aGateNumber )
{
    const VERTEX* prev = &aCadstarVertices.at( 0 );
    const VERTEX* cur;

    wxASSERT_MSG(
            prev->Type == VERTEX_TYPE::POINT, "First vertex should always be a point vertex" );

    for( size_t i = 1; i < aCadstarVertices.size(); i++ )
    {
        cur = &aCadstarVertices.at( i );

        LIB_ITEM* segment    = nullptr;
        bool      cw         = false;
        wxPoint   startPoint = getKiCadLibraryPoint( prev->End, aSymbolOrigin );
        wxPoint   endPoint   = getKiCadLibraryPoint( cur->End, aSymbolOrigin );
        wxPoint   centerPoint;

        if( cur->Type == VERTEX_TYPE::ANTICLOCKWISE_SEMICIRCLE
                || cur->Type == VERTEX_TYPE::CLOCKWISE_SEMICIRCLE )
        {
            centerPoint = ( startPoint + endPoint ) / 2;
        }
        else
        {
            centerPoint = getKiCadLibraryPoint( cur->Center, aSymbolOrigin );
        }


        switch( cur->Type )
        {
        case VERTEX_TYPE::POINT:
            segment = new LIB_POLYLINE( aPart );
            ( (LIB_POLYLINE*) segment )->AddPoint( startPoint );
            ( (LIB_POLYLINE*) segment )->AddPoint( endPoint );
            break;

        case VERTEX_TYPE::CLOCKWISE_SEMICIRCLE:
        case VERTEX_TYPE::CLOCKWISE_ARC:
            cw = true;
            KI_FALLTHROUGH;

        case VERTEX_TYPE::ANTICLOCKWISE_SEMICIRCLE:
        case VERTEX_TYPE::ANTICLOCKWISE_ARC:
            segment = new LIB_ARC( aPart );

            ( (LIB_ARC*) segment )->SetPosition( centerPoint );

            if( cw )
            {
                ( (LIB_ARC*) segment )->SetStart( endPoint );
                ( (LIB_ARC*) segment )->SetEnd( startPoint );
            }
            else
            {
                ( (LIB_ARC*) segment )->SetStart( startPoint );
                ( (LIB_ARC*) segment )->SetEnd( endPoint );
            }

            ( (LIB_ARC*) segment )->CalcRadiusAngles();
            break;
        }

        segment->SetUnit( aGateNumber );
        aPart->AddDrawItem( segment );

        prev = cur;
    }
}


void CADSTAR_SCH_ARCHIVE_LOADER::loadLibraryFieldAttribute(
        const ATTRIBUTE_LOCATION& aCadstarAttrLoc, wxPoint aSymbolOrigin, LIB_FIELD* aKiCadField )
{
    aKiCadField->SetTextPos( getKiCadLibraryPoint( aCadstarAttrLoc.Position, aSymbolOrigin ) );
    aKiCadField->SetTextAngle( getAngleTenthDegree( aCadstarAttrLoc.OrientAngle ) );
    aKiCadField->SetBold( false );
    aKiCadField->SetVisible( true );

    applyTextSettings( aCadstarAttrLoc.TextCodeID, aCadstarAttrLoc.Alignment,
            aCadstarAttrLoc.Justification, aKiCadField );
}


SCH_COMPONENT* CADSTAR_SCH_ARCHIVE_LOADER::loadSchematicSymbol(
        const SYMBOL& aCadstarSymbol, LIB_PART* aKiCadPart, double& aComponentOrientationDeciDeg )
{
    SCH_COMPONENT* component = new SCH_COMPONENT();

    component->SetPosition( getKiCadPoint( aCadstarSymbol.Origin ) );

    int compOrientation = COMPONENT_ORIENTATION_T::CMP_ORIENT_0;

    int oDeg = (int) NormalizeAngle180( getAngleTenthDegree( aCadstarSymbol.OrientAngle ) );

    if( oDeg >= -450 && oDeg <= 450 )
    {
        compOrientation              = COMPONENT_ORIENTATION_T::CMP_ORIENT_0;
        aComponentOrientationDeciDeg = 0.0;
    }
    else if( oDeg >= 450 && oDeg <= 1350 )
    {
        compOrientation              = COMPONENT_ORIENTATION_T::CMP_ORIENT_90;
        aComponentOrientationDeciDeg = 900.0;
    }
    else if( oDeg >= 1350 || oDeg <= -1350 )
    {
        compOrientation              = COMPONENT_ORIENTATION_T::CMP_ORIENT_180;
        aComponentOrientationDeciDeg = 1800.0;
    }
    else
    {
        compOrientation              = COMPONENT_ORIENTATION_T::CMP_ORIENT_270;
        aComponentOrientationDeciDeg = 2700.0;
    }

    if( aCadstarSymbol.Mirror )
        compOrientation += COMPONENT_ORIENTATION_T::CMP_MIRROR_Y;

    component->SetOrientation( compOrientation );
    LIB_ID libId( mLibraryFileName.GetName(), aKiCadPart->GetName() );
    component->SetLibId( libId );
    component->SetLibSymbol( new LIB_PART( *aKiCadPart ) );
    component->SetUnit( getKiCadUnitNumberFromGate( aCadstarSymbol.GateID ) );

    if( mSheetMap.find( aCadstarSymbol.LayerID ) == mSheetMap.end() )
    {
        wxLogError(
                wxString::Format( _( "Symbol '%s' references sheet ID '%s' which does not exist in "
                                     "the design. The symbol was not loaded." ),
                        aCadstarSymbol.ComponentRef.Designator, aCadstarSymbol.LayerID ) );

        delete component;
        return nullptr;
    }


    SCH_SHEET* kiSheet = mSheetMap.at( aCadstarSymbol.LayerID );

    SCH_SHEET_PATH sheetpath;
    mRootSheet->LocatePathOfScreen( kiSheet->GetScreen(), &sheetpath );
    wxString currentSheetPath = sheetpath.PathAsString() + component->m_Uuid.AsString();

    if( aCadstarSymbol.IsComponent )
    {
        component->AddHierarchicalReference( currentSheetPath,
                aCadstarSymbol.ComponentRef.Designator,
                getKiCadUnitNumberFromGate( aCadstarSymbol.GateID ) );
    }

    kiSheet->GetScreen()->Append( component );

    return component;
}

void CADSTAR_SCH_ARCHIVE_LOADER::loadSymbolFieldAttribute(
        const ATTRIBUTE_LOCATION& aCadstarAttrLoc, const double& aComponentOrientationDeciDeg,
        SCH_FIELD* aKiCadField )
{
    aKiCadField->SetPosition( getKiCadPoint( aCadstarAttrLoc.Position ) );
    aKiCadField->SetTextAngle(
            getAngleTenthDegree( aCadstarAttrLoc.OrientAngle ) - aComponentOrientationDeciDeg );
    aKiCadField->SetBold( false );
    aKiCadField->SetVisible( true );

    applyTextSettings( aCadstarAttrLoc.TextCodeID, aCadstarAttrLoc.Alignment,
            aCadstarAttrLoc.Justification, aKiCadField );
}


void CADSTAR_SCH_ARCHIVE_LOADER::loadSheetAndChildSheets(
        LAYER_ID aCadstarSheetID, wxPoint aPosition, wxSize aSheetSize, SCH_SHEET* aParentSheet )
{
    wxCHECK_MSG( mSheetMap.find( aCadstarSheetID ) == mSheetMap.end(), , "Sheet already loaded!" );

    SCH_SHEET*  sheet  = new SCH_SHEET( aParentSheet, aPosition );
    SCH_SCREEN* screen = new SCH_SCREEN( mSchematic );

    sheet->SetSize( aSheetSize );
    sheet->SetScreen( screen );

    wxString name = Sheets.SheetNames.at( aCadstarSheetID );

    SCH_FIELD& sheetNameField = sheet->GetFields()[SHEETNAME];
    SCH_FIELD& filenameField  = sheet->GetFields()[SHEETFILENAME];

    sheetNameField.SetText( name );

    wxFileName  loadedFilePath = wxFileName( Filename );
    std::string filename       = wxString::Format(
            "%s_%02d", loadedFilePath.GetName(), getSheetNumber( aCadstarSheetID ) )
                                   .ToStdString();

    ReplaceIllegalFileNameChars( &filename );
    filename += wxT( "." ) + KiCadSchematicFileExtension;

    filenameField.SetText( filename );
    wxFileName fn( filename );
    sheet->GetScreen()->SetFileName( fn.GetFullPath() );
    aParentSheet->GetScreen()->Append( sheet );

    mSheetMap.insert( { aCadstarSheetID, sheet } );

    loadChildSheets( aCadstarSheetID );
}


void CADSTAR_SCH_ARCHIVE_LOADER::loadChildSheets( LAYER_ID aCadstarSheetID )
{
    wxCHECK_MSG( mSheetMap.find( aCadstarSheetID ) != mSheetMap.end(), ,
            "FIXME! Parent sheet should be loaded before attempting to load subsheets" );

    for( std::pair<BLOCK_ID, BLOCK> blockPair : Schematic.Blocks )
    {
        BLOCK& block = blockPair.second;

        if( block.LayerID == aCadstarSheetID && block.Type == BLOCK::TYPE::CHILD )
        {
            // In KiCad you can only draw rectangular shapes whereas in Cadstar arbitrary shapes
            // are allowed. We will calculate the extents of the Cadstar shape and draw a rectangle

            std::pair<wxPoint, wxSize> blockExtents;

            if( block.Figures.size() > 0 )
            {
                blockExtents = getFigureExtentsKiCad( block.Figures.begin()->second );
            }
            else
            {
                THROW_IO_ERROR( wxString::Format(
                        _( "The CADSTAR schematic might be corrupt: Block %s references a "
                           "child sheet but has no Figure defined." ),
                        block.ID ) );
            }

            loadSheetAndChildSheets( block.AssocLayerID, blockExtents.first, blockExtents.second,
                    mSheetMap.at( aCadstarSheetID ) );

            if( block.HasBlockLabel )
            {
                // Add the block label as a separate field
                SCH_SHEET* loadedSheet = mSheetMap.at( block.AssocLayerID );
                SCH_FIELDS fields      = loadedSheet->GetFields();

                for( SCH_FIELD& field : fields )
                {
                    field.SetVisible( false );
                }

                SCH_FIELD blockNameField( getKiCadPoint( block.BlockLabel.Position ), 2,
                        loadedSheet, wxString( "Block name" ) );
                applyTextSettings( block.BlockLabel.TextCodeID, block.BlockLabel.Alignment,
                        block.BlockLabel.Justification, &blockNameField );
                blockNameField.SetTextAngle( getAngleTenthDegree( block.BlockLabel.OrientAngle ) );
                blockNameField.SetText( block.Name );
                blockNameField.SetVisible( true );
                fields.push_back( blockNameField );
                loadedSheet->SetFields( fields );
            }
        }
    }
}


std::vector<CADSTAR_SCH_ARCHIVE_LOADER::LAYER_ID> CADSTAR_SCH_ARCHIVE_LOADER::findOrphanSheets()
{
    std::vector<LAYER_ID> childSheets, orphanSheets;

    //Find all sheets that are child of another
    for( std::pair<BLOCK_ID, BLOCK> blockPair : Schematic.Blocks )
    {
        BLOCK&    block        = blockPair.second;
        LAYER_ID& assocSheetID = block.AssocLayerID;

        if( block.Type == BLOCK::TYPE::CHILD )
            childSheets.push_back( assocSheetID );
    }

    //Add sheets that do not have a parent
    for( LAYER_ID sheetID : Sheets.SheetOrder )
    {
        if( std::find( childSheets.begin(), childSheets.end(), sheetID ) == childSheets.end() )
            orphanSheets.push_back( sheetID );
    }

    return orphanSheets;
}


int CADSTAR_SCH_ARCHIVE_LOADER::getSheetNumber( LAYER_ID aCadstarSheetID )
{
    int i = 1;

    for( LAYER_ID sheetID : Sheets.SheetOrder )
    {
        if( sheetID == aCadstarSheetID )
            return i;

        ++i;
    }

    return -1;
}


void CADSTAR_SCH_ARCHIVE_LOADER::checkAndLogHatchCode( const HATCHCODE_ID& aCadstarHatchcodeID )
{
    if( mHatchcodesTested.find( aCadstarHatchcodeID ) != mHatchcodesTested.end() )
    {
        return; //already checked
    }
    else
    {
        HATCHCODE hcode = getHatchCode( aCadstarHatchcodeID );

        if( hcode.Hatches.size() != 2 )
        {
            wxLogWarning( wxString::Format(
                    _( "The CADSTAR Hatching code '%s' has %d hatches defined. "
                       "KiCad only supports 2 hatches (crosshatching) 90 degrees apart. "
                       "The imported hatching is crosshatched." ),
                    hcode.Name, (int) hcode.Hatches.size() ) );
        }
        else
        {
            if( hcode.Hatches.at( 0 ).LineWidth != hcode.Hatches.at( 1 ).LineWidth )
            {
                wxLogWarning( wxString::Format(
                        _( "The CADSTAR Hatching code '%s' has different line widths for each "
                           "hatch. KiCad only supports one width for the haching. The imported "
                           "hatching uses the width defined in the first hatch definition, i.e. "
                           "%.2f mm." ),
                        hcode.Name,
                        (double) ( (double) getKiCadLength( hcode.Hatches.at( 0 ).LineWidth ) )
                                / 1E6 ) );
            }

            if( hcode.Hatches.at( 0 ).Step != hcode.Hatches.at( 1 ).Step )
            {
                wxLogWarning( wxString::Format(
                        _( "The CADSTAR Hatching code '%s' has different step sizes for each "
                           "hatch. KiCad only supports one step size for the haching. The imported "
                           "hatching uses the step size defined in the first hatching definition, "
                           "i.e. %.2f mm." ),
                        hcode.Name,
                        (double) ( (double) getKiCadLength( hcode.Hatches.at( 0 ).Step ) )
                                / 1E6 ) );
            }

            if( abs( hcode.Hatches.at( 0 ).OrientAngle - hcode.Hatches.at( 1 ).OrientAngle )
                    != 90000 )
            {
                wxLogWarning( wxString::Format(
                        _( "The hatches in CADSTAR Hatching code '%s' have an angle  "
                           "difference of %.1f degrees. KiCad only supports hatching 90 "
                           "degrees apart.  The imported hatching has two hatches 90 "
                           "degrees apart, oriented %.1f degrees from horizontal." ),
                        hcode.Name,
                        getAngleDegrees( abs( hcode.Hatches.at( 0 ).OrientAngle
                                              - hcode.Hatches.at( 1 ).OrientAngle ) ),
                        getAngleDegrees( hcode.Hatches.at( 0 ).OrientAngle ) ) );
            }
        }

        mHatchcodesTested.insert( aCadstarHatchcodeID );
    }
}


CADSTAR_SCH_ARCHIVE_LOADER::SYMDEF_ID CADSTAR_SCH_ARCHIVE_LOADER::getSymDefFromName(
        const wxString& aSymdefName, const wxString& aSymDefAlternate )
{
    for( std::pair<SYMDEF_ID, SYMDEF_SCM> symPair : Library.SymbolDefinitions )
    {
        SYMDEF_ID  id     = symPair.first;
        SYMDEF_SCM symdef = symPair.second;

        if( symdef.ReferenceName == aSymdefName && symdef.Alternate == aSymDefAlternate )
            return id;
    }

    return SYMDEF_ID();
}


wxString CADSTAR_SCH_ARCHIVE_LOADER::generateSymDefName( const SYMDEF_ID& aSymdefID )
{
    wxCHECK( Library.SymbolDefinitions.find( aSymdefID ) != Library.SymbolDefinitions.end(),
            wxEmptyString );

    SYMDEF_SCM symbol = Library.SymbolDefinitions.at( aSymdefID );

    wxString symbolName =
            symbol.ReferenceName
            + ( ( symbol.Alternate.size() > 0 ) ? ( wxT( " (" ) + symbol.Alternate + wxT( ")" ) ) :
                                                  wxT( "" ) );

    return symbolName;
}


int CADSTAR_SCH_ARCHIVE_LOADER::getLineThickness( const LINECODE_ID& aCadstarLineCodeID )
{
    wxCHECK( Assignments.Codedefs.LineCodes.find( aCadstarLineCodeID )
                     != Assignments.Codedefs.LineCodes.end(),
            mSchematic->Settings().m_DefaultWireThickness );

    return getKiCadLength( Assignments.Codedefs.LineCodes.at( aCadstarLineCodeID ).Width );
}


CADSTAR_SCH_ARCHIVE_LOADER::HATCHCODE CADSTAR_SCH_ARCHIVE_LOADER::getHatchCode(
        const HATCHCODE_ID& aCadstarHatchcodeID )
{
    wxCHECK( Assignments.Codedefs.HatchCodes.find( aCadstarHatchcodeID )
                     != Assignments.Codedefs.HatchCodes.end(),
            HATCHCODE() );

    return Assignments.Codedefs.HatchCodes.at( aCadstarHatchcodeID );
}


CADSTAR_SCH_ARCHIVE_LOADER::TEXTCODE CADSTAR_SCH_ARCHIVE_LOADER::getTextCode(
        const TEXTCODE_ID& aCadstarTextCodeID )
{
    wxCHECK( Assignments.Codedefs.TextCodes.find( aCadstarTextCodeID )
                     != Assignments.Codedefs.TextCodes.end(),
            TEXTCODE() );

    return Assignments.Codedefs.TextCodes.at( aCadstarTextCodeID );
}


wxString CADSTAR_SCH_ARCHIVE_LOADER::getAttributeName( const ATTRIBUTE_ID& aCadstarAttributeID )
{
    wxCHECK( Assignments.Codedefs.AttributeNames.find( aCadstarAttributeID )
                     != Assignments.Codedefs.AttributeNames.end(),
            wxEmptyString );

    return Assignments.Codedefs.AttributeNames.at( aCadstarAttributeID ).Name;
}


CADSTAR_SCH_ARCHIVE_LOADER::PART CADSTAR_SCH_ARCHIVE_LOADER::getPart(
        const PART_ID& aCadstarPartID )
{
    wxCHECK( Parts.PartDefinitions.find( aCadstarPartID ) != Parts.PartDefinitions.end(), PART() );

    return Parts.PartDefinitions.at( aCadstarPartID );
}


CADSTAR_SCH_ARCHIVE_LOADER::ROUTECODE CADSTAR_SCH_ARCHIVE_LOADER::getRouteCode(
        const ROUTECODE_ID& aCadstarRouteCodeID )
{
    wxCHECK( Assignments.Codedefs.RouteCodes.find( aCadstarRouteCodeID )
                     != Assignments.Codedefs.RouteCodes.end(),
            ROUTECODE() );

    return Assignments.Codedefs.RouteCodes.at( aCadstarRouteCodeID );
}


wxString CADSTAR_SCH_ARCHIVE_LOADER::getAttributeValue( const ATTRIBUTE_ID& aCadstarAttributeID,
        const std::map<ATTRIBUTE_ID, ATTRIBUTE_VALUE>&                      aCadstarAttributeMap )
{
    wxCHECK( aCadstarAttributeMap.find( aCadstarAttributeID ) != aCadstarAttributeMap.end(),
            wxEmptyString );

    return aCadstarAttributeMap.at( aCadstarAttributeID ).Value;
}


CADSTAR_SCH_ARCHIVE_LOADER::PART::DEFINITION::PIN CADSTAR_SCH_ARCHIVE_LOADER::getPartDefinitionPin(
        const PART& aCadstarPart, const GATE_ID& aGateID, const TERMINAL_ID& aTerminalID )
{
    for( std::pair<PART_DEFINITION_PIN_ID, PART::DEFINITION::PIN> pinPair :
            aCadstarPart.Definition.Pins )
    {
        PART::DEFINITION::PIN partPin = pinPair.second;

        if( partPin.TerminalGate == aGateID && partPin.TerminalPin == aTerminalID )
            return partPin;
    }

    return PART::DEFINITION::PIN();
}


double CADSTAR_SCH_ARCHIVE_LOADER::getHatchCodeAngleDegrees(
        const HATCHCODE_ID& aCadstarHatchcodeID )
{
    checkAndLogHatchCode( aCadstarHatchcodeID );
    HATCHCODE hcode = getHatchCode( aCadstarHatchcodeID );

    if( hcode.Hatches.size() < 1 )
        return mSchematic->Settings().m_DefaultWireThickness;
    else
        return getAngleDegrees( hcode.Hatches.at( 0 ).OrientAngle );
}


int CADSTAR_SCH_ARCHIVE_LOADER::getKiCadHatchCodeThickness(
        const HATCHCODE_ID& aCadstarHatchcodeID )
{
    checkAndLogHatchCode( aCadstarHatchcodeID );
    HATCHCODE hcode = getHatchCode( aCadstarHatchcodeID );

    if( hcode.Hatches.size() < 1 )
        return mSchematic->Settings().m_DefaultWireThickness;
    else
        return getKiCadLength( hcode.Hatches.at( 0 ).LineWidth );
}


int CADSTAR_SCH_ARCHIVE_LOADER::getKiCadHatchCodeGap( const HATCHCODE_ID& aCadstarHatchcodeID )
{
    checkAndLogHatchCode( aCadstarHatchcodeID );
    HATCHCODE hcode = getHatchCode( aCadstarHatchcodeID );

    if( hcode.Hatches.size() < 1 )
        return mSchematic->Settings().m_DefaultWireThickness;
    else
        return getKiCadLength( hcode.Hatches.at( 0 ).Step );
}

int CADSTAR_SCH_ARCHIVE_LOADER::getKiCadUnitNumberFromGate( const GATE_ID& aCadstarGateID )
{
    if( aCadstarGateID.IsEmpty() )
        return 1;

    return (int) aCadstarGateID.Upper().GetChar( 0 ) - (int) wxUniChar( 'A' ) + 1;
}


LABEL_SPIN_STYLE CADSTAR_SCH_ARCHIVE_LOADER::getSpinStyle(
        const long long& aCadstarOrientation, bool aMirror )
{
    LABEL_SPIN_STYLE spinStyle = LABEL_SPIN_STYLE::LEFT;
    int              oDeg = (int) NormalizeAngle180( getAngleTenthDegree( aCadstarOrientation ) );

    if( oDeg >= -450 && oDeg <= 450 )
        spinStyle = LABEL_SPIN_STYLE::RIGHT; // 0deg
    else if( oDeg >= 450 && oDeg <= 1350 )
        spinStyle = LABEL_SPIN_STYLE::BOTTOM; // 90deg
    else if( oDeg >= 1350 || oDeg <= -1350 )
        spinStyle = LABEL_SPIN_STYLE::LEFT; // 180deg
    else
        spinStyle = LABEL_SPIN_STYLE::UP; // 270deg


    if( aMirror )
    {
        spinStyle = spinStyle.RotateCCW();
        spinStyle = spinStyle.RotateCCW();
    }

    return spinStyle;
}


void CADSTAR_SCH_ARCHIVE_LOADER::applyTextSettings( const TEXTCODE_ID& aCadstarTextCodeID,
        const ALIGNMENT& aCadstarAlignment, const JUSTIFICATION& aCadstarJustification,
        EDA_TEXT* aKiCadTextItem )
{
    TEXTCODE textCode = getTextCode( aCadstarTextCodeID );

    aKiCadTextItem->SetTextWidth( getKiCadLength( textCode.Width ) );
    aKiCadTextItem->SetTextHeight( getKiCadLength( textCode.Height ) );
    aKiCadTextItem->SetTextThickness( getKiCadLength( textCode.LineWidth ) );

    switch( aCadstarAlignment )
    {
    case ALIGNMENT::NO_ALIGNMENT: // Default for Single line text is Bottom Left
    case ALIGNMENT::BOTTOMLEFT:
        aKiCadTextItem->SetVertJustify( GR_TEXT_VJUSTIFY_BOTTOM );
        aKiCadTextItem->SetHorizJustify( GR_TEXT_HJUSTIFY_LEFT );
        break;

    case ALIGNMENT::BOTTOMCENTER:
        aKiCadTextItem->SetVertJustify( GR_TEXT_VJUSTIFY_BOTTOM );
        aKiCadTextItem->SetHorizJustify( GR_TEXT_HJUSTIFY_CENTER );
        break;

    case ALIGNMENT::BOTTOMRIGHT:
        aKiCadTextItem->SetVertJustify( GR_TEXT_VJUSTIFY_BOTTOM );
        aKiCadTextItem->SetHorizJustify( GR_TEXT_HJUSTIFY_RIGHT );
        break;

    case ALIGNMENT::CENTERLEFT:
        aKiCadTextItem->SetVertJustify( GR_TEXT_VJUSTIFY_CENTER );
        aKiCadTextItem->SetHorizJustify( GR_TEXT_HJUSTIFY_LEFT );
        break;

    case ALIGNMENT::CENTERCENTER:
        aKiCadTextItem->SetVertJustify( GR_TEXT_VJUSTIFY_CENTER );
        aKiCadTextItem->SetHorizJustify( GR_TEXT_HJUSTIFY_CENTER );
        break;

    case ALIGNMENT::CENTERRIGHT:
        aKiCadTextItem->SetVertJustify( GR_TEXT_VJUSTIFY_CENTER );
        aKiCadTextItem->SetHorizJustify( GR_TEXT_HJUSTIFY_RIGHT );
        break;

    case ALIGNMENT::TOPLEFT:
        aKiCadTextItem->SetVertJustify( GR_TEXT_VJUSTIFY_TOP );
        aKiCadTextItem->SetHorizJustify( GR_TEXT_HJUSTIFY_LEFT );
        break;

    case ALIGNMENT::TOPCENTER:
        aKiCadTextItem->SetVertJustify( GR_TEXT_VJUSTIFY_TOP );
        aKiCadTextItem->SetHorizJustify( GR_TEXT_HJUSTIFY_CENTER );
        break;

    case ALIGNMENT::TOPRIGHT:
        aKiCadTextItem->SetVertJustify( GR_TEXT_VJUSTIFY_TOP );
        aKiCadTextItem->SetHorizJustify( GR_TEXT_HJUSTIFY_RIGHT );
        break;
    }
}


std::pair<wxPoint, wxSize> CADSTAR_SCH_ARCHIVE_LOADER::getFigureExtentsKiCad(
        const FIGURE& aCadstarFigure )
{
    wxPoint upperLeft( Assignments.Settings.DesignLimit.x, 0 );
    wxPoint lowerRight( 0, Assignments.Settings.DesignLimit.y );

    for( const VERTEX& v : aCadstarFigure.Shape.Vertices )
    {
        if( upperLeft.x > v.End.x )
            upperLeft.x = v.End.x;

        if( upperLeft.y < v.End.y )
            upperLeft.y = v.End.y;

        if( lowerRight.x < v.End.x )
            lowerRight.x = v.End.x;

        if( lowerRight.y > v.End.y )
            lowerRight.y = v.End.y;
    }

    for( CUTOUT cutout : aCadstarFigure.Shape.Cutouts )
    {
        for( const VERTEX& v : aCadstarFigure.Shape.Vertices )
        {
            if( upperLeft.x > v.End.x )
                upperLeft.x = v.End.x;

            if( upperLeft.y < v.End.y )
                upperLeft.y = v.End.y;

            if( lowerRight.x < v.End.x )
                lowerRight.x = v.End.x;

            if( lowerRight.y > v.End.y )
                lowerRight.y = v.End.y;
        }
    }

    wxPoint upperLeftKiCad  = getKiCadPoint( upperLeft );
    wxPoint lowerRightKiCad = getKiCadPoint( lowerRight );

    wxPoint size = lowerRightKiCad - upperLeftKiCad;

    return { upperLeftKiCad, wxSize( abs( size.x ), abs( size.y ) ) };
}


wxPoint CADSTAR_SCH_ARCHIVE_LOADER::getKiCadPoint( wxPoint aCadstarPoint )
{
    wxPoint retval;

    retval.x = ( aCadstarPoint.x - mDesignCenter.x ) * KiCadUnitMultiplier;
    retval.y = -( aCadstarPoint.y - mDesignCenter.y ) * KiCadUnitMultiplier;

    return retval;
}


wxPoint CADSTAR_SCH_ARCHIVE_LOADER::getKiCadLibraryPoint(
        wxPoint aCadstarPoint, wxPoint aCadstarCentre )
{
    wxPoint retval;

    retval.x = ( aCadstarPoint.x - aCadstarCentre.x ) * KiCadUnitMultiplier;
    retval.y = ( aCadstarPoint.y - aCadstarCentre.y ) * KiCadUnitMultiplier;

    return retval;
}


double CADSTAR_SCH_ARCHIVE_LOADER::getPolarAngle( wxPoint aPoint )
{
    return NormalizeAnglePos( ArcTangente( aPoint.y, aPoint.x ) );
}


double CADSTAR_SCH_ARCHIVE_LOADER::getPolarRadius( wxPoint aPoint )
{
    return sqrt(
            ( (double) aPoint.x * (double) aPoint.x ) + ( (double) aPoint.y * (double) aPoint.y ) );
}
