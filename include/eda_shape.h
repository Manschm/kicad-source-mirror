/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2018 Jean-Pierre Charras jp.charras at wanadoo.fr
 * Copyright (C) 1992-2022 KiCad Developers, see AUTHORS.txt for contributors.
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

#ifndef EDA_SHAPE_H
#define EDA_SHAPE_H

#include <trigo.h>
#include <geometry/shape_poly_set.h>
#include <geometry/geometry_utils.h>
#include <stroke_params.h>

class LINE_READER;
class EDA_DRAW_FRAME;
class FOOTPRINT;
class MSG_PANEL_ITEM;

using KIGFX::COLOR4D;

enum class SHAPE_T : int
{
    SEGMENT = 0,
    RECT,
    ARC,
    CIRCLE,
    POLY,
    BEZIER,
    LAST          ///< marker for list end
};


// WARNING: Do not change these values without updating dialogs that depend on their position values
enum class FILL_T : int
{
    NO_FILL = 1,
    FILLED_SHAPE,               // Fill with object color
    FILLED_WITH_BG_BODYCOLOR,   // Fill with background body color
    FILLED_WITH_COLOR           // Fill with a separate color
};


// Holding struct to keep originating midpoint
struct ARC_MID
{
    VECTOR2I mid;
    VECTOR2I start;
    VECTOR2I end;
    VECTOR2I center;
};

class EDA_SHAPE
{
public:
    EDA_SHAPE( SHAPE_T aType, int aLineWidth, FILL_T aFill );

    // Do not create a copy constructor & operator=.
    // The ones generated by the compiler are adequate.

    virtual ~EDA_SHAPE();

    void SwapShape( EDA_SHAPE* aImage );

    wxString ShowShape() const;

    wxString SHAPE_T_asString() const;

    bool IsAnnotationProxy() const { return m_annotationProxy; }
    void SetIsAnnotationProxy( bool aIsProxy = true ) { m_annotationProxy = aIsProxy; }

    bool IsFilled() const
    {
        return GetFillMode() != FILL_T::NO_FILL;
    }

    void SetFilled( bool aFlag )
    {
        m_fill = aFlag ? FILL_T::FILLED_SHAPE : FILL_T::NO_FILL;
    }

    void SetFillMode( FILL_T aFill )           { m_fill = aFill; }
    FILL_T GetFillMode() const                 { return m_fill; }

    COLOR4D GetFillColor() const               { return m_fillColor; }
    void SetFillColor( const COLOR4D& aColor ) { m_fillColor = aColor; }

    void SetWidth( int aWidth )                { m_stroke.SetWidth( aWidth ); }
    int GetWidth() const                       { return m_stroke.GetWidth(); }
    virtual int GetEffectiveWidth() const      { return GetWidth(); }

    void SetShape( SHAPE_T aShape )            { m_shape = aShape; }
    SHAPE_T GetShape() const                   { return m_shape; }

    /**
     * Return the starting point of the graphic.
     */
    const VECTOR2I& GetStart() const { return m_start; }
    int             GetStartY() const { return m_start.y; }
    int             GetStartX() const { return m_start.x; }

    void SetStart( const VECTOR2I& aStart )
    {
        m_start = aStart;
        m_endsSwapped = false;
    }

    void SetStartY( int y )
    {
        m_start.y = y;
        m_endsSwapped = false;
    }

    void SetStartX( int x )
    {
        m_start.x = x;
        m_endsSwapped = false;
    }

    /**
     * Return the ending point of the graphic.
     */
    const VECTOR2I& GetEnd() const { return m_end; }
    int             GetEndY() const { return m_end.y; }
    int             GetEndX() const { return m_end.x; }

    void SetEnd( const VECTOR2I& aEnd )
    {
        m_end = aEnd;
        m_endsSwapped = false;
    }

    void SetEndY( int y )
    {
        m_end.y = y;
        m_endsSwapped = false;
    }

    void SetEndX( int x )
    {
        m_end.x = x;
        m_endsSwapped = false;
    }

    virtual VECTOR2I GetTopLeft() const { return GetStart(); }
    virtual VECTOR2I GetBotRight() const { return GetEnd(); }

    virtual void SetTop( int val ) { SetStartY( val ); }
    virtual void SetLeft( int val ) { SetStartX( val ); }
    virtual void SetRight( int val ) { SetEndX( val ); }
    virtual void SetBottom( int val ) { SetEndY( val ); }

    void            SetBezierC1( const VECTOR2I& aPt ) { m_bezierC1 = aPt; }
    const VECTOR2I& GetBezierC1() const { return m_bezierC1; }

    void            SetBezierC2( const VECTOR2I& aPt ) { m_bezierC2 = aPt; }
    const VECTOR2I& GetBezierC2() const { return m_bezierC2; }

    VECTOR2I getCenter() const;
    void     SetCenter( const VECTOR2I& aCenter );

    /**
     * Set the end point from the angle center and start.
     */
    void SetArcAngleAndEnd( const EDA_ANGLE& aAngle, bool aCheckNegativeAngle = false );

    EDA_ANGLE GetArcAngle() const;

    /**
     * Have the start and end points been swapped since they were set?
     * @return true if they have
    */
    bool EndsSwapped() const { return m_endsSwapped; }

    // Some attributes are read only, since they are derived from m_Start, m_End, and m_Angle.
    // No Set...() function for these attributes.

    VECTOR2I              GetArcMid() const;
    std::vector<VECTOR2I> GetRectCorners() const;

    /**
     * Calc arc start and end angles such that aStartAngle < aEndAngle.  Each may be between
     * -360.0 and 360.0.
     */
    void CalcArcAngles( EDA_ANGLE& aStartAngle, EDA_ANGLE& aEndAngle ) const;

    int GetRadius() const;

    /**
     * Set the three controlling points for an arc.
     *
     * NB: these are NOT what's currently stored, so we have to do some calculations behind
     * the scenes.  However, they are what SHOULD be stored.
     */
    void SetArcGeometry( const VECTOR2I& aStart, const VECTOR2I& aMid, const VECTOR2I& aEnd );

    /**
     * Set the data used for mid point caching.  If the controlling points remain constant, then
     * we keep the midpoint the same as it was when read in.  This minimizes VCS churn
     *
     * @param aStart Cached start point
     * @param aMid Cached mid point
     * @param aEnd Cached end point
     * @param aCenter Calculated center point using the preceeding three
     */
    void SetCachedArcData( const VECTOR2I& aStart, const VECTOR2I& aMid, const VECTOR2I& aEnd, const VECTOR2I& aCenter );

    const std::vector<VECTOR2I>& GetBezierPoints() const { return m_bezierPoints; }

    /**
     * Duplicate the list of corners in a std::vector<VECTOR2I>
     *
     * It must be used only to convert the SHAPE_POLY_SET internal corner buffer
     * to a list of VECTOR2Is, and nothing else, because it duplicates the buffer,
     * that is inefficient to know for instance the corner count
     */
    void DupPolyPointsList( std::vector<VECTOR2I>& aBuffer ) const;

    /**
     * @return the number of corners of the polygonal shape
     */
    int GetPointCount() const;

    // Accessors to the polygonal shape
    SHAPE_POLY_SET& GetPolyShape() { return m_poly; }
    const SHAPE_POLY_SET& GetPolyShape() const { return m_poly; }

    /**
     * @return true if the polygonal shape is valid (has more than 2 points)
     */
    bool IsPolyShapeValid() const;

    void SetPolyShape( const SHAPE_POLY_SET& aShape )
    {
        m_poly = aShape;

        for( int ii = 0; ii < m_poly.OutlineCount(); ++ii )
        {
            if( m_poly.HoleCount( ii ) )
            {
                m_poly.Fracture( SHAPE_POLY_SET::PM_FAST );
                break;
            }
        }
    }

    void SetPolyPoints( const std::vector<VECTOR2I>& aPoints );

    /**
     * Rebuild the m_bezierPoints vertex list that approximate the Bezier curve by a list of
     * segments.
     *
     * Has meaning only for BEZIER shape.
     *
     * @param aMinSegLen is the min length of segments approximating the bezier. The shape's last
     *                   segment can be shorter.  This parameter avoids having too many very short
     *                   segment in list. Good values are between m_width/2 and m_width.
     */
    void RebuildBezierToSegmentsPointsList( int aMinSegLen );

    /**
     * Make a set of SHAPE objects representing the EDA_SHAPE.  Caller owns the objects.
     *
     * @param aEdgeOnly indicates only edges should be generated (even if 0 width), and no fill
     *                  shapes.
     */
    virtual std::vector<SHAPE*> MakeEffectiveShapes( bool aEdgeOnly = false ) const
    {
        return makeEffectiveShapes( aEdgeOnly );
    }

    void ShapeGetMsgPanelInfo( EDA_DRAW_FRAME* aFrame, std::vector<MSG_PANEL_ITEM>& aList );

    /**
     * Return the length of the track using the hypotenuse calculation.
     *
     * @return the length of the track
     */
    double GetLength() const;

    /**
     * Convert the shape to a closed polygon.  Circles and arcs are approximated by segments.
     *
     * @param aBuffer is a buffer to store the polygon.
     * @param aClearance is the clearance around the pad.
     * @param aError is the maximum deviation from a true arc.
     * @param aErrorLoc whether any approximation error shoule be placed inside or outside
     * @param ignoreLineWidth is used for edge cut items where the line width is only for
     *                        visualization
     */
    void TransformShapeToPolygon( SHAPE_POLY_SET& aBuffer, int aClearance, int aError,
                                  ERROR_LOC aErrorLoc, bool ignoreLineWidth = false ) const;

    int Compare( const EDA_SHAPE* aOther ) const;

protected:
    void    setPosition( const VECTOR2I& aPos );
    VECTOR2I getPosition() const;

    void move( const VECTOR2I& aMoveVector );
    void rotate( const VECTOR2I& aRotCentre, const EDA_ANGLE& aAngle );
    void flip( const VECTOR2I& aCentre, bool aFlipLeftRight );
    void scale( double aScale );

    // To be implemented by concrete classes
    virtual EDA_ANGLE getParentOrientation() const = 0;
    virtual VECTOR2I getParentPosition() const = 0;

    const BOX2I getBoundingBox() const;

    void computeArcBBox( BOX2I& aBBox ) const;

    bool hitTest( const VECTOR2I& aPosition, int aAccuracy = 0 ) const;
    bool hitTest( const BOX2I& aRect, bool aContained, int aAccuracy = 0 ) const;

    const std::vector<VECTOR2I> buildBezierToSegmentsPointsList( int aMinSegLen ) const;

    void beginEdit( const VECTOR2I& aStartPoint );
    bool continueEdit( const VECTOR2I& aPosition );
    void calcEdit( const VECTOR2I& aPosition );

    /**
     * Finishes editing the shape.
     * @param aClosed Should polygon shapes be closed (yes for pcbnew/fpeditor, no for libedit)
     */
    void endEdit( bool aClosed = true );
    void setEditState( int aState ) { m_editState = aState; }

    /**
     * Make a set of SHAPE objects representing the EDA_SHAPE.  Caller owns the objects.
     *
     * @param aEdgeOnly indicates only edges should be generated (even if 0 width), and no fill
     *                  shapes.
     * @param aLineChainOnly indicates SHAPE_POLY_SET is being abused slightly to represent a
     *                       lineChain rather than a closed polygon
     */
    // fixme: move to shape_compound
    std::vector<SHAPE*> makeEffectiveShapes( bool aEdgeOnly, bool aLineChainOnly = false ) const;

protected:
    bool                  m_endsSwapped;  // true if start/end were swapped e.g. SetArcAngleAndEnd
    SHAPE_T               m_shape;        // Shape: line, Circle, Arc
    STROKE_PARAMS         m_stroke;       // Line style, width, etc.
    FILL_T                m_fill;
    COLOR4D               m_fillColor;

    VECTOR2I              m_start;             // Line start point or Circle center
    VECTOR2I              m_end;               // Line end point or Circle 3 o'clock point

    VECTOR2I              m_arcCenter;         // Used only for Arcs: arc end point
    ARC_MID               m_arcMidData;        // Used to store originating data

    VECTOR2I              m_bezierC1;          // Bezier Control Point 1
    VECTOR2I              m_bezierC2;          // Bezier Control Point 2

    std::vector<VECTOR2I> m_bezierPoints;
    SHAPE_POLY_SET        m_poly;              // Stores the S_POLYGON shape

    int                   m_editState;
    bool                  m_annotationProxy;   // A shape storing the position of an annotation
};

#endif  // EDA_SHAPE_H
