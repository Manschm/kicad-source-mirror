
/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2022 KiCad Developers, see AUTHORS.txt for contributors.
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

#ifndef KICAD_BITMAP_INFO_H
#define KICAD_BITMAP_INFO_H

#include <vector>
#include <bitmaps/bitmaps_list.h>

#include <wx/string.h> // wxT

struct BITMAP_INFO
{
    BITMAPS  id;
    wxString filename;
    int      height;
    wxString theme;
};


extern const std::vector<BITMAP_INFO> g_BitmapInfo;

#endif // KICAD_BITMAP_INFO_H
