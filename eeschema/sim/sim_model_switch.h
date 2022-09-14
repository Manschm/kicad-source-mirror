/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2022 Mikolaj Wielgus
 * Copyright (C) 2022 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * https://www.gnu.org/licenses/gpl-3.0.html
 * or you may search the http://www.gnu.org website for the version 3 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#ifndef SIM_MODEL_SWITCH_H
#define SIM_MODEL_SWITCH_H

#include <sim/sim_model.h>


class SIM_MODEL_SWITCH : public SIM_MODEL
{
public:
    class SPICE_GENERATOR : public SIM_MODEL::SPICE_GENERATOR
    {
    public:
        using SIM_MODEL::SPICE_GENERATOR::SPICE_GENERATOR;

        wxString ItemLine( const wxString& aRefName, const wxString& aModelName,
                           const std::vector<wxString>& aSymbolPinNumbers,
                           const std::vector<wxString>& aPinNetNames ) const override;
        wxString ItemParams() const override;
        std::vector<std::reference_wrapper<const PIN>> GetPins() const override;
    };


    SIM_MODEL_SWITCH( TYPE aType );

protected:
    wxString GenerateParamValuePair( const PARAM& aParam, bool& aIsFirst ) const override;

private:
    std::vector<wxString> getPinNames() const override
    {
        return { "ctrl+", "ctrl-", "no+", "no-" };
    }

    bool requiresSpiceModelLine() const override { return true; }

    static const std::vector<PARAM::INFO> makeSwVParamInfos();
    static const std::vector<PARAM::INFO> makeSwIParamInfos();
};

#endif // SIM_MODEL_SWITCH_H
