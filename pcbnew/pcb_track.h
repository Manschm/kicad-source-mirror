/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2004 Jean-Pierre Charras, jaen-pierre.charras@gipsa-lab.inpg.com
 * Copyright (C) 1992-2024 KiCad Developers, see AUTHORS.txt for contributors.
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
 * @brief A single base class (PCB_TRACK) represents both tracks and vias, with subclasses
 * for curved tracks (PCB_ARC) and vias (PCB_VIA).  All told there are three KICAD_Ts:
 * PCB_TRACK_T, PCB_ARC_T, and PCB_VIA_T.
 *
 * For vias there is a further VIATYPE which indicates THROUGH, BLIND_BURIED, or MICROVIA,
 * which are supported by the synthetic KICAD_Ts PCB_LOCATE_STDVIA_T, PCB_LOCATE_BBVIA_T, and
 * PCB_LOCATE_UVIA_T.
 */

#ifndef CLASS_TRACK_H
#define CLASS_TRACK_H


#include <mutex>
#include <array>
#include <board_connected_item.h>
#include <base_units.h>
#include <geometry/shape_segment.h>
#include <core/minoptmax.h>
#include <core/arraydim.h>
#include <padstack.h>

class PCB_TRACK;
class PCB_VIA;
class PAD;
class MSG_PANEL_ITEM;
class SHAPE_POLY_SET;
class SHAPE_ARC;


// Flag used in locate routines (from which endpoint work)
enum ENDPOINT_T : int
{
    ENDPOINT_START = 0,
    ENDPOINT_END = 1
};

// Note that this enum must be synchronized to GAL_LAYER_ID
enum class VIATYPE : int
{
    THROUGH      = 3, /* Always a through hole via */
    BLIND_BURIED = 2, /* this via can be on internal layers */
    MICROVIA     = 1, /* this via which connect from an external layer
                       * to the near neighbor internal layer */
    NOT_DEFINED  = 0  /* not yet used */
};

#define UNDEFINED_DRILL_DIAMETER  -1       //< Undefined via drill diameter.

// Used for tracks and vias for algorithmic safety, not to enforce constraints
#define GEOMETRY_MIN_SIZE (int) ( 0.001 * pcbIUScale.IU_PER_MM )


class PCB_TRACK : public BOARD_CONNECTED_ITEM
{
public:
    static inline bool ClassOf( const EDA_ITEM* aItem )
    {
        return aItem && PCB_TRACE_T == aItem->Type();
    }

    PCB_TRACK( BOARD_ITEM* aParent, KICAD_T idtype = PCB_TRACE_T );

    // Do not create a copy constructor.  The one generated by the compiler is adequate.

    void Move( const VECTOR2I& aMoveVector ) override
    {
        m_Start += aMoveVector;
        m_End   += aMoveVector;
    }

    void Rotate( const VECTOR2I& aRotCentre, const EDA_ANGLE& aAngle ) override;

    virtual void Mirror( const VECTOR2I& aCentre, bool aMirrorAroundXAxis );

    void Flip( const VECTOR2I& aCentre, bool aFlipLeftRight ) override;

    void SetPosition( const VECTOR2I& aPos ) override { m_Start = aPos; }
    VECTOR2I GetPosition() const override             { return m_Start; }
    const VECTOR2I GetFocusPosition() const override  { return ( m_Start + m_End ) / 2; }

    void SetWidth( int aWidth )             { m_Width = aWidth; }
    int GetWidth() const                    { return m_Width; }

    void SetEnd( const VECTOR2I& aEnd )     { m_End = aEnd; }
    const VECTOR2I& GetEnd() const          { return m_End; }

    void SetStart( const VECTOR2I& aStart ) { m_Start = aStart; }
    const VECTOR2I& GetStart() const        { return m_Start; }

    void SetEndX( int aX )                  { m_End.x = aX; }
    void SetEndY( int aY )                  { m_End.y = aY; }

    int GetEndX() const                     { return m_End.x; }
    int GetEndY() const                     { return m_End.y; }

    /// Return the selected endpoint (start or end)
    const VECTOR2I& GetEndPoint( ENDPOINT_T aEndPoint ) const
    {
        if( aEndPoint == ENDPOINT_START )
            return m_Start;
        else
            return m_End;
    }

    // Virtual function
    const BOX2I GetBoundingBox() const override;

    /**
     * Get the length of the track using the hypotenuse calculation.
     *
     * @return the length of the track.
     */
    virtual double GetLength() const;

    /**
     * Convert the track shape to a closed polygon.
     *
     * Circles (vias) and arcs (ends of tracks) are approximated by segments.
     *
     * @param aBuffer is a buffer to store the polygon
     * @param aClearance is the clearance around the pad
     * @param aError is the maximum deviation from true circle
     * @param ignoreLineWidth is used for edge cut items where the line width is only for
     *                        visualization
     */
    void TransformShapeToPolygon( SHAPE_POLY_SET& aBuffer, PCB_LAYER_ID aLayer, int aClearance,
                                  int aError, ERROR_LOC aErrorLoc,
                                  bool ignoreLineWidth = false ) const override;

    // @copydoc BOARD_ITEM::GetEffectiveShape
    std::shared_ptr<SHAPE> GetEffectiveShape( PCB_LAYER_ID aLayer = UNDEFINED_LAYER,
                                              FLASHING aFlash = FLASHING::DEFAULT ) const override;

    /**
     * Return STARTPOINT if point if near (dist = min_dist) start point, ENDPOINT if
     * point if near (dist = min_dist) end point,STARTPOINT|ENDPOINT if point if near
     * (dist = min_dist) both ends, or 0 if none of the above.
     *
     * If min_dist < 0: min_dist = track_width/2
     */
    EDA_ITEM_FLAGS IsPointOnEnds( const VECTOR2I& point, int min_dist = 0 ) const;

    /**
     * Return true if segment length is zero.
     */
    bool IsNull() const
    {
        return ( Type() == PCB_VIA_T ) || ( m_Start == m_End );
    }

    void GetMsgPanelInfo( EDA_DRAW_FRAME* aFrame, std::vector<MSG_PANEL_ITEM>& aList ) override;
    wxString GetFriendlyName() const override;

    INSPECT_RESULT Visit( INSPECTOR inspector, void* testData,
                          const std::vector<KICAD_T>& aScanTypes ) override;

    bool HitTest( const VECTOR2I& aPosition, int aAccuracy = 0 ) const override;
    bool HitTest( const BOX2I& aRect, bool aContained, int aAccuracy = 0 ) const override;

    bool ApproxCollinear( const PCB_TRACK& aTrack );

    wxString GetClass() const override
    {
        return wxT( "PCB_TRACK" );
    }

    virtual MINOPTMAX<int> GetWidthConstraint( wxString* aSource = nullptr ) const;

    wxString GetItemDescription( UNITS_PROVIDER* aUnitsProvider ) const override;

    BITMAPS GetMenuImage() const override;

    virtual EDA_ITEM* Clone() const override;

    virtual void ViewGetLayers( int aLayers[], int& aCount ) const override;

    double ViewGetLOD( int aLayer, KIGFX::VIEW* aView ) const override;

    const BOX2I ViewBBox() const override;

    /**
     * @return true because a track or a via is always on a copper layer.
     */
    bool IsOnCopperLayer() const override
    {
        return true;
    }

    /**
     * Get last used LOD for the track net name.
     *
     * @return LOD from ViewGetLOD()
     */
    double GetCachedLOD()
    {
        return m_CachedLOD;
    }

    /**
     * Set the cached LOD.
     *
     * @param aLOD value from ViewGetLOD() or 0.0 to always display.
     */
    void SetCachedLOD( double aLOD )
    {
        m_CachedLOD = aLOD;
    }

    /**
     * Get last used zoom scale for the track net name.
     *
     * @return scale from GetScale()
     */
    double GetCachedScale()
    {
        return m_CachedScale;
    }

    virtual double Similarity( const BOARD_ITEM& aOther ) const override;

    virtual bool operator==( const BOARD_ITEM& aOther ) const override;
    virtual bool operator==( const PCB_TRACK& aOther ) const;

    /**
     * Set the cached scale.
     *
     * @param aScale value from GetScale()
     */
    void SetCachedScale( double aScale )
    {
        m_CachedScale = aScale;
    }

    struct cmp_tracks
    {
        bool operator()( const PCB_TRACK* aFirst, const PCB_TRACK* aSecond ) const;
    };

    void Serialize( google::protobuf::Any &aContainer ) const override;
    bool Deserialize( const google::protobuf::Any &aContainer ) override;

#if defined (DEBUG)
    virtual void Show( int nestLevel, std::ostream& os ) const override { ShowDummy( os ); }
#endif

protected:
    virtual void swapData( BOARD_ITEM* aImage ) override;

    void GetMsgPanelInfoBase_Common( EDA_DRAW_FRAME* aFrame,
                                     std::vector<MSG_PANEL_ITEM>& aList ) const;

protected:
    int      m_Width;        ///< Thickness of track, or via diameter
    VECTOR2I m_Start;        ///< Line start point
    VECTOR2I m_End;          ///< Line end point

    double   m_CachedLOD;    ///< Last LOD used to draw this track's net
    double   m_CachedScale;  ///< Last zoom scale used to draw this track's net.
};


class PCB_ARC : public PCB_TRACK
{
public:
    PCB_ARC( BOARD_ITEM* aParent ) :
        PCB_TRACK( aParent, PCB_ARC_T )
    { }

    PCB_ARC( BOARD_ITEM* aParent, const SHAPE_ARC* aArc );

    static inline bool ClassOf( const EDA_ITEM *aItem )
    {
        return aItem && PCB_ARC_T == aItem->Type();
    }

    virtual void Move( const VECTOR2I& aMoveVector ) override
    {
        m_Start += aMoveVector;
        m_Mid   += aMoveVector;
        m_End   += aMoveVector;
    }

    void Rotate( const VECTOR2I& aRotCentre, const EDA_ANGLE& aAngle ) override;

    void Mirror( const VECTOR2I& aCentre, bool aMirrorAroundXAxis ) override;

    void Flip( const VECTOR2I& aCentre, bool aFlipLeftRight ) override;

    void SetMid( const VECTOR2I& aMid ) { m_Mid = aMid; }
    const VECTOR2I& GetMid() const      { return m_Mid; }

    void SetPosition( const VECTOR2I& aPos ) override { m_Start = aPos; }

    virtual VECTOR2I GetPosition() const override;
    const VECTOR2I GetFocusPosition() const override  { return m_Mid; }

    virtual VECTOR2I GetCenter() const override { return GetPosition(); }

    double GetRadius() const;
    EDA_ANGLE GetAngle() const;
    EDA_ANGLE GetArcAngleStart() const;
    EDA_ANGLE GetArcAngleEnd() const;  // Called by Python; ignore CLion's claim that it's unused
    virtual bool HitTest( const VECTOR2I& aPosition, int aAccuracy = 0 ) const override;

    virtual bool HitTest( const BOX2I& aRect, bool aContained = true,
                          int aAccuracy = 0 ) const override;

    bool IsCCW() const;

    wxString GetClass() const override
    {
        return wxT( "PCB_ARC" );
    }

    // @copydoc BOARD_ITEM::GetEffectiveShape
    std::shared_ptr<SHAPE> GetEffectiveShape( PCB_LAYER_ID aLayer = UNDEFINED_LAYER,
                                              FLASHING aFlash = FLASHING::DEFAULT ) const override;

    /**
     * Return the length of the arc track.
     *
     * @return double - the length of the track
     */
    virtual double GetLength() const override
    {
        return GetRadius() * std::abs( GetAngle().AsRadians() );
    }

    EDA_ITEM* Clone() const override;

    /**
     * @return true if the arc is too small to allow a safe calculation
     * of center position and arc angles i.e if distance between m_Mid and each arc end
     * is only a few internal units.
     * @param aThreshold is the minimal dist in internal units. Default id 5 IU
     */
    bool IsDegenerated( int aThreshold = 5 ) const;

    double Similarity( const BOARD_ITEM& aOther ) const override;

    bool operator==( const BOARD_ITEM& aOther ) const override;

    void Serialize( google::protobuf::Any &aContainer ) const override;
    bool Deserialize( const google::protobuf::Any &aContainer ) override;

protected:
    virtual void swapData( BOARD_ITEM* aImage ) override;

private:
    VECTOR2I m_Mid; ///< Arc mid point, halfway between start and end
};


class PCB_VIA : public PCB_TRACK
{
public:
    PCB_VIA( BOARD_ITEM* aParent );

    static inline bool ClassOf( const EDA_ITEM *aItem )
    {
        return aItem && PCB_VIA_T == aItem->Type();
    }

    PCB_VIA( const PCB_VIA& aOther );
    PCB_VIA& operator=( const PCB_VIA &aOther );

    bool IsType( const std::vector<KICAD_T>& aScanTypes ) const override
    {
        if( BOARD_CONNECTED_ITEM::IsType( aScanTypes ) )
            return true;

        for( KICAD_T scanType : aScanTypes )
        {
            if( scanType == PCB_LOCATE_STDVIA_T && m_viaType == VIATYPE::THROUGH )
                return true;
            else if( scanType == PCB_LOCATE_UVIA_T && m_viaType == VIATYPE::MICROVIA )
                return true;
            else if( scanType == PCB_LOCATE_BBVIA_T && m_viaType == VIATYPE::BLIND_BURIED )
                return true;
        }

        return false;
    }

    VIATYPE GetViaType() const { return m_viaType; }
    void SetViaType( VIATYPE aViaType ) { m_viaType = aViaType; }

    const PADSTACK& Padstack() const              { return m_padStack; }
    PADSTACK& Padstack()                          { return m_padStack; }
    void SetPadstack( const PADSTACK& aPadstack ) { m_padStack = aPadstack; }

    bool HasHole() const override
    {
        return true;
    }

    bool HasDrilledHole() const override
    {
        return m_viaType == VIATYPE::THROUGH || m_viaType == VIATYPE::BLIND_BURIED;
    }

    std::shared_ptr<SHAPE_SEGMENT> GetEffectiveHoleShape() const override;

    MINOPTMAX<int> GetWidthConstraint( wxString* aSource = nullptr ) const override;
    MINOPTMAX<int> GetDrillConstraint( wxString* aSource = nullptr ) const;

    bool IsTented() const override;
    int GetSolderMaskExpansion() const;

    PCB_LAYER_ID GetLayer() const override;
    void SetLayer( PCB_LAYER_ID aLayer ) override;

    bool IsOnLayer( PCB_LAYER_ID aLayer ) const override;

    virtual LSET GetLayerSet() const override;

    /**
     * Note SetLayerSet() initialize the first and last copper layers connected by the via.
     * So currently SetLayerSet ignore non copper layers
     */
    virtual void SetLayerSet( LSET aLayers ) override;

    /**
     * For a via m_layer contains the top layer, the other layer is in m_bottomLayer/
     *
     * @param aTopLayer is the first layer connected by the via.
     * @param aBottomLayer is last layer connected by the via.
     */
    void SetLayerPair( PCB_LAYER_ID aTopLayer, PCB_LAYER_ID aBottomLayer );

    void SetBottomLayer( PCB_LAYER_ID aLayer );
    void SetTopLayer( PCB_LAYER_ID aLayer );

    /**
     * Return the 2 layers used by the via (the via actually uses all layers between these
     * 2 layers)
     *
     *  @param[out] top_layer is storage for the first layer of the via (can be null).
     *  @param[out] bottom_layer is storage for the last layer of the via(can be null).
     */
    void LayerPair( PCB_LAYER_ID* top_layer, PCB_LAYER_ID* bottom_layer ) const;

    PCB_LAYER_ID TopLayer() const;
    PCB_LAYER_ID BottomLayer() const;

    /**
     * Check so that the layers are correct depending on the type of via, and
     * so that the top actually is on top.
     */
    void SanitizeLayers();

    VECTOR2I GetPosition() const override               { return m_Start; }
    void SetPosition( const VECTOR2I& aPoint ) override { m_Start = aPoint;  m_End = aPoint; }

    void GetMsgPanelInfo( EDA_DRAW_FRAME* aFrame, std::vector<MSG_PANEL_ITEM>& aList ) override;

    bool HitTest( const VECTOR2I& aPosition, int aAccuracy = 0 ) const override;
    bool HitTest( const BOX2I& aRect, bool aContained, int aAccuracy = 0 ) const override;

    wxString GetClass() const override
    {
        return wxT( "PCB_VIA" );
    }

    wxString GetItemDescription( UNITS_PROVIDER* aUnitsProvider ) const override;

    BITMAPS GetMenuImage() const override;

    EDA_ITEM* Clone() const override;

    void ViewGetLayers( int aLayers[], int& aCount ) const override;

    double ViewGetLOD( int aLayer, KIGFX::VIEW* aView ) const override;

    void Flip( const VECTOR2I& aCentre, bool aFlipLeftRight ) override;

#if defined (DEBUG)
    void Show( int nestLevel, std::ostream& os ) const override { ShowDummy( os ); }
#endif

    int GetMinAnnulus( PCB_LAYER_ID aLayer, wxString* aSource ) const;

    /**
     * @deprecated - use Padstack().SetUnconnectedLayerMode()
     * Sets the unconnected removal property.  If true, the copper is removed on zone fill
     * or when specifically requested when the via is not connected on a layer.
     */
    void SetRemoveUnconnected( bool aSet )
    {
        m_padStack.SetUnconnectedLayerMode( aSet
                ? PADSTACK::UNCONNECTED_LAYER_MODE::REMOVE_ALL
                : PADSTACK::UNCONNECTED_LAYER_MODE::KEEP_ALL );
    }

    bool GetRemoveUnconnected() const
    {
        return m_padStack.UnconnectedLayerMode() != PADSTACK::UNCONNECTED_LAYER_MODE::KEEP_ALL;
    }

    /**
     * @deprecated - use Padstack().SetUnconnectedLayerMode()
     * Sets whether we keep the start and end annular rings even if they are not connected
     */
    void SetKeepStartEnd( bool aSet )
    {
        m_padStack.SetUnconnectedLayerMode( aSet
                ? PADSTACK::UNCONNECTED_LAYER_MODE::REMOVE_EXCEPT_START_AND_END
                : PADSTACK::UNCONNECTED_LAYER_MODE::REMOVE_ALL );
    }

    bool GetKeepStartEnd() const
    {
        return m_padStack.UnconnectedLayerMode()
               == PADSTACK::UNCONNECTED_LAYER_MODE::REMOVE_EXCEPT_START_AND_END;
    }

    bool ConditionallyFlashed( PCB_LAYER_ID aLayer ) const
    {
        switch( m_padStack.UnconnectedLayerMode() )
        {
        case PADSTACK::UNCONNECTED_LAYER_MODE::KEEP_ALL:
            return false;

        case PADSTACK::UNCONNECTED_LAYER_MODE::REMOVE_ALL:
            return true;

        case PADSTACK::UNCONNECTED_LAYER_MODE::REMOVE_EXCEPT_START_AND_END:
        {
            if( aLayer == m_padStack.Drill().start || aLayer == m_padStack.Drill().end )
                return false;
        }
        }

        return true;
    }

    /**
     * Check to see whether the via should have a pad on the specific layer.
     *
     * @param aLayer Layer to check for connectivity
     * @return true if connected by pad or track (or optionally zone)
     */
    bool FlashLayer( int aLayer ) const;

    /**
     * Check to see if the via is present on any of the layers in the set.
     *
     * @param aLayers is the set of layers to check the via against.
     * @return true if connected by pad or track (or optionally zone) on any of the associated
     *         layers.
     */
    bool FlashLayer( LSET aLayers ) const;

    /**
     * Return the top-most and bottom-most connected layers.
     * @param aTopmost
     * @param aBottommost
     */
    void GetOutermostConnectedLayers( PCB_LAYER_ID* aTopmost,
                             PCB_LAYER_ID* aBottommost ) const;

    /**
     * Set the drill value for vias.
     *
     * @param aDrill is the new drill diameter
     */
    void SetDrill( int aDrill )
    {
        m_padStack.Drill().size = { aDrill, aDrill };
    }

    /**
     * Return the local drill setting for this PCB_VIA.
     *
     * @note Use GetDrillValue() to get the calculated value.
     */
    int GetDrill() const                    { return m_padStack.Drill().size.x; }

    /**
     * Calculate the drill value for vias (m_drill if > 0, or default drill value for the board).
     *
     * @return the calculated drill value.
     */
    int GetDrillValue() const;

    /**
     * Set the drill value for vias to the default value #UNDEFINED_DRILL_DIAMETER.
     */
    void SetDrillDefault()
    {
        m_padStack.Drill().size = { UNDEFINED_DRILL_DIAMETER, UNDEFINED_DRILL_DIAMETER };
    }

    /**
     * Check if the via is a free via (as opposed to one created on a track by the router).
     *
     * Free vias don't have their nets automatically updated by the connectivity algorithm.
     *
     * @return true if the via is a free via
     */
    bool GetIsFree() const              { return m_isFree; }
    void SetIsFree( bool aFree = true ) { m_isFree = aFree; }

    // @copydoc BOARD_ITEM::GetEffectiveShape
    std::shared_ptr<SHAPE> GetEffectiveShape( PCB_LAYER_ID aLayer = UNDEFINED_LAYER,
                                              FLASHING aFlash = FLASHING::DEFAULT ) const override;

    void ClearZoneLayerOverrides()
    {
        m_zoneLayerOverrides.fill( ZLO_NONE );
    }

    const ZONE_LAYER_OVERRIDE& GetZoneLayerOverride( PCB_LAYER_ID aLayer ) const
    {
        return m_zoneLayerOverrides.at( aLayer );
    }

    void SetZoneLayerOverride( PCB_LAYER_ID aLayer, ZONE_LAYER_OVERRIDE aOverride )
    {
        std::unique_lock<std::mutex> cacheLock( m_zoneLayerOverridesMutex );
        m_zoneLayerOverrides.at( aLayer ) = aOverride;
    }

    double Similarity( const BOARD_ITEM& aOther ) const override;

    bool operator==( const PCB_VIA& aOther ) const;
    bool operator==( const BOARD_ITEM& aOther ) const override;

    void Serialize( google::protobuf::Any &aContainer ) const override;
    bool Deserialize( const google::protobuf::Any &aContainer ) override;

protected:
    void swapData( BOARD_ITEM* aImage ) override;

    wxString layerMaskDescribe() const override;

private:
    VIATYPE      m_viaType;                  ///< through, blind/buried or micro

    PADSTACK     m_padStack;

    bool         m_isFree;                   ///< "Free" vias don't get their nets auto-updated

    std::mutex                                     m_zoneLayerOverridesMutex;
    std::array<ZONE_LAYER_OVERRIDE, MAX_CU_LAYERS> m_zoneLayerOverrides;
};


#endif // CLASS_TRACK_H
