/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2019 Jean-Pierre Charras, jp.charras at wanadoo.fr
 * Copyright (C) 2008 Wayne Stambaugh <stambaughw@gmail.com>
 * Copyright (C) 2004-2020 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include <pgm_base.h>
#include <confirm.h>
#include <widgets/infobar.h>
#include <tools/ee_actions.h>
#include <tools/lib_drawing_tools.h>
#include <symbol_edit_frame.h>
#include <class_libentry.h>
#include <class_library.h>
#include <template_fieldnames.h>
#include <wildcards_and_files_ext.h>
#include <symbol_lib_table.h>
#include <symbol_library_manager.h>
#include <symbol_tree_pane.h>
#include <widgets/lib_tree.h>
#include <sch_plugins/legacy/sch_legacy_plugin.h>
#include <sch_plugins/kicad/sch_sexpr_plugin.h>
#include <dialogs/dialog_lib_new_component.h>
#include <dialog_helpers.h>
#include <wx/clipbrd.h>


void SYMBOL_EDIT_FRAME::updateTitle()
{
    wxString lib = GetCurLib();
    wxString title;

    if( IsSymbolFromSchematic() )
    {
        title += wxString::Format( _( "%s from schematic" ), m_reference );
        title +=  wxT( " \u2014 " );
    }
    else
    {
        if( GetCurPart() )
            title += GetCurPart()->GetLibId().Format() + wxT( " \u2014 " ) ;

        if( GetCurPart() && m_libMgr && m_libMgr->IsLibraryReadOnly( GetCurLib() ) )
            title += _( "[Read Only Library]" ) + wxT( " \u2014 " );
    }

    title += _( "Symbol Editor" );
    SetTitle( title );
}


void SYMBOL_EDIT_FRAME::SelectActiveLibrary( const wxString& aLibrary )
{
    wxString selectedLib = aLibrary;

    if( selectedLib.empty() )
        selectedLib = SelectLibraryFromList();

    if( !selectedLib.empty() )
        SetCurLib( selectedLib );

    updateTitle();
}


wxString SYMBOL_EDIT_FRAME::SelectLibraryFromList()
{
    PROJECT& prj = Prj();

    if( prj.SchSymbolLibTable()->IsEmpty() )
    {
        ShowInfoBarError( _( "No symbol libraries are loaded." ) );
        return wxEmptyString;
    }

    wxArrayString headers;

    headers.Add( _( "Library" ) );

    std::vector< wxArrayString > itemsToDisplay;
    std::vector< wxString > libNicknames = prj.SchSymbolLibTable()->GetLogicalLibs();

    // Conversion from wxArrayString to vector of ArrayString
    for( const wxString& name : libNicknames )
    {
        wxArrayString item;

        item.Add( name );
        itemsToDisplay.push_back( item );
    }

    wxString oldLibName = prj.GetRString( PROJECT::SCH_LIB_SELECT );

    EDA_LIST_DIALOG dlg( this, _( "Select Symbol Library" ), headers, itemsToDisplay, oldLibName );

    if( dlg.ShowModal() != wxID_OK )
        return wxEmptyString;

    wxString libName = dlg.GetTextSelection();

    if( !libName.empty() )
    {
        if( prj.SchSymbolLibTable()->HasLibrary( libName ) )
            prj.SetRString( PROJECT::SCH_LIB_SELECT, libName );
        else
            libName = wxEmptyString;
    }

    return libName;
}


bool SYMBOL_EDIT_FRAME::saveCurrentPart()
{
    if( GetCurPart() )
    {
        LIB_ID libId = GetCurPart()->GetLibId();
        const wxString& libName = libId.GetLibNickname();
        const wxString& partName = libId.GetLibItemName();

        if( m_libMgr->FlushPart( partName, libName ) )
        {
            m_libMgr->ClearPartModified( partName, libName );
            return true;
        }
    }

    return false;
}


bool SYMBOL_EDIT_FRAME::LoadSymbolAndSelectLib( const LIB_ID& aLibId, int aUnit, int aConvert )
{
    if( GetCurPart() && GetCurPart()->GetLibId() == aLibId
            && GetUnit() == aUnit && GetConvert() == aConvert )
    {
        return true;
    }

    if( GetScreen()->IsModify() && GetCurPart() )
    {
        if( !HandleUnsavedChanges( this, _( "The current symbol has been modified.  "
                                            "Save changes?" ),
                                   [&]()->bool { return saveCurrentPart(); } ) )
        {
            return false;
        }
    }

    SelectActiveLibrary( aLibId.GetLibNickname() );
    return LoadSymbolFromCurrentLib( aLibId.GetLibItemName(), aUnit, aConvert );
}


bool SYMBOL_EDIT_FRAME::LoadSymbolFromCurrentLib( const wxString& aAliasName, int aUnit,
                                                  int aConvert )
{
    LIB_PART* alias = nullptr;

    try
    {
        alias = Prj().SchSymbolLibTable()->LoadSymbol( GetCurLib(), aAliasName );
    }
    catch( const IO_ERROR& ioe )
    {
        wxString msg;

        msg.Printf( _( "Error occurred loading symbol \"%s\" from library \"%s\"." ),
                    aAliasName, GetCurLib() );
        DisplayErrorMessage( this, msg, ioe.What() );
        return false;
    }

    if( !alias || !LoadOneLibraryPartAux( alias, GetCurLib(), aUnit, aConvert ) )
        return false;

    // Enable synchronized pin edit mode for symbols with interchangeable units
    m_SyncPinEdit = !GetCurPart()->UnitsLocked();

    ClearUndoRedoList();
    m_toolManager->RunAction( ACTIONS::zoomFitScreen, true );
    SetShowDeMorgan( GetCurPart()->Flatten()->HasConversion() );

    if( aUnit > 0 )
        RebuildSymbolUnitsList();

    return true;
}


bool SYMBOL_EDIT_FRAME::LoadOneLibraryPartAux( LIB_PART* aEntry, const wxString& aLibrary,
                                               int aUnit, int aConvert )
{
    wxString msg, rootName;
    bool rebuildMenuAndToolbar = false;

    if( !aEntry || aLibrary.empty() )
        return false;

    if( aEntry->GetName().IsEmpty() )
    {
        wxLogWarning( "Symbol in library \"%s\" has empty name field.", aLibrary );
        return false;
    }

    m_toolManager->RunAction( ACTIONS::cancelInteractive, true );

    // Symbols from the schematic are edited in place and not managed by the library manager.
    if( IsSymbolFromSchematic() )
    {
        delete m_my_part;
        m_my_part = nullptr;

        SCH_SCREEN* screen = GetScreen();
        delete screen;
        SetScreen( m_dummyScreen );
        m_isSymbolFromSchematic = false;
        rebuildMenuAndToolbar = true;
    }

    LIB_PART* lib_part = m_libMgr->GetBufferedPart( aEntry->GetName(), aLibrary );
    wxCHECK( lib_part, false );

    m_unit = aUnit > 0 ? aUnit : 1;
    m_convert = aConvert > 0 ? aConvert : 1;

    // The buffered screen for the part
    SCH_SCREEN* part_screen = m_libMgr->GetScreen( lib_part->GetName(), aLibrary );

    SetScreen( part_screen );
    SetCurPart( new LIB_PART( *lib_part ) );
    SetCurLib( aLibrary );

    if( rebuildMenuAndToolbar )
    {
        ReCreateMenuBar();
        ReCreateHToolbar();
        GetInfoBar()->Dismiss();
    }

    m_toolManager->RunAction( ACTIONS::zoomFitScreen, true );
    updateTitle();
    RebuildSymbolUnitsList();
    SetShowDeMorgan( GetCurPart()->HasConversion() );

    // Display the document information based on the entry selected just in
    // case the entry is an alias.
    DisplaySymbolDatasheet();
    Refresh();

    return true;
}


void SYMBOL_EDIT_FRAME::LoadOneSymbol()
{
    EE_SELECTION_TOOL* selTool = m_toolManager->GetTool<EE_SELECTION_TOOL>();

    // Exit if no library entry is selected or a command is in progress.
    if( !m_my_part || !EE_CONDITIONS::Idle( selTool->GetSelection() ) )
        return;

    PROJECT&        prj = Prj();
    SEARCH_STACK*   search = prj.SchSearchS();

    wxString default_path = prj.GetRString( PROJECT::SCH_LIB_PATH );

    if( !default_path )
        default_path = search->LastVisitedPath();

    wxFileDialog dlg( this, _( "Import Symbol" ), default_path,
                      wxEmptyString, SchematicSymbolFileWildcard(),
                      wxFD_OPEN | wxFD_FILE_MUST_EXIST );

    if( dlg.ShowModal() == wxID_CANCEL )
        return;

    wxString filename = dlg.GetPath();

    prj.SetRString( PROJECT::SCH_LIB_PATH, filename );

    wxArrayString symbols;
    SCH_PLUGIN::SCH_PLUGIN_RELEASER pi( SCH_IO_MGR::FindPlugin( SCH_IO_MGR::SCH_LEGACY ) );

    wxString msg;

    try
    {
        pi->EnumerateSymbolLib( symbols, filename );
    }
    catch( const IO_ERROR& ioe )
    {
        msg.Printf( _( "Cannot import symbol library \"%s\"." ), filename );
        DisplayErrorMessage( this, msg, ioe.What() );
        return;
    }

    if( symbols.empty() )
    {
        msg.Printf( _( "Symbol library file \"%s\" is empty." ), filename );
        DisplayError( this,  msg );
        return;
    }

    if( symbols.GetCount() > 1 )
    {
        msg.Printf( _( "More than one symbol found in symbol file \"%s\"." ), filename );
        wxMessageBox( msg, _( "Warning" ), wxOK | wxICON_EXCLAMATION, this );
    }

    LIB_PART* alias = nullptr;

    try
    {
        alias = pi->LoadSymbol( filename, symbols[0] );
    }
    catch( const IO_ERROR& )
    {
        return;
    }

    wxCHECK_RET( alias, "Invalid symbol." );

    SaveCopyInUndoList( m_my_part );

    LIB_PART* first = alias;
    LIB_ITEMS_CONTAINER& drawList = first->GetDrawItems();

    for( LIB_ITEM& item : drawList )
    {
        if( item.Type() == LIB_FIELD_T )
            continue;

        if( item.GetUnit() )
            item.SetUnit( m_unit );

        if( item.GetConvert() )
            item.SetConvert( m_convert );

        item.SetFlags( IS_NEW | SELECTED );

        LIB_ITEM* newItem = (LIB_ITEM*) item.Clone();

        newItem->SetParent( m_my_part );
        m_my_part->AddDrawItem( newItem );
        item.ClearSelected();
    }

    m_my_part->RemoveDuplicateDrawItems();

    OnModify();
}


void SYMBOL_EDIT_FRAME::SaveOneSymbol()
{
    wxCHECK( m_my_part, /* void */ );

    // Export the current part as a symbol (.sym file)
    // this is the current part without its aliases and doc file
    // because a .sym file is used to import graphics in a part being edited
    if( m_my_part->GetDrawItems().empty() )
        return;

    wxString        msg;
    PROJECT&        prj = Prj();
    SEARCH_STACK*   search = prj.SchSearchS();

    wxString default_path = prj.GetRString( PROJECT::SCH_LIB_PATH );

    if( !default_path )
        default_path = search->LastVisitedPath();

    wxFileDialog dlg( this, _( "Export Symbol" ), default_path,
                      m_my_part->GetName() + "." + SchematicSymbolFileExtension,
                      SchematicSymbolFileWildcard(),
                      wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

    if( dlg.ShowModal() == wxID_CANCEL )
        return;

    wxFileName fn = dlg.GetPath();

    /* The GTK file chooser doesn't return the file extension added to
     * file name so add it here. */
    if( fn.GetExt().IsEmpty() )
        fn.SetExt( SchematicSymbolFileExtension );

    prj.SetRString( PROJECT::SCH_LIB_PATH, fn.GetPath() );

    if( fn.FileExists() && !wxRemoveFile( fn.GetFullPath() ) )
        return;

    SetStatusText( wxString::Format( _( "Saving symbol in \"%s\"" ), fn.GetPath() ) );

    SCH_PLUGIN::SCH_PLUGIN_RELEASER plugin( SCH_IO_MGR::FindPlugin( SCH_IO_MGR::SCH_LEGACY ) );

    try
    {
        PROPERTIES nodoc_props;     // Doc file is useless for a .sym file
        nodoc_props[ SCH_LEGACY_PLUGIN::PropNoDocFile ] = "";
        plugin->CreateSymbolLib( fn.GetFullPath(), &nodoc_props );

        // The part gets flattened by the LIB_PART copy constructor.
        LIB_PART* saved_part = new LIB_PART( *m_my_part );
        plugin->SaveSymbol( fn.GetFullPath(), saved_part, &nodoc_props );
    }
    catch( const IO_ERROR& ioe )
    {
        msg.Printf( _( "An error occurred saving symbol file \"%s\"" ), fn.GetFullPath() );
        DisplayErrorMessage( this, msg, ioe.What() );
    }
}


void SYMBOL_EDIT_FRAME::SaveAll()
{
    saveAllLibraries( false );
    m_treePane->GetLibTree()->RefreshLibTree();
    refreshSchematic();
}


void SYMBOL_EDIT_FRAME::CreateNewPart()
{
    m_toolManager->RunAction( ACTIONS::cancelInteractive, true );

    wxArrayString rootSymbols;
    wxString lib = getTargetLib();

    if( !m_libMgr->LibraryExists( lib ) )
    {
        lib = SelectLibraryFromList();

        if( !m_libMgr->LibraryExists( lib ) )
            return;
    }

    m_libMgr->GetRootSymbolNames( lib, rootSymbols );

    rootSymbols.Sort();

    DIALOG_LIB_NEW_COMPONENT dlg( this, &rootSymbols );
    dlg.SetMinSize( dlg.GetSize() );

    if( dlg.ShowModal() == wxID_CANCEL )
        return;

    if( dlg.GetName().IsEmpty() )
    {
        wxMessageBox( _( "This new symbol has no name and cannot be created." ) );
        return;
    }

    wxString name = dlg.GetName();
    // Currently, symbol names cannot include a space, that breaks libraries:
    name.Replace( " ", "_" );

    // Test if there is a component with this name already.
    if( !lib.empty() && m_libMgr->PartExists( name, lib ) )
    {
        wxString msg = wxString::Format( _( "Symbol \"%s\" already exists in library \"%s\"" ),
                                         name, lib );
        DisplayError( this, msg );
        return;
    }

    LIB_PART new_part( name );      // do not create part on the heap, it will be buffered soon

    wxString parentSymbolName = dlg.GetParentSymbolName();

    if( parentSymbolName.IsEmpty() )
    {
        new_part.GetReferenceField().SetText( dlg.GetReference() );
        new_part.SetUnitCount( dlg.GetUnitCount() );

        // Initialize new_part.m_TextInside member:
        // if 0, pin text is outside the body (on the pin)
        // if > 0, pin text is inside the body

        if( dlg.GetPinNameInside() )
        {
            new_part.SetPinNameOffset( dlg.GetPinTextPosition() );

            if( new_part.GetPinNameOffset() == 0 )
                new_part.SetPinNameOffset( 1 );
        }
        else
        {
            new_part.SetPinNameOffset( 0 );
        }

        ( dlg.GetPowerSymbol() ) ? new_part.SetPower() : new_part.SetNormal();
        new_part.SetShowPinNumbers( dlg.GetShowPinNumber() );
        new_part.SetShowPinNames( dlg.GetShowPinName() );
        new_part.LockUnits( dlg.GetLockItems() );
        new_part.SetIncludeInBom( dlg.GetIncludeInBom() );
        new_part.SetIncludeOnBoard( dlg.GetIncludeOnBoard() );

        if( dlg.GetUnitCount() < 2 )
            new_part.LockUnits( false );

        new_part.SetConversion( dlg.GetAlternateBodyStyle() );
        // must be called after loadPart, that calls SetShowDeMorgan, but
        // because the symbol is empty,it looks like it has no alternate body
        SetShowDeMorgan( dlg.GetAlternateBodyStyle() );
    }
    else
    {
        LIB_PART* parent = m_libMgr->GetAlias( parentSymbolName, lib );
        wxCHECK( parent, /* void */ );
        new_part.SetParent( parent );

        // Inherit the parent mandatory field attributes.
        for( int id=0;  id<MANDATORY_FIELDS;  ++id )
        {
            LIB_FIELD* field = new_part.GetField( id );

            // the MANDATORY_FIELDS are exactly that in RAM.
            wxCHECK( field, /* void */ );

            LIB_FIELD* parentField = parent->GetField( id );

            wxCHECK( parentField, /* void */ );

            *field = *parentField;

            switch( id )
            {
            case REFERENCE_FIELD:
                // parent's reference already copied
                break;

            case VALUE_FIELD:
                field->SetText( name );
                break;

            case FOOTPRINT_FIELD:
            case DATASHEET_FIELD:
                // - footprint might be the same as parent, but might not
                // - datasheet is most likely different
                // - probably best to play it safe and copy neither
                field->SetText( wxEmptyString );
                break;
            }

            field->SetParent( &new_part );
        }
    }

    m_libMgr->UpdatePart( &new_part, lib );
    SyncLibraries( false );
    LoadPart( name, lib, 1 );
}


void SYMBOL_EDIT_FRAME::Save()
{
    LIB_ID libId = getTargetLibId();
    const wxString& libName = libId.GetLibNickname();
    const wxString& partName = libId.GetLibItemName();

    if( partName.IsEmpty() )
    {
        saveLibrary( libName, false );
    }
    else if( !libName.IsEmpty() && m_libMgr->IsLibraryReadOnly( libName ) )
    {
        // Force a "Save As..." if the modified library is read only.
        saveLibrary( libName, true );
    }
    else
    {
        // Save a single library.
        if( m_libMgr->FlushPart( partName, libName ) )
            m_libMgr->ClearPartModified( partName, libName );
    }

    m_treePane->GetLibTree()->RefreshLibTree();
    refreshSchematic();
}


void SYMBOL_EDIT_FRAME::SaveAs()
{
    LIB_ID libId = getTargetLibId();
    const wxString& libName = libId.GetLibNickname();
    const wxString& partName = libId.GetLibItemName();

    if( partName.IsEmpty() )
        saveLibrary( libName, true );
    else
        savePartAs();

    m_treePane->GetLibTree()->RefreshLibTree();
    refreshSchematic();
}


void SYMBOL_EDIT_FRAME::savePartAs()
{
    LIB_ID old_lib_id = getTargetLibId();
    wxString old_name = old_lib_id.GetLibItemName();
    wxString old_lib = old_lib_id.GetLibNickname();
    LIB_PART* part = m_libMgr->GetBufferedPart( old_name, old_lib );

    if( part )
    {
        SYMBOL_LIB_TABLE* tbl = Prj().SchSymbolLibTable();
        wxArrayString headers;
        std::vector< wxArrayString > itemsToDisplay;
        std::vector< wxString > libNicknames = tbl->GetLogicalLibs();

        headers.Add( _( "Nickname" ) );
        headers.Add( _( "Description" ) );

        for( const auto& name : libNicknames )
        {
            wxArrayString item;
            item.Add( name );
            item.Add( tbl->GetDescription( name ) );
            itemsToDisplay.push_back( item );
        }

        EDA_LIST_DIALOG dlg( this, _( "Save Copy of Symbol" ), headers, itemsToDisplay, old_lib );
        dlg.SetListLabel( _( "Save in library:" ) );
        dlg.SetOKLabel( _( "Save" ) );

        wxBoxSizer* bNameSizer = new wxBoxSizer( wxHORIZONTAL );

        wxStaticText* label = new wxStaticText( &dlg, wxID_ANY, _( "Name:" ),
                                                wxDefaultPosition, wxDefaultSize, 0 );
        bNameSizer->Add( label, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxLEFT, 5 );

        wxTextCtrl* nameTextCtrl = new wxTextCtrl( &dlg, wxID_ANY, old_name,
                                                   wxDefaultPosition, wxDefaultSize, 0 );
        bNameSizer->Add( nameTextCtrl, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

        wxSizer* mainSizer = dlg.GetSizer();
        mainSizer->Prepend( bNameSizer, 0, wxEXPAND|wxTOP|wxLEFT|wxRIGHT, 5 );

        // Move nameTextCtrl to the head of the tab-order
        if( dlg.GetChildren().DeleteObject( nameTextCtrl ) )
            dlg.GetChildren().Insert( nameTextCtrl );

        dlg.SetInitialFocus( nameTextCtrl );

        dlg.Layout();
        mainSizer->Fit( &dlg );

        if( dlg.ShowModal() != wxID_OK )
            return;                   // canceled by user

        wxString new_lib = dlg.GetTextSelection();

        if( new_lib.IsEmpty() )
        {
            DisplayError( this, _( "No library specified.  Symbol could not be saved." ) );
            return;
        }

        // @todo Either check the selecteced library to see if the parent symbol name is in
        //       the new library and/or copy the parent symbol as well.  This is the lazy
        //       solution to ensure derived parts do not get orphaned.
        if( part->IsAlias() && new_lib != old_lib )
        {
            DisplayError( this, _( "Derived symbols must be save in the same library\n"
                                   "that the parent symbol exists." ) );
            return;
        }

        wxString new_name = nameTextCtrl->GetValue();
        new_name.Trim( true );
        new_name.Trim( false );
        new_name.Replace( " ", "_" );

        if( new_name.IsEmpty() )
        {
            // This is effectively a cancel.  No need to nag the user about it.
            return;
        }

        // Test if there is a component with this name already.
        if( m_libMgr->PartExists( new_name, new_lib ) )
        {
            wxString msg = wxString::Format( _( "Symbol \"%s\" already exists in library \"%s\"" ),
                                             new_name, new_lib );
            DisplayError( this, msg );
            return;
        }

        LIB_PART new_part( *part );
        new_part.SetName( new_name );

        m_libMgr->UpdatePart( &new_part, new_lib );
        SyncLibraries( false );
        m_treePane->GetLibTree()->SelectLibId( LIB_ID( new_lib, new_part.GetName() ) );
        LoadPart( new_name, new_lib, m_unit );
    }
}


void SYMBOL_EDIT_FRAME::UpdateAfterSymbolProperties( wxString* aOldName )
{
    wxCHECK( m_my_part, /* void */ );

    wxString  msg;
    wxString  lib = GetCurLib();

    if( aOldName && *aOldName != m_my_part->GetName() )
    {
        // Test the current library for name conflicts
        if( !lib.empty() && m_libMgr->PartExists( m_my_part->GetName(), lib ) )
        {
            msg.Printf( _( "The name '%s' conflicts with an existing entry in the library '%s'." ),
                        m_my_part->GetName(),
                        lib );

            DisplayErrorMessage( this, msg );
            m_my_part->SetName( *aOldName );
        }
        else
        {
            m_libMgr->UpdatePartAfterRename( m_my_part, *aOldName, lib );
        }

        // Reselect the renamed part
        m_treePane->GetLibTree()->SelectLibId( LIB_ID( lib, m_my_part->GetName() ) );
    }

    RebuildSymbolUnitsList();
    SetShowDeMorgan( GetCurPart()->Flatten()->HasConversion() );
    updateTitle();
    DisplaySymbolDatasheet();

    RebuildView();
    OnModify();
}


void SYMBOL_EDIT_FRAME::DeletePartFromLibrary()
{
    LIB_ID libId = getTargetLibId();

    if( m_libMgr->IsPartModified( libId.GetLibItemName(), libId.GetLibNickname() )
        && !IsOK( this, _( wxString::Format( "The symbol \"%s\" has been modified\n"
                                             "Do you want to remove it from the library?",
                                             libId.GetUniStringLibItemName() ) ) ) )
    {
        return;
    }

    if( m_libMgr->HasDerivedSymbols( libId.GetLibItemName(), libId.GetLibNickname() ) )
    {
        wxString msg;

        msg.Printf( _( "The symbol \"%s\" is used to derive other symbols.\n"
                       "Deleting this symbol will delete all of the symbols derived from it.\n\n"
                       "Do you wish to delete this symbol and all of it's derivatives?" ),
                    libId.GetLibItemName().wx_str() );

        wxMessageDialog::ButtonLabel yesButtonLabel( _( "Delete Symbol" ) );
        wxMessageDialog::ButtonLabel noButtonLabel( _( "Keep Symbol" ) );

        wxMessageDialog dlg( this, msg, _( "Warning" ),
                             wxYES_NO | wxYES_DEFAULT | wxICON_QUESTION | wxCENTER );
        dlg.SetYesNoLabels( yesButtonLabel, noButtonLabel );

        if( dlg.ShowModal() == wxID_NO )
            return;
    }

    if( isCurrentPart( libId ) )
        emptyScreen();

    m_libMgr->RemovePart( libId.GetLibItemName(), libId.GetLibNickname() );

    m_treePane->GetLibTree()->RefreshLibTree();
    refreshSchematic();
}


void SYMBOL_EDIT_FRAME::CopyPartToClipboard()
{
    int dummyUnit;
    LIB_ID libId = m_treePane->GetLibTree()->GetSelectedLibId( &dummyUnit );
    LIB_PART* part = m_libMgr->GetBufferedPart( libId.GetLibItemName(), libId.GetLibNickname() );

    if( !part )
        return;

    std::unique_ptr< LIB_PART> tmp = part->Flatten();
    STRING_FORMATTER formatter;
    SCH_SEXPR_PLUGIN::FormatPart( tmp.get(), formatter );

    auto clipboard = wxTheClipboard;
    wxClipboardLocker clipboardLock( clipboard );

    if( !clipboardLock || !clipboard->IsOpened() )
        return;

    auto data = new wxTextDataObject( wxString( formatter.GetString().c_str(), wxConvUTF8 ) );
    clipboard->SetData( data );

    clipboard->Flush();
}


void SYMBOL_EDIT_FRAME::DuplicatePart( bool aFromClipboard )
{
    int dummyUnit;
    LIB_ID libId = m_treePane->GetLibTree()->GetSelectedLibId( &dummyUnit );
    wxString lib = libId.GetLibNickname();

    if( !m_libMgr->LibraryExists( lib ) )
        return;

    LIB_PART* srcPart = nullptr;
    LIB_PART* newPart = nullptr;

    if( aFromClipboard )
    {
        auto clipboard = wxTheClipboard;
        wxClipboardLocker clipboardLock( clipboard );

        if( !clipboardLock || ! clipboard->IsSupported( wxDF_TEXT ) )
            return;

        wxTextDataObject data;
        clipboard->GetData( data );
        wxString partSource = data.GetText();

        STRING_LINE_READER reader( TO_UTF8( partSource ), "Clipboard" );

        try
        {
            newPart = SCH_SEXPR_PLUGIN::ParsePart( reader );
        }
        catch( IO_ERROR& e )
        {
            wxLogMessage( "Can not paste: %s", e.Problem() );
            return;
        }
    }
    else
    {
        srcPart = m_libMgr->GetBufferedPart( libId.GetLibItemName(), lib );

        wxCHECK( srcPart, /* void */ );

        newPart = new LIB_PART( *srcPart );

        // Derive from same parent.
        if( srcPart->IsAlias() )
        {
            std::shared_ptr< LIB_PART > srcParent = srcPart->GetParent().lock();

            wxCHECK( srcParent, /* void */ );

            newPart->SetParent( srcParent.get() );
        }
    }

    if( !newPart )
        return;

    ensureUniqueName( newPart, lib );
    m_libMgr->UpdatePart( newPart, lib );
    SyncLibraries( false );
    m_treePane->GetLibTree()->SelectLibId( LIB_ID( lib, newPart->GetName() ) );

    delete newPart;
}


void SYMBOL_EDIT_FRAME::ensureUniqueName( LIB_PART* aPart, const wxString& aLibrary )
{
    wxCHECK( aPart, /* void */ );

    int      i = 1;
    wxString newName = aPart->GetName();

    // Append a number to the name until the name is unique in the library.
    while( m_libMgr->PartExists( newName, aLibrary ) )
        newName.Printf( "%s_%d", aPart->GetName(), i++ );

    aPart->SetName( newName );
}


void SYMBOL_EDIT_FRAME::Revert( bool aConfirm )
{
    LIB_ID libId = getTargetLibId();
    const wxString& libName = libId.GetLibNickname();

    // Empty if this is the library itself that is selected.
    const wxString& partName = libId.GetLibItemName();

    wxString msg = wxString::Format( _( "Revert \"%s\" to last version saved?" ),
                                     partName.IsEmpty() ? libName : partName );

    if( aConfirm && !ConfirmRevertDialog( this, msg ) )
        return;

    bool reload_currentPart = false;
    wxString curr_partName = partName;

    if( GetCurPart() )
    {
        // the library itself is reverted: the current part will be reloaded only if it is
        // owned by this library
        if( partName.IsEmpty() )
        {
            LIB_ID curr_libId = GetCurPart()->GetLibId();
            reload_currentPart = libName == curr_libId.GetLibNickname();

            if( reload_currentPart )
                curr_partName = curr_libId.GetLibItemName();
        }
        else
        {
            reload_currentPart = isCurrentPart( libId );
        }
    }

    int unit = m_unit;

    if( reload_currentPart )
        emptyScreen();

    if( partName.IsEmpty() )
    {
        m_libMgr->RevertLibrary( libName );
    }
    else
    {
        libId = m_libMgr->RevertPart( libId.GetLibItemName(), libId.GetLibNickname() );

        m_treePane->GetLibTree()->SelectLibId( libId );
        m_libMgr->ClearPartModified( libId.GetLibItemName(), libId.GetLibNickname() );
    }

    if( reload_currentPart && m_libMgr->PartExists( curr_partName, libName ) )
        LoadPart( curr_partName, libName, unit );

    m_treePane->Refresh();
    refreshSchematic();
}


void SYMBOL_EDIT_FRAME::RevertAll()
{
    wxCHECK_RET( m_libMgr, "Library manager object not created." );

    Revert( false );
    m_libMgr->RevertAll();
}


void SYMBOL_EDIT_FRAME::LoadPart( const wxString& aAlias, const wxString& aLibrary, int aUnit )
{
    LIB_PART* part = m_libMgr->GetBufferedPart( aAlias, aLibrary );

    if( !part )
    {
        wxString msg;

        msg.Printf( _( "Symbol name \"%s\" not found in library \"%s\"" ), aAlias, aLibrary );
        DisplayError( this, msg );
        return;
    }

    // Optimize default edit options for this symbol
    // Usually if units are locked, graphic items are specific to each unit
    // and if units are interchangeable, graphic items are common to units
    LIB_DRAWING_TOOLS* tools = GetToolManager()->GetTool<LIB_DRAWING_TOOLS>();
    tools->SetDrawSpecificUnit( part->UnitsLocked() );

    LoadOneLibraryPartAux( part, aLibrary, aUnit, 0 );
}


bool SYMBOL_EDIT_FRAME::saveLibrary( const wxString& aLibrary, bool aNewFile )
{
    wxFileName fn;
    wxString   msg;
    SCH_IO_MGR::SCH_FILE_T fileType = SCH_IO_MGR::SCH_FILE_T::SCH_KICAD;
    PROJECT&   prj = Prj();

    m_toolManager->RunAction( ACTIONS::cancelInteractive, true );

    if( !aNewFile && ( aLibrary.empty() || !prj.SchSymbolLibTable()->HasLibrary( aLibrary ) ) )
    {
        ShowInfoBarError( _( "No library specified." ) );
        return false;
    }

    if( aNewFile )
    {
        SEARCH_STACK* search = prj.SchSearchS();

        // Get a new name for the library
        wxString default_path = prj.GetRString( PROJECT::SCH_LIB_PATH );

        if( !default_path )
            default_path = search->LastVisitedPath();

        fn.SetName( aLibrary );
        fn.SetExt( KiCadSymbolLibFileExtension );

        wxString wildcards = KiCadSymbolLibFileWildcard();

        wxFileDialog dlg( this, wxString::Format( _( "Save Library \"%s\" As..." ), aLibrary ),
                          default_path, fn.GetFullName(), wildcards,
                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

        if( dlg.ShowModal() == wxID_CANCEL )
            return false;

        fn = dlg.GetPath();

        prj.SetRString( PROJECT::SCH_LIB_PATH, fn.GetPath() );

        if( fn.GetExt().IsEmpty() )
            fn.SetExt( KiCadSymbolLibFileExtension );
    }
    else
    {
        fn = prj.SchSymbolLibTable()->GetFullURI( aLibrary );
        fileType = SCH_IO_MGR::GuessPluginTypeFromLibPath( fn.GetFullPath() );
    }

    // Verify the user has write privileges before attempting to save the library file.
    if( !aNewFile && m_libMgr->IsLibraryReadOnly( aLibrary ) )
        return false;

    ClearMsgPanel();

    // Copy .kicad_symb file to .bak.
    if( !backupFile( fn, "bak" ) )
        return false;

    if( !m_libMgr->SaveLibrary( aLibrary, fn.GetFullPath(), fileType ) )
    {
        msg.Printf( _( "Failed to save changes to symbol library file \"%s\"" ),
                    fn.GetFullPath() );
        DisplayErrorMessage( this, _( "Error saving library" ), msg );
        return false;
    }

    if( !aNewFile )
        m_libMgr->ClearLibraryModified( aLibrary );

    ClearMsgPanel();
    msg.Printf( _( "Symbol library file \"%s\" saved" ), fn.GetFullPath() );
    RebuildSymbolUnitsList();

    return true;
}


bool SYMBOL_EDIT_FRAME::saveAllLibraries( bool aRequireConfirmation )
{
    wxString msg;
    bool doSave = true;
    int dirtyCount = 0;
    bool applyToAll = false;
    bool retv = true;

    for( const auto& libNickname : m_libMgr->GetLibraryNames() )
    {
        if( m_libMgr->IsLibraryModified( libNickname ) )
            dirtyCount++;
    }

    for( const auto& libNickname : m_libMgr->GetLibraryNames() )
    {
        if( m_libMgr->IsLibraryModified( libNickname ) )
        {
            if( aRequireConfirmation && !applyToAll )
            {
                msg.Printf( _( "Save changes to \"%s\" before closing?" ), libNickname );

                switch( UnsavedChangesDialog( this, msg, dirtyCount > 1 ? &applyToAll : nullptr ) )
                {
                case wxID_YES: doSave = true;  break;
                case wxID_NO:  doSave = false; break;
                default:
                case wxID_CANCEL: return false;
                }
            }

            if( doSave )
            {
                // If saving under existing name fails then do a Save As..., and if that
                // fails then cancel close action.
                if( m_libMgr->IsLibraryReadOnly( libNickname ) )
                {
                    m_infoBar->Dismiss();
                    msg.Printf( _( "Library \"%s\" is read only and must be saved as a "
                                   "different library." ), libNickname );
                    m_infoBar->ShowMessageFor( msg, 3000, wxICON_EXCLAMATION );
                    retv = false;
                    continue;
                }

                if( saveLibrary( libNickname, false ) )
                    continue;

                if( !saveLibrary( libNickname, true ) )
                    retv = false;
            }
        }
    }

    return retv;
}


void SYMBOL_EDIT_FRAME::DisplaySymbolDatasheet()
{
    EDA_DRAW_FRAME::ClearMsgPanel();

    if( !m_my_part )
        return;

    wxString msg = m_my_part->GetName();

    AppendMsgPanel( _( "Name" ), msg, 8 );

    if( m_my_part->IsAlias() )
    {
        PART_SPTR parent = m_my_part->GetParent().lock();

        msg = parent ? parent->GetName() : _( "Undefined!" );
        AppendMsgPanel( _( "Parent" ), msg, 8 );
    }

    static wxChar UnitLetter[] = wxT( "?ABCDEFGHIJKLMNOPQRSTUVWXYZ" );
    msg = UnitLetter[m_unit];

    AppendMsgPanel( _( "Unit" ), msg, 8 );

    if( m_convert > 1 )
        msg = _( "Convert" );
    else
        msg = _( "Normal" );

    AppendMsgPanel( _( "Body" ), msg, 8 );

    if( m_my_part->IsPower() )
        msg = _( "Power Symbol" );
    else
        msg = _( "Symbol" );

    AppendMsgPanel( _( "Type" ), msg, 8 );
    AppendMsgPanel( _( "Description" ), m_my_part->GetDescription(), 8 );
    AppendMsgPanel( _( "Keywords" ), m_my_part->GetKeyWords() );
    AppendMsgPanel( _( "Datasheet" ), m_my_part->GetDatasheetField().GetText() );
}


