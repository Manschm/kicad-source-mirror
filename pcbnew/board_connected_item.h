/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2012 Jean-Pierre Charras, jean-pierre.charras@ujf-grenoble.fr
 * Copyright (C) 2012 SoftPLC Corporation, Dick Hollenbeck <dick@softplc.com>
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

#ifndef BOARD_CONNECTED_ITEM_H
#define BOARD_CONNECTED_ITEM_H

#include <board_item.h>
#include <teardrop/teardrop_parameters.h>

class NETCLASS;
class NETINFO_ITEM;
class PAD;

/**
 * A base class derived from #BOARD_ITEM for items that can be connected and have a net, a
 * netname, a clearance ...
 */
class BOARD_CONNECTED_ITEM : public BOARD_ITEM
{
public:
    BOARD_CONNECTED_ITEM( BOARD_ITEM* aParent, KICAD_T idtype );

    // Do not create a copy constructor & operator=.
    // The ones generated by the compiler are adequate.

    static inline bool ClassOf( const EDA_ITEM* aItem )
    {
        if( aItem == nullptr )
            return false;

        switch( aItem->Type() )
        {
        case PCB_PAD_T:
        case PCB_TRACE_T:
        case PCB_ARC_T:
        case PCB_VIA_T:
        case PCB_ZONE_T:
        case PCB_SHAPE_T:
            return true;

        default:
            return false;
        }
    }

    ///< @copydoc BOARD_ITEM::IsConnected()
    bool IsConnected() const override
    {
        return true;
    }

    /**
     * Return #NET_INFO object for a given item.
     */
    NETINFO_ITEM* GetNet() const
    {
        return m_netinfo;
    }

    /**
     * Set a NET_INFO object for the item.
     */
    void SetNet( NETINFO_ITEM* aNetInfo )
    {
        m_netinfo = aNetInfo;
    }

    /**
     * @return the net code.
     */
    int GetNetCode() const;

    /**
     * Set net using a net code.
     *
     * @note Pads not on copper layers will have their net code always set to 0 (not connected).
     *
     * @param aNetCode is a net code for the new net. It has to exist in #NETINFO_LIST held
     *                 by #BOARD.
     * @param aNoAssert if true, do not assert that the net exists.  Otherwise, item is assigned
     *                  to the unconnected net.
     * @return true on success, false if the net did not exist
     */
    bool SetNetCode( int aNetCode, bool aNoAssert );

    void SetNetCode( int aNetCode )
    {
        SetNetCode( aNetCode, false );
    }

    /**
     * @return the full netname.
     */
    wxString GetNetname() const;

    /**
     * @return the full netname or "<no net>" in square braces, followed by "(Not Found)" if the
     *         netcode is undefined.
     */
    wxString GetNetnameMsg() const;

    /**
     * @return the short netname.
     */
    const wxString& GetShortNetname() const;

    /**
     * @return the unescaped short netname.
     */
    const wxString& GetDisplayNetname() const;

    /**
     * Return an item's "own" clearance in internal units.
     *
     * @param aLayer the layer in question.
     * @param aSource [out] optionally reports the source as a user-readable string.
     * @return the clearance in internal units.
     */
    virtual int GetOwnClearance( PCB_LAYER_ID aLayer, wxString* aSource = nullptr ) const;

    /**
     * Return any clearance overrides set in the "classic" (ie: pre-rule) system.
     *
     * @param aSource [out] optionally reports the source as a user-readable string.
     * @return the clearance in internal units.
     */
    virtual std::optional<int> GetClearanceOverrides( wxString* aSource ) const
    {
        return std::optional<int>();
    }

    /**
     * Return any local clearances set in the "classic" (ie: pre-rule) system.
     *
     * @return the clearance (if any is specified) in internal units.
     */
    virtual std::optional<int> GetLocalClearance() const
    {
        return std::optional<int>();
    }

    /**
     * Return any local clearances set in the "classic" (ie: pre-rule) system.
     *
     * @param aSource [out] optionally reports the source as a user-readable string.
     * @return the clearance (if any is specified) in internal units.
     */
    virtual std::optional<int> GetLocalClearance( wxString* aSource ) const
    {
        return std::optional<int>();
    }

    /**
     * Return the #NETCLASS for this item.
     *
     * @note Do **not** return a std::shared_ptr from this.  It is used heavily in DRC, and the
     *       std::shared_ptr stuff shows up large in performance profiling.
     */
    virtual NETCLASS* GetEffectiveNetClass() const;

    /**
     * Returns the name of the effective netclass.  Primarily for the use of the property system.
     */
    wxString GetNetClassName() const;

    void SetLocalRatsnestVisible( bool aVisible ) { m_localRatsnestVisible = aVisible; }
    bool GetLocalRatsnestVisible() const { return m_localRatsnestVisible; }

    TEARDROP_PARAMETERS& GetTeardropParams() { return m_teardropParams; }
    const TEARDROP_PARAMETERS& GetTeardropParams() const { return m_teardropParams; }

    void SetTeardropsEnabled( bool aEnable ) { m_teardropParams.m_Enabled = aEnable; }
    bool GetTeardropsEnabled() const { return m_teardropParams.m_Enabled; }

    void SetTeardropBestLengthRatio( double aRatio ) { m_teardropParams.m_BestLengthRatio = aRatio; }
    double GetTeardropBestLengthRatio() const { return m_teardropParams.m_BestLengthRatio; }

    void SetTeardropMaxLength( int aMaxLength ) { m_teardropParams.m_TdMaxLen = aMaxLength; }
    int GetTeardropMaxLength() const { return m_teardropParams.m_TdMaxLen; }

    void SetTeardropBestWidthRatio( double aRatio ) { m_teardropParams.m_BestWidthRatio = aRatio; }
    double GetTeardropBestWidthRatio() const { return m_teardropParams.m_BestWidthRatio; }

    void SetTeardropMaxWidth( int aMaxWidth ) { m_teardropParams.m_TdMaxWidth = aMaxWidth; }
    int GetTeardropMaxWidth() const { return m_teardropParams.m_TdMaxWidth; }

    void SetTeardropCurved( bool aCurve ) { m_teardropParams.m_CurveSegCount = aCurve ? 1 : 0; }
    bool GetTeardropCurved() const { return m_teardropParams.m_CurveSegCount > 0; }

    void SetTeardropPreferZoneConnections( bool aPrefer ) { m_teardropParams.m_TdOnPadsInZones = !aPrefer; }
    bool GetTeardropPreferZoneConnections() const { return !m_teardropParams.m_TdOnPadsInZones; }

    void SetTeardropAllowSpanTwoTracks( bool aAllow ) { m_teardropParams.m_AllowUseTwoTracks = aAllow; }
    bool GetTeardropAllowSpanTwoTracks() const { return m_teardropParams.m_AllowUseTwoTracks; }

    void SetTeardropMaxTrackWidth( double aRatio ) { m_teardropParams.m_WidthtoSizeFilterRatio = aRatio; }
    double GetTeardropMaxTrackWidth() const { return m_teardropParams.m_WidthtoSizeFilterRatio; }

protected:
    /// Store all information about the net that item belongs to.
    NETINFO_ITEM* m_netinfo;

    /// Not all BOARD_CONNECTED_ITEMs support teardrops, but we want those that do to share a
    /// single section in the property inspector.
    TEARDROP_PARAMETERS m_teardropParams;

private:
    bool m_localRatsnestVisible;

};

#endif  // BOARD_CONNECTED_ITEM_H
