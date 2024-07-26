/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2009 SoftPLC Corporation, Dick Hollenbeck <dick@softplc.com>
 * Copyright (C) 2009 Jean-Pierre Charras, jean-pierre.charras@inpg.fr
 * Copyright (C) 2009-2022 KiCad Developers, see change_log.txt for contributors.
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

#include <algorithm>
#include <netclass.h>
#include <macros.h>
#include <base_units.h>

// This will get mapped to "kicad_default" in the specctra_export.
const char NETCLASS::Default[] = "Default";

// Initial values for netclass initialization
const int DEFAULT_CLEARANCE        = pcbIUScale.mmToIU( 0.2 ); // track to track and track to pads clearance
const int DEFAULT_VIA_DIAMETER     = pcbIUScale.mmToIU( 0.6 );
const int DEFAULT_VIA_DRILL        = pcbIUScale.mmToIU( 0.3 );
const int DEFAULT_UVIA_DIAMETER    = pcbIUScale.mmToIU( 0.3 );
const int DEFAULT_UVIA_DRILL       = pcbIUScale.mmToIU( 0.1 );
const int DEFAULT_TRACK_WIDTH      = pcbIUScale.mmToIU( 0.2 );
const int DEFAULT_DIFF_PAIR_WIDTH  = pcbIUScale.mmToIU( 0.2 );
const int DEFAULT_DIFF_PAIR_GAP    = pcbIUScale.mmToIU( 0.25 );
const int DEFAULT_DIFF_PAIR_VIAGAP = pcbIUScale.mmToIU( 0.25 );

const int DEFAULT_WIRE_WIDTH       = schIUScale.MilsToIU( 6 );
const int DEFAULT_BUS_WIDTH        = schIUScale.MilsToIU( 12 );

const int DEFAULT_LINE_STYLE       = 0; // solid


NETCLASS::NETCLASS( const wxString& aName, bool aInitWithDefaults ) : m_isDefault( false )
{
    m_constituents.push_back( this );

    SetName( aName );
    SetPriority( -1 );

    // Colors are a special optional case - always set, but UNSPECIFIED used in place of optional
    SetPcbColor( COLOR4D::UNSPECIFIED );
    SetSchematicColor( COLOR4D::UNSPECIFIED );

    if( aInitWithDefaults )
    {
        SetClearance( DEFAULT_CLEARANCE );
        SetViaDrill( DEFAULT_VIA_DRILL );
        SetuViaDrill( DEFAULT_UVIA_DRILL );
        SetTrackWidth( DEFAULT_TRACK_WIDTH );
        SetViaDiameter( DEFAULT_VIA_DIAMETER );
        SetuViaDiameter( DEFAULT_UVIA_DIAMETER );
        SetDiffPairWidth( DEFAULT_DIFF_PAIR_WIDTH );
        SetDiffPairGap( DEFAULT_DIFF_PAIR_GAP );
        SetDiffPairViaGap( DEFAULT_DIFF_PAIR_VIAGAP );

        SetWireWidth( DEFAULT_WIRE_WIDTH );
        SetBusWidth( DEFAULT_BUS_WIDTH );
        SetLineStyle( DEFAULT_LINE_STYLE );
    }

    ResetParents();
}


void NETCLASS::ResetParents()
{
    SetClearanceParent( this );
    SetTrackWidthParent( this );
    SetViaDiameterParent( this );
    SetViaDrillParent( this );
    SetuViaDiameterParent( this );
    SetuViaDrillParent( this );
    SetDiffPairWidthParent( this );
    SetDiffPairGapParent( this );
    SetDiffPairViaGapParent( this );
    SetPcbColorParent( this );
    SetWireWidthParent( this );
    SetBusWidthParent( this );
    SetSchematicColorParent( this );
    SetLineStyleParent( this );
}


void NETCLASS::ResetParameters()
{
    SetPcbColor( COLOR4D::UNSPECIFIED );
    SetSchematicColor( COLOR4D::UNSPECIFIED );
    SetClearance( std::optional<int>() );
    SetViaDrill( std::optional<int>() );
    SetuViaDrill( std::optional<int>() );
    SetTrackWidth( std::optional<int>() );
    SetViaDiameter( std::optional<int>() );
    SetuViaDiameter( std::optional<int>() );
    SetDiffPairWidth( std::optional<int>() );
    SetDiffPairGap( std::optional<int>() );
    SetDiffPairViaGap( std::optional<int>() );
    SetWireWidth( std::optional<int>() );
    SetBusWidth( std::optional<int>() );
    SetLineStyle( std::optional<int>() );

    ResetParents();
}


bool NETCLASS::operator==( const NETCLASS& other ) const
{
    return m_constituents == other.m_constituents;
}


const std::vector<NETCLASS*>& NETCLASS::GetConstituentNetclasses() const
{
    return m_constituents;
}


void NETCLASS::SetConstituentNetclasses( std::vector<NETCLASS*>&& constituents )
{
    m_constituents = std::move( constituents );
}


bool NETCLASS::ContainsNetclassWithName( const wxString& netclass ) const
{
    return std::any_of( m_constituents.begin(), m_constituents.end(),
                        [&netclass]( const NETCLASS* nc )
                        {
                            return nc->GetName() == netclass;
                        } );
}


const wxString NETCLASS::GetName() const
{
    if( m_constituents.size() == 1 )
        return m_Name;

    wxASSERT( m_constituents.size() >= 2 );

    wxString name;

    if( m_constituents.size() == 2 )
    {
        name.Printf( _( "%s and %s" ), m_constituents[0]->GetName(), m_constituents[1]->GetName() );
    }
    else if( m_constituents.size() == 3 )
    {
        name.Printf( _( "%s, %s and %s" ), m_constituents[0]->GetName(),
                     m_constituents[1]->GetName(), m_constituents[2]->GetName() );
    }
    else if( m_constituents.size() > 3 )
    {
        name.Printf( _( "%s, %s and %d more" ), m_constituents[0]->GetName(),
                     m_constituents[1]->GetName(), static_cast<int>( m_constituents.size() - 2 ) );
    }

    return name;
}


const wxString NETCLASS::GetVariableSubstitutionName() const
{
    if( m_constituents.size() == 1 )
        return m_Name;

    wxASSERT( m_constituents.size() >= 2 );

    wxString name = m_constituents[0]->GetName();

    for( std::size_t i = 1; i < m_constituents.size(); ++i )
    {
        name += ",";
        name += m_constituents[i]->GetName();
    }

    return name;
}
