/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2009 Jean-Pierre Charras, jaen-pierre.charras@gipsa-lab.inpg.com
 * Copyright (C) 1992-2021 KiCad Developers, see AUTHORS.txt for contributors.
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

/**
 * @file sch_no_connect.h
 */

#ifndef _SCH_NO_CONNECT_H_
#define _SCH_NO_CONNECT_H_


#include <sch_item.h>


class NETLIST_OBJECT_LIST;

class SCH_NO_CONNECT : public SCH_ITEM
{
public:
    SCH_NO_CONNECT( const VECTOR2I& pos = VECTOR2I( 0, 0 ) );

    // Do not create a copy constructor.  The one generated by the compiler is adequate.

    ~SCH_NO_CONNECT() { }

    static inline bool ClassOf( const EDA_ITEM* aItem )
    {
        return aItem && SCH_NO_CONNECT_T == aItem->Type();
    }

    wxString GetClass() const override
    {
        return wxT( "SCH_NO_CONNECT" );
    }

    int GetSize() const
    {
        return m_size;
    }

    int GetPenWidth() const override;

    void SwapData( SCH_ITEM* aItem ) override;

    void ViewGetLayers( int aLayers[], int& aCount ) const override;

    void GetEndPoints( std::vector< DANGLING_END_ITEM >& aItemList ) override;

    const BOX2I GetBoundingBox() const override;

    // Geometric transforms (used in block operations):

    void Move( const VECTOR2I& aMoveVector ) override
    {
        m_pos += aMoveVector;
    }

    void MirrorHorizontally( int aCenter ) override;
    void MirrorVertically( int aCenter ) override;
    void Rotate( const VECTOR2I& aCenter, bool aRotateCCW ) override;

    bool IsConnectable() const override { return true; }

    bool HasConnectivityChanges( const SCH_ITEM* aItem,
                                 const SCH_SHEET_PATH* aInstance = nullptr ) const override;

    bool CanConnect( const SCH_ITEM* aItem ) const override
    {
        return ( aItem->Type() == SCH_LINE_T && aItem->GetLayer() == LAYER_WIRE ) ||
                 aItem->Type() == SCH_SYMBOL_T;
    }

    std::vector<VECTOR2I> GetConnectionPoints() const override;

    wxString GetItemDescription( UNITS_PROVIDER* aUnitsProvider ) const override
    {
        return wxString( _( "No Connect" ) );
    }

    BITMAPS GetMenuImage() const override;

    VECTOR2I GetPosition() const override { return m_pos; }
    void     SetPosition( const VECTOR2I& aPosition ) override { m_pos = aPosition; }

    bool HitTest( const VECTOR2I& aPosition, int aAccuracy = 0 ) const override;
    bool HitTest( const BOX2I& aRect, bool aContained, int aAccuracy = 0 ) const override;

    void Print( const SCH_RENDER_SETTINGS* aSettings, int aUnit, int aBodyStyle,
                const VECTOR2I& aOffset, bool aForceNoFill, bool aDimmed ) override;

    void Plot( PLOTTER* aPlotter, bool aBackground, const SCH_PLOT_OPTS& aPlotOpts,
               int aUnit, int aBodyStyle, const VECTOR2I& aOffset, bool aDimmed ) override;

    EDA_ITEM* Clone() const override;

    double Similarity( const SCH_ITEM& aOther ) const override;

    bool operator==( const SCH_ITEM& aOther ) const override;

#if defined(DEBUG)
    void Show( int nestLevel, std::ostream& os ) const override { ShowDummy( os ); }
#endif

private:
    bool doIsConnected( const VECTOR2I& aPosition ) const override;

    VECTOR2I m_pos;                 ///< Position of the no connect object.
    int      m_size;                ///< Size of the no connect object.
};


#endif    // _SCH_NO_CONNECT_H_
