/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2004 Jean-Pierre Charras, jaen-pierre.charras@gipsa-lab.inpg.com
 * Copyright (C) 2004-2022 KiCad Developers, see AUTHORS.txt for contributors.
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

#ifndef LIB_SHAPE_H
#define LIB_SHAPE_H

#include <lib_item.h>
#include <eda_shape.h>


class LIB_SHAPE : public LIB_ITEM, public EDA_SHAPE
{
public:
    LIB_SHAPE( LIB_SYMBOL* aParent, SHAPE_T aShape = SHAPE_T::UNDEFINED, int aLineWidth = 0,
               FILL_T aFillType = FILL_T::NO_FILL, KICAD_T aType = LIB_SHAPE_T );

    // Do not create a copy constructor.  The one generated by the compiler is adequate.

    ~LIB_SHAPE() { }

    wxString GetClass() const override
    {
        return wxT( "LIB_SHAPE" );
    }

    static inline bool ClassOf( const EDA_ITEM* aItem )
    {
        return aItem && aItem->Type() == LIB_SHAPE_T;
    }

    wxString GetTypeName() const override
    {
        return ShowShape();
    }

    STROKE_PARAMS GetStroke() const { return m_stroke; }
    void SetStroke( const STROKE_PARAMS& aStroke ) { m_stroke = aStroke; }

    bool HitTest( const VECTOR2I& aPosition, int aAccuracy = 0 ) const override;
    bool HitTest( const BOX2I& aRect, bool aContained, int aAccuracy = 0 ) const override;

    int GetPenWidth() const override;

    LINE_STYLE GetEffectiveLineStyle() const
    {
        if( m_stroke.GetLineStyle() == LINE_STYLE::DEFAULT )
            return LINE_STYLE::SOLID;
        else
            return m_stroke.GetLineStyle();
    }

    const BOX2I GetBoundingBox() const override;

    void GetMsgPanelInfo( EDA_DRAW_FRAME* aFrame, std::vector<MSG_PANEL_ITEM>& aList ) override;

    void BeginEdit( const VECTOR2I& aStartPoint ) override  { beginEdit( aStartPoint ); }
    bool ContinueEdit( const VECTOR2I& aPosition ) override { return continueEdit( aPosition ); }
    void CalcEdit( const VECTOR2I& aPosition ) override     { calcEdit( aPosition ); }

    void EndEdit( bool aClosed = false ) override           { endEdit( aClosed ); }
    void SetEditState( int aState )                         { setEditState( aState ); }

    void AddPoint( const VECTOR2I& aPosition );

    void Offset( const VECTOR2I& aOffset ) override;

    void MoveTo( const VECTOR2I& aPosition ) override;

    VECTOR2I GetPosition() const override                      { return getPosition(); }
    void     SetPosition( const VECTOR2I& aPosition ) override { setPosition( aPosition ); }

    VECTOR2I GetCenter() const { return getCenter(); }

    /**
     * Make a set of SHAPE objects representing the LIB_SHAPE.  Caller owns the objects.
     *
     * @param aEdgeOnly indicates only edges should be generated (even if 0 width), and no fill
     *                  shapes.
     */
    virtual std::vector<SHAPE*> MakeEffectiveShapes( bool aEdgeOnly = false ) const override
    {
        return makeEffectiveShapes( aEdgeOnly, true );
    }

    void Normalize();

    void MirrorHorizontally( int aCenter ) override;
    void MirrorVertically( int aCenter ) override;
    void Rotate( const VECTOR2I& aCenter, bool aRotateCCW = true ) override;

    void Print( const SCH_RENDER_SETTINGS* aSettings, int aUnit, int aBodyStyle,
                const VECTOR2I& aOffset, bool aForceNoFill, bool aDimmed ) override;

    void Plot( PLOTTER* aPlotter, bool aBackground, const SCH_PLOT_OPTS& aPlotOpts,
               int aUnit, int aBodyStyle, const VECTOR2I& aOffset, bool aDimmed ) override;

    wxString GetItemDescription( UNITS_PROVIDER* aUnitsProvider ) const override;

    BITMAPS GetMenuImage() const override;

    EDA_ITEM* Clone() const override;

    void ViewGetLayers( int aLayers[], int& aCount ) const override;

    double Similarity( const LIB_ITEM& aOther ) const override;

    bool operator==( const LIB_ITEM& aOther ) const override;

private:
    /**
     * @copydoc LIB_ITEM::compare()
     *
     * The circle specific sort order is as follows:
     *      - Circle horizontal (X) position.
     *      - Circle vertical (Y) position.
     *      - Circle radius.
     */
    int compare( const LIB_ITEM& aOther, int aCompareFlags = 0 ) const override;
};


#endif    // LIB_SHAPE_H
