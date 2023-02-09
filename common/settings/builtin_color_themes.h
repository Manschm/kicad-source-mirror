/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2020 Jon Evans <jon@craftyjon.com>
 * Copyright (C) 2020-2023 KiCad Developers, see AUTHORS.txt for contributors.
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

// NOTE: This file should only be included in color_settings.cpp

#ifndef _BUILTIN_COLOR_THEMES_H
#define _BUILTIN_COLOR_THEMES_H

#define CSS_COLOR( r, g, b, a ) COLOR4D().FromCSSRGBA( r, g, b, a )

static const std::map<int, COLOR4D> s_defaultTheme =
        {
            { LAYER_SCHEMATIC_ANCHOR,     CSS_COLOR( 0,   0,   255, 1 ) },
            { LAYER_SCHEMATIC_AUX_ITEMS,  CSS_COLOR( 0,   0,   0,   1 ) },
            { LAYER_SCHEMATIC_BACKGROUND, CSS_COLOR( 245, 244, 239, 1 ) },
            { LAYER_HOVERED,              CSS_COLOR( 0,   0,   255, 1 ) },
            { LAYER_BRIGHTENED,           CSS_COLOR( 255, 0,   255, 1 ) },
            { LAYER_BUS,                  CSS_COLOR( 0,   0,   132, 1 ) },
            { LAYER_BUS_JUNCTION,         CSS_COLOR( 0,   0,   132, 1 ) },
            { LAYER_DEVICE_BACKGROUND,    CSS_COLOR( 255, 255, 194, 1 ) },
            { LAYER_DEVICE,               CSS_COLOR( 132, 0,   0,   1 ) },
            { LAYER_SCHEMATIC_CURSOR,     CSS_COLOR( 15,  15,  15,  1 ) },
            { LAYER_ERC_ERR,              CSS_COLOR( 230, 9,   13,  0.8 ) },
            { LAYER_ERC_WARN,             CSS_COLOR( 209, 146, 0,   0.8 ) },
            { LAYER_ERC_EXCLUSION,        CSS_COLOR( 94,  194, 194, 0.8 ) },
            { LAYER_FIELDS,               CSS_COLOR( 132, 0,   132, 1 ) },
            { LAYER_SCHEMATIC_GRID,       CSS_COLOR( 181, 181, 181, 1 ) },
            { LAYER_SCHEMATIC_GRID_AXES,  CSS_COLOR( 0,   0,   132, 1 ) },
            { LAYER_HIDDEN,               CSS_COLOR( 94,  194, 194, 1 ) },
            { LAYER_JUNCTION,             CSS_COLOR( 0,   150, 0,   1 ) },
            { LAYER_GLOBLABEL,            CSS_COLOR( 132, 0,   0,   1 ) },
            { LAYER_HIERLABEL,            CSS_COLOR( 114, 86,  0,   1 ) },
            { LAYER_LOCLABEL,             CSS_COLOR( 15,  15,  15,  1 ) },
            { LAYER_NETCLASS_REFS,        CSS_COLOR( 72,  72,  72,  1 ) },
            { LAYER_NOCONNECT,            CSS_COLOR( 0,   0,   132, 1 ) },
            { LAYER_NOTES,                CSS_COLOR( 0,   0,   194, 1 ) },
            { LAYER_PRIVATE_NOTES,        CSS_COLOR( 72,  72,  255, 1 ) },
            { LAYER_NOTES_BACKGROUND,     CSS_COLOR( 0,   0,   0,   0 ) },
            { LAYER_PIN,                  CSS_COLOR( 132, 0,   0,   1 ) },
            { LAYER_PINNAM,               CSS_COLOR( 0,   100, 100, 1 ) },
            { LAYER_PINNUM,               CSS_COLOR( 169, 0,   0,   1 ) },
            { LAYER_REFERENCEPART,        CSS_COLOR( 0,   100, 100, 1 ) },
#ifdef __WXMAC__
            // Macs look better with a lighter shadow
            { LAYER_SELECTION_SHADOWS,      COLOR4D( .78, .92, 1.0, 0.8 ) },
#else
            { LAYER_SELECTION_SHADOWS,      COLOR4D( .4, .7, 1.0, 0.8 ) },
#endif
            { LAYER_SHEET,                  CSS_COLOR( 132, 0,   0,   1 ) },
            { LAYER_SHEET_BACKGROUND,       CSS_COLOR( 255, 255, 255, 0 ) },
            { LAYER_SHEETFILENAME,          CSS_COLOR( 114, 86,  0,   1 ) },
            { LAYER_SHEETFIELDS,            CSS_COLOR( 132, 0,   132, 1 ) },
            { LAYER_SHEETLABEL,             CSS_COLOR( 0,   100, 100, 1 ) },
            { LAYER_SHEETNAME,              CSS_COLOR( 0,   100, 100, 1 ) },
            { LAYER_VALUEPART,              CSS_COLOR( 0,   100, 100, 1 ) },
            { LAYER_WIRE,                   CSS_COLOR( 0,   150, 0,   1 ) },
            { LAYER_SCHEMATIC_DRAWINGSHEET, CSS_COLOR( 132, 0,   0,   1 ) },
            { LAYER_SCHEMATIC_PAGE_LIMITS,  CSS_COLOR( 181, 181, 181, 1 ) },
            { LAYER_OP_VOLTAGES,            CSS_COLOR( 132, 0,   50,  1 ) },
            { LAYER_OP_CURRENTS,            CSS_COLOR( 224, 0,   12,  1 ) },

            { LAYER_GERBVIEW_AXES,          CSS_COLOR( 0,   0,   132, 1 ) },
            { LAYER_GERBVIEW_BACKGROUND,    CSS_COLOR( 0,   0,   0,   1 ) },
            { LAYER_DCODES,                 CSS_COLOR( 255, 255, 255, 1 ) },
            { LAYER_GERBVIEW_GRID,          CSS_COLOR( 132, 132, 132, 1 ) },
            { LAYER_NEGATIVE_OBJECTS,       CSS_COLOR( 132, 132, 132, 1 ) },
            { LAYER_GERBVIEW_DRAWINGSHEET,  CSS_COLOR(   0,   0, 132, 1 ) },
            { LAYER_GERBVIEW_PAGE_LIMITS,   CSS_COLOR( 132, 132, 132, 1 ) },

            { GERBVIEW_LAYER_ID_START,      CSS_COLOR( 200, 52,  52,  1 ) },
            { GERBVIEW_LAYER_ID_START + 1,  CSS_COLOR( 127, 200, 127, 1 ) },
            { GERBVIEW_LAYER_ID_START + 2,  CSS_COLOR( 206, 125, 44,  1 ) },
            { GERBVIEW_LAYER_ID_START + 3,  CSS_COLOR( 79,  203, 203, 1 ) },
            { GERBVIEW_LAYER_ID_START + 4,  CSS_COLOR( 219, 98, 139,  1 ) },
            { GERBVIEW_LAYER_ID_START + 5,  CSS_COLOR( 167, 165, 198, 1 ) },
            { GERBVIEW_LAYER_ID_START + 6,  CSS_COLOR( 40,  204, 217, 1 ) },
            { GERBVIEW_LAYER_ID_START + 7,  CSS_COLOR( 232, 178, 167, 1 ) },
            { GERBVIEW_LAYER_ID_START + 8,  CSS_COLOR( 242, 237, 161, 1 ) },
            { GERBVIEW_LAYER_ID_START + 9,  CSS_COLOR( 141, 203, 129, 1 ) },
            { GERBVIEW_LAYER_ID_START + 10, CSS_COLOR( 237, 124, 51,  1 ) },
            { GERBVIEW_LAYER_ID_START + 11, CSS_COLOR( 91,  195, 235, 1 ) },
            { GERBVIEW_LAYER_ID_START + 12, CSS_COLOR( 247, 111, 142, 1 ) },
            { GERBVIEW_LAYER_ID_START + 13, CSS_COLOR( 77,  127, 196, 1 ) },
            { GERBVIEW_LAYER_ID_START + 14, CSS_COLOR( 200, 52,  52,  1 ) },
            { GERBVIEW_LAYER_ID_START + 15, CSS_COLOR( 127, 200, 127, 1 ) },
            { GERBVIEW_LAYER_ID_START + 16, CSS_COLOR( 206, 125, 44,  1 ) },
            { GERBVIEW_LAYER_ID_START + 17, CSS_COLOR( 79,  203, 203, 1 ) },
            { GERBVIEW_LAYER_ID_START + 18, CSS_COLOR( 219, 98, 139,  1 ) },
            { GERBVIEW_LAYER_ID_START + 19, CSS_COLOR( 167, 165, 198, 1 ) },
            { GERBVIEW_LAYER_ID_START + 20, CSS_COLOR( 40,  204, 217, 1 ) },
            { GERBVIEW_LAYER_ID_START + 21, CSS_COLOR( 232, 178, 167, 1 ) },
            { GERBVIEW_LAYER_ID_START + 22, CSS_COLOR( 242, 237, 161, 1 ) },
            { GERBVIEW_LAYER_ID_START + 23, CSS_COLOR( 141, 203, 129, 1 ) },
            { GERBVIEW_LAYER_ID_START + 24, CSS_COLOR( 237, 124, 51,  1 ) },
            { GERBVIEW_LAYER_ID_START + 25, CSS_COLOR( 91,  195, 235, 1 ) },
            { GERBVIEW_LAYER_ID_START + 26, CSS_COLOR( 247, 111, 142, 1 ) },
            { GERBVIEW_LAYER_ID_START + 27, CSS_COLOR( 77,  127, 196, 1 ) },
            { GERBVIEW_LAYER_ID_START + 28, CSS_COLOR( 200, 52,  52,  1 ) },
            { GERBVIEW_LAYER_ID_START + 29, CSS_COLOR( 127, 200, 127, 1 ) },
            { GERBVIEW_LAYER_ID_START + 30, CSS_COLOR( 206, 125, 44,  1 ) },
            { GERBVIEW_LAYER_ID_START + 31, CSS_COLOR( 79,  203, 203, 1 ) },
            { GERBVIEW_LAYER_ID_START + 32, CSS_COLOR( 219, 98, 139,  1 ) },
            { GERBVIEW_LAYER_ID_START + 33, CSS_COLOR( 167, 165, 198, 1 ) },
            { GERBVIEW_LAYER_ID_START + 34, CSS_COLOR( 40,  204, 217, 1 ) },
            { GERBVIEW_LAYER_ID_START + 35, CSS_COLOR( 232, 178, 167, 1 ) },
            { GERBVIEW_LAYER_ID_START + 36, CSS_COLOR( 242, 237, 161, 1 ) },
            { GERBVIEW_LAYER_ID_START + 37, CSS_COLOR( 141, 203, 129, 1 ) },
            { GERBVIEW_LAYER_ID_START + 38, CSS_COLOR( 237, 124, 51,  1 ) },
            { GERBVIEW_LAYER_ID_START + 39, CSS_COLOR( 91,  195, 235, 1 ) },
            { GERBVIEW_LAYER_ID_START + 40, CSS_COLOR( 247, 111, 142, 1 ) },
            { GERBVIEW_LAYER_ID_START + 41, CSS_COLOR( 77,  127, 196, 1 ) },
            { GERBVIEW_LAYER_ID_START + 42, CSS_COLOR( 200, 52,  52,  1 ) },
            { GERBVIEW_LAYER_ID_START + 43, CSS_COLOR( 127, 200, 127, 1 ) },
            { GERBVIEW_LAYER_ID_START + 44, CSS_COLOR( 206, 125, 44,  1 ) },
            { GERBVIEW_LAYER_ID_START + 45, CSS_COLOR( 79,  203, 203, 1 ) },
            { GERBVIEW_LAYER_ID_START + 46, CSS_COLOR( 219, 98, 139,  1 ) },
            { GERBVIEW_LAYER_ID_START + 47, CSS_COLOR( 167, 165, 198, 1 ) },
            { GERBVIEW_LAYER_ID_START + 48, CSS_COLOR( 40,  204, 217, 1 ) },
            { GERBVIEW_LAYER_ID_START + 49, CSS_COLOR( 232, 178, 167, 1 ) },
            { GERBVIEW_LAYER_ID_START + 50, CSS_COLOR( 242, 237, 161, 1 ) },
            { GERBVIEW_LAYER_ID_START + 51, CSS_COLOR( 141, 203, 129, 1 ) },
            { GERBVIEW_LAYER_ID_START + 52, CSS_COLOR( 237, 124, 51,  1 ) },
            { GERBVIEW_LAYER_ID_START + 53, CSS_COLOR( 91,  195, 235, 1 ) },
            { GERBVIEW_LAYER_ID_START + 54, CSS_COLOR( 247, 111, 142, 1 ) },
            { GERBVIEW_LAYER_ID_START + 55, CSS_COLOR( 77,  127, 196, 1 ) },
            { GERBVIEW_LAYER_ID_START + 56, CSS_COLOR( 200, 52,  52,  1 ) },
            { GERBVIEW_LAYER_ID_START + 57, CSS_COLOR( 127, 200, 127, 1 ) },
            { GERBVIEW_LAYER_ID_START + 58, CSS_COLOR( 206, 125, 44,  1 ) },
            { GERBVIEW_LAYER_ID_START + 59, CSS_COLOR( 79,  203, 203, 1 ) },

            { LAYER_ANCHOR,                 CSS_COLOR( 255, 38,  226, 1 ) },
            { LAYER_LOCKED_ITEM_SHADOW,     CSS_COLOR( 255, 38,  226, 0.5 ) },
            { LAYER_CONFLICTS_SHADOW,       CSS_COLOR( 255,  0,   05, 0.5 ) },
            { LAYER_AUX_ITEMS,              CSS_COLOR( 255, 255, 255, 1 ) },
            { LAYER_PCB_BACKGROUND,         CSS_COLOR( 0,   16,  35,  1 ) },
            { LAYER_CURSOR,                 CSS_COLOR( 255, 255, 255, 1 ) },
            { LAYER_DRC_ERROR,              CSS_COLOR( 215, 91,  107, 0.8 ) },
            { LAYER_DRC_WARNING,            CSS_COLOR( 255, 208, 66,  0.8 ) },
            { LAYER_DRC_EXCLUSION,          CSS_COLOR( 255, 255, 255, 0.8 ) },
            { LAYER_MOD_TEXT_INVISIBLE,     CSS_COLOR( 132, 132, 132, 1 ) },
            { LAYER_GRID,                   CSS_COLOR( 132, 132, 132, 1 ) },
            { LAYER_GRID_AXES,              CSS_COLOR( 194, 194, 194, 1 ) },
            { LAYER_PAD_PLATEDHOLES,        CSS_COLOR( 194, 194, 0, 1 ) },
            { LAYER_PADS_TH,                CSS_COLOR( 227, 183, 46,  1 ) },
            { LAYER_NON_PLATEDHOLES,        CSS_COLOR( 26,  196, 210, 1 ) },
            { LAYER_RATSNEST,               CSS_COLOR( 0,   248, 255, 0.35 ) },
            { LAYER_SELECT_OVERLAY,         CSS_COLOR( 4,   255, 67,  1 ) },
            { LAYER_VIA_THROUGH,            CSS_COLOR( 236, 236, 236, 1 ) },
            { LAYER_VIA_BBLIND,             CSS_COLOR( 187, 151, 38,  1 ) },
            { LAYER_VIA_HOLES,              CSS_COLOR( 227, 183, 46, 1 ) },
            { LAYER_VIA_MICROVIA,           CSS_COLOR( 0,   132, 132, 1 ) },
            { LAYER_DRAWINGSHEET,           CSS_COLOR( 200, 114, 171, 1 ) },
            { LAYER_PAGE_LIMITS,            CSS_COLOR( 132, 132, 132, 1 ) },

            { F_Cu,                         CSS_COLOR( 200, 52,  52,  1 ) },
            { In1_Cu,                       CSS_COLOR( 127, 200, 127, 1 ) },
            { In2_Cu,                       CSS_COLOR( 206, 125, 44,  1 ) },
            { In3_Cu,                       CSS_COLOR( 79,  203, 203, 1 ) },
            { In4_Cu,                       CSS_COLOR( 219, 98, 139,  1 ) },
            { In5_Cu,                       CSS_COLOR( 167, 165, 198, 1 ) },
            { In6_Cu,                       CSS_COLOR( 40,  204, 217, 1 ) },
            { In7_Cu,                       CSS_COLOR( 232, 178, 167, 1 ) },
            { In8_Cu,                       CSS_COLOR( 242, 237, 161, 1 ) },
            { In9_Cu,                       CSS_COLOR( 141, 203, 129, 1 ) },
            { In10_Cu,                      CSS_COLOR( 237, 124, 51,  1 ) },
            { In11_Cu,                      CSS_COLOR( 91,  195, 235, 1 ) },
            { In12_Cu,                      CSS_COLOR( 247, 111, 142, 1 ) },
            { In13_Cu,                      CSS_COLOR( 167, 165, 198, 1 ) },
            { In14_Cu,                      CSS_COLOR( 40,  204, 217, 1 ) },
            { In15_Cu,                      CSS_COLOR( 232, 178, 167, 1 ) },
            { In16_Cu,                      CSS_COLOR( 242, 237, 161, 1 ) },
            { In17_Cu,                      CSS_COLOR( 237, 124, 51,  1 ) },
            { In18_Cu,                      CSS_COLOR( 91,  195, 235, 1 ) },
            { In19_Cu,                      CSS_COLOR( 247, 111, 142, 1 ) },
            { In20_Cu,                      CSS_COLOR( 167, 165, 198, 1 ) },
            { In21_Cu,                      CSS_COLOR( 40,  204, 217, 1 ) },
            { In22_Cu,                      CSS_COLOR( 232, 178, 167, 1 ) },
            { In23_Cu,                      CSS_COLOR( 242, 237, 161, 1 ) },
            { In24_Cu,                      CSS_COLOR( 237, 124, 51,  1 ) },
            { In25_Cu,                      CSS_COLOR( 91,  195, 235, 1 ) },
            { In26_Cu,                      CSS_COLOR( 247, 111, 142, 1 ) },
            { In27_Cu,                      CSS_COLOR( 167, 165, 198, 1 ) },
            { In28_Cu,                      CSS_COLOR( 40,  204, 217, 1 ) },
            { In29_Cu,                      CSS_COLOR( 232, 178, 167, 1 ) },
            { In30_Cu,                      CSS_COLOR( 242, 237, 161, 1 ) },
            { B_Cu,                         CSS_COLOR( 77,  127, 196, 1 ) },

            { B_Adhes,                      CSS_COLOR( 0,   0, 132,   1 ) },
            { F_Adhes,                      CSS_COLOR( 132, 0, 132,   1 ) },
            { B_Paste,                      CSS_COLOR( 0,   194, 194, 0.9 ) },
            { F_Paste,                      CSS_COLOR( 180, 160, 154, 0.9 ) },
            { B_SilkS,                      CSS_COLOR( 232, 178, 167, 1 ) },
            { F_SilkS,                      CSS_COLOR( 242, 237, 161, 1 ) },
            { B_Mask,                       CSS_COLOR( 2,   255, 238, 0.4 ) },
            { F_Mask,                       CSS_COLOR( 216, 100, 255, 0.4 ) },
            { Dwgs_User,                    CSS_COLOR( 194, 194, 194, 1 ) },
            { Cmts_User,                    CSS_COLOR( 89,  148, 220, 1 ) },
            { Eco1_User,                    CSS_COLOR( 180, 219, 210, 1 ) },
            { Eco2_User,                    CSS_COLOR( 216, 200, 82,  1 ) },
            { Edge_Cuts,                    CSS_COLOR( 208, 210, 205, 1 ) },
            { Margin,                       CSS_COLOR( 255, 38,  226, 1 ) },
            { B_CrtYd,                      CSS_COLOR( 38,  233,  255, 1 ) },
            { F_CrtYd,                      CSS_COLOR( 255, 38,  226, 1 ) },
            { B_Fab,                        CSS_COLOR( 88,  93,  132, 1 ) },
            { F_Fab,                        CSS_COLOR( 175, 175, 175, 1 ) },
            { User_1,                       CSS_COLOR( 194, 194, 194, 1 ) },
            { User_2,                       CSS_COLOR( 89,  148, 220, 1 ) },
            { User_3,                       CSS_COLOR( 180, 219, 210, 1 ) },
            { User_4,                       CSS_COLOR( 216, 200, 82,  1 ) },
            { User_5,                       CSS_COLOR( 194, 194, 194, 1 ) },
            { User_6,                       CSS_COLOR( 89,  148, 220, 1 ) },
            { User_7,                       CSS_COLOR( 180, 219, 210, 1 ) },
            { User_8,                       CSS_COLOR( 216, 200, 82,  1 ) },
            { User_9,                       CSS_COLOR( 232, 178, 167, 1 ) },

            { LAYER_3D_BACKGROUND_BOTTOM,   COLOR4D( 0.4, 0.4, 0.5, 1.0 ) },
            { LAYER_3D_BACKGROUND_TOP,      COLOR4D( 0.8, 0.8, 0.9, 1.0 ) },
            { LAYER_3D_BOARD,               COLOR4D( 0.2, 0.17, 0.09, 0.9 ) },
            { LAYER_3D_COPPER,              COLOR4D( 0.7, 0.61, 0.0, 1.0 ) },
            { LAYER_3D_SILKSCREEN_BOTTOM,   COLOR4D( 0.9, 0.9, 0.9, 1.0 ) },
            { LAYER_3D_SILKSCREEN_TOP,      COLOR4D( 0.9, 0.9, 0.9, 1.0 ) },
            { LAYER_3D_SOLDERMASK_BOTTOM,   COLOR4D( 0.08, 0.2, 0.14, 0.83 ) },
            { LAYER_3D_SOLDERMASK_TOP,      COLOR4D( 0.08, 0.2, 0.14, 0.83 ) },
            { LAYER_3D_SOLDERPASTE,         COLOR4D( 0.5, 0.5, 0.5, 1.0 ) }
        };


static const std::map<int, COLOR4D> s_classicTheme =
        {
            { LAYER_SCHEMATIC_ANCHOR,       CSS_COLOR( 0, 0, 255, 1 ) },
            { LAYER_SCHEMATIC_AUX_ITEMS,    COLOR4D( BLACK ) },
            { LAYER_SCHEMATIC_BACKGROUND,   COLOR4D( WHITE ) },
            { LAYER_HOVERED,                COLOR4D( BLUE ) },
            { LAYER_BRIGHTENED,             COLOR4D( PUREMAGENTA ) },
            { LAYER_BUS,                    COLOR4D( BLUE ) },
            { LAYER_BUS_JUNCTION,           COLOR4D( BLUE ) },
            { LAYER_DEVICE_BACKGROUND,      COLOR4D( LIGHTYELLOW ) },
            { LAYER_DEVICE,                 COLOR4D( RED ) },
            { LAYER_SCHEMATIC_CURSOR,       COLOR4D( BLACK ) },
            { LAYER_ERC_ERR,                COLOR4D( PURERED ).WithAlpha( 0.8 ) },
            { LAYER_ERC_WARN,               COLOR4D( PUREGREEN ).WithAlpha( 0.8 ) },
            { LAYER_ERC_EXCLUSION,          COLOR4D( LIGHTGRAY ) },
            { LAYER_FIELDS,                 COLOR4D( MAGENTA ) },
            { LAYER_SCHEMATIC_GRID,         COLOR4D( DARKGRAY ) },
            { LAYER_SCHEMATIC_GRID_AXES,    COLOR4D( BLUE ) },
            { LAYER_HIDDEN,                 COLOR4D( LIGHTGRAY ) },
            { LAYER_JUNCTION,               COLOR4D( GREEN ) },
            { LAYER_GLOBLABEL,              COLOR4D( RED ) },
            { LAYER_HIERLABEL,              COLOR4D( BROWN ) },
            { LAYER_LOCLABEL,               COLOR4D( BLACK ) },
            { LAYER_NETCLASS_REFS,          COLOR4D( BLACK ) },
            { LAYER_NOCONNECT,              COLOR4D( BLUE ) },
            { LAYER_NOTES,                  COLOR4D( LIGHTBLUE ) },
            { LAYER_PRIVATE_NOTES,          COLOR4D( LIGHTBLUE ) },
            { LAYER_NOTES_BACKGROUND,       COLOR4D( UNSPECIFIED_COLOR ) },
            { LAYER_PIN,                    COLOR4D( RED ) },
            { LAYER_PINNAM,                 COLOR4D( CYAN ) },
            { LAYER_PINNUM,                 COLOR4D( RED ) },
            { LAYER_REFERENCEPART,          COLOR4D( CYAN ) },
#ifdef __WXMAC__
            // Macs look better with a lighter shadow
            { LAYER_SELECTION_SHADOWS,      COLOR4D( .78, .92, 1.0, 0.8 ) },
#else
            { LAYER_SELECTION_SHADOWS,      COLOR4D( .4, .7, 1.0, 0.8 ) },
#endif
            { LAYER_SHEET,                  COLOR4D( MAGENTA ) },
            { LAYER_SHEET_BACKGROUND,       COLOR4D( WHITE ).WithAlpha( 0.0 ) },
            { LAYER_SHEETFILENAME,          COLOR4D( BROWN ) },
            { LAYER_SHEETFIELDS,            COLOR4D( MAGENTA ) },
            { LAYER_SHEETLABEL,             COLOR4D( CYAN ) },
            { LAYER_SHEETNAME,              COLOR4D( CYAN ) },
            { LAYER_VALUEPART,              COLOR4D( CYAN ) },
            { LAYER_WIRE,                   COLOR4D( GREEN ) },
            { LAYER_SCHEMATIC_DRAWINGSHEET, COLOR4D( RED ) },
            { LAYER_OP_VOLTAGES,            COLOR4D( DARKMAGENTA ) },
            { LAYER_OP_CURRENTS,            COLOR4D( RED ) },

            { LAYER_GERBVIEW_AXES,          COLOR4D( BLUE ) },
            { LAYER_GERBVIEW_BACKGROUND,    COLOR4D( BLACK ) },
            { LAYER_DCODES,                 COLOR4D( WHITE ) },
            { LAYER_GERBVIEW_GRID,          COLOR4D( MAGENTA ) },
            { LAYER_NEGATIVE_OBJECTS,       COLOR4D( DARKGRAY ) },
            { LAYER_GERBVIEW_DRAWINGSHEET,  COLOR4D( DARKRED ) },
            { LAYER_GERBVIEW_PAGE_LIMITS,   COLOR4D( MAGENTA ) },

            { GERBVIEW_LAYER_ID_START,      COLOR4D( RED ) },
            { GERBVIEW_LAYER_ID_START + 1,  COLOR4D( YELLOW ) },
            { GERBVIEW_LAYER_ID_START + 2,  COLOR4D( LIGHTMAGENTA ) },
            { GERBVIEW_LAYER_ID_START + 3,  COLOR4D( LIGHTRED ) },
            { GERBVIEW_LAYER_ID_START + 4,  COLOR4D( CYAN ) },
            { GERBVIEW_LAYER_ID_START + 5,  COLOR4D( GREEN ) },
            { GERBVIEW_LAYER_ID_START + 6,  COLOR4D( BLUE ) },
            { GERBVIEW_LAYER_ID_START + 7,  COLOR4D( DARKGRAY ) },
            { GERBVIEW_LAYER_ID_START + 8,  COLOR4D( MAGENTA ) },
            { GERBVIEW_LAYER_ID_START + 9,  COLOR4D( LIGHTGRAY ) },
            { GERBVIEW_LAYER_ID_START + 10, COLOR4D( MAGENTA ) },
            { GERBVIEW_LAYER_ID_START + 11, COLOR4D( RED ) },
            { GERBVIEW_LAYER_ID_START + 12, COLOR4D( BROWN ) },
            { GERBVIEW_LAYER_ID_START + 13, COLOR4D( LIGHTGRAY ) },
            { GERBVIEW_LAYER_ID_START + 14, COLOR4D( BLUE ) },
            { GERBVIEW_LAYER_ID_START + 15, COLOR4D( GREEN ) },
            { GERBVIEW_LAYER_ID_START + 16, COLOR4D( RED ) },
            { GERBVIEW_LAYER_ID_START + 17, COLOR4D( YELLOW ) },
            { GERBVIEW_LAYER_ID_START + 18, COLOR4D( LIGHTMAGENTA ) },
            { GERBVIEW_LAYER_ID_START + 19, COLOR4D( LIGHTRED ) },
            { GERBVIEW_LAYER_ID_START + 20, COLOR4D( CYAN ) },
            { GERBVIEW_LAYER_ID_START + 21, COLOR4D( GREEN ) },
            { GERBVIEW_LAYER_ID_START + 22, COLOR4D( BLUE ) },
            { GERBVIEW_LAYER_ID_START + 23, COLOR4D( DARKGRAY ) },
            { GERBVIEW_LAYER_ID_START + 24, COLOR4D( MAGENTA ) },
            { GERBVIEW_LAYER_ID_START + 25, COLOR4D( LIGHTGRAY ) },
            { GERBVIEW_LAYER_ID_START + 26, COLOR4D( MAGENTA ) },
            { GERBVIEW_LAYER_ID_START + 27, COLOR4D( RED ) },
            { GERBVIEW_LAYER_ID_START + 28, COLOR4D( BROWN ) },
            { GERBVIEW_LAYER_ID_START + 29, COLOR4D( LIGHTGRAY ) },
            { GERBVIEW_LAYER_ID_START + 30, COLOR4D( BLUE ) },
            { GERBVIEW_LAYER_ID_START + 31, COLOR4D( GREEN ) },
            { GERBVIEW_LAYER_ID_START + 32, COLOR4D( RED ) },
            { GERBVIEW_LAYER_ID_START + 33, COLOR4D( YELLOW ) },
            { GERBVIEW_LAYER_ID_START + 34, COLOR4D( LIGHTMAGENTA ) },
            { GERBVIEW_LAYER_ID_START + 35, COLOR4D( LIGHTRED ) },
            { GERBVIEW_LAYER_ID_START + 36, COLOR4D( CYAN ) },
            { GERBVIEW_LAYER_ID_START + 37, COLOR4D( GREEN ) },
            { GERBVIEW_LAYER_ID_START + 38, COLOR4D( BLUE ) },
            { GERBVIEW_LAYER_ID_START + 39, COLOR4D( DARKGRAY ) },
            { GERBVIEW_LAYER_ID_START + 40, COLOR4D( MAGENTA ) },
            { GERBVIEW_LAYER_ID_START + 41, COLOR4D( LIGHTGRAY ) },
            { GERBVIEW_LAYER_ID_START + 42, COLOR4D( MAGENTA ) },
            { GERBVIEW_LAYER_ID_START + 43, COLOR4D( RED ) },
            { GERBVIEW_LAYER_ID_START + 44, COLOR4D( BROWN ) },
            { GERBVIEW_LAYER_ID_START + 45, COLOR4D( LIGHTGRAY ) },
            { GERBVIEW_LAYER_ID_START + 46, COLOR4D( BLUE ) },
            { GERBVIEW_LAYER_ID_START + 47, COLOR4D( GREEN ) },
            { GERBVIEW_LAYER_ID_START + 48, COLOR4D( RED ) },
            { GERBVIEW_LAYER_ID_START + 49, COLOR4D( YELLOW ) },
            { GERBVIEW_LAYER_ID_START + 50, COLOR4D( LIGHTMAGENTA ) },
            { GERBVIEW_LAYER_ID_START + 51, COLOR4D( LIGHTRED ) },
            { GERBVIEW_LAYER_ID_START + 52, COLOR4D( CYAN ) },
            { GERBVIEW_LAYER_ID_START + 53, COLOR4D( GREEN ) },
            { GERBVIEW_LAYER_ID_START + 54, COLOR4D( BLUE ) },
            { GERBVIEW_LAYER_ID_START + 55, COLOR4D( DARKGRAY ) },
            { GERBVIEW_LAYER_ID_START + 56, COLOR4D( MAGENTA ) },
            { GERBVIEW_LAYER_ID_START + 57, COLOR4D( LIGHTGRAY ) },
            { GERBVIEW_LAYER_ID_START + 58, COLOR4D( MAGENTA ) },
            { GERBVIEW_LAYER_ID_START + 59, COLOR4D( RED ) },

            { LAYER_ANCHOR,                 COLOR4D( BLUE ) },
            { LAYER_LOCKED_ITEM_SHADOW,     COLOR4D( BLUE ).WithAlpha( 0.6 ) },
            { LAYER_CONFLICTS_SHADOW,       COLOR4D( RED ).WithAlpha( 0.5 ) },
            { LAYER_AUX_ITEMS,              COLOR4D( WHITE ) },
            { LAYER_PCB_BACKGROUND,         COLOR4D( BLACK ) },
            { LAYER_CURSOR,                 COLOR4D( WHITE ) },
            { LAYER_DRC_ERROR,              COLOR4D( PURERED ).WithAlpha( 0.8 ) },
            { LAYER_DRC_WARNING,            COLOR4D( PUREGREEN ).WithAlpha( 0.8 ) },
            { LAYER_DRC_EXCLUSION,          COLOR4D( WHITE ) },
            { LAYER_MOD_TEXT_INVISIBLE,     COLOR4D( LIGHTGRAY ) },
            { LAYER_GRID,                   COLOR4D( DARKGRAY ) },
            { LAYER_GRID_AXES,              COLOR4D( BLUE ) },
            { LAYER_PAD_PLATEDHOLES,        COLOR4D( YELLOW ) },
            { LAYER_PADS_TH,                COLOR4D( YELLOW ) },
            { LAYER_NON_PLATEDHOLES,        COLOR4D( YELLOW ) },
            { LAYER_RATSNEST,               COLOR4D( WHITE ) },
            { LAYER_SELECT_OVERLAY,         COLOR4D( PUREGREEN ) },
            { LAYER_VIA_THROUGH,            COLOR4D( LIGHTGRAY ) },
            { LAYER_VIA_BBLIND,             COLOR4D( BROWN ) },
            { LAYER_VIA_HOLES,              COLOR4D( 0.5, 0.4, 0, 0.8 ) },
            { LAYER_VIA_MICROVIA,           COLOR4D( CYAN ) },
            { LAYER_DRAWINGSHEET,           COLOR4D( DARKRED ) },
            { LAYER_PAGE_LIMITS,            COLOR4D( DARKGRAY) },

            { F_Cu,                         COLOR4D( RED ) },
            { In1_Cu,                       COLOR4D( YELLOW ) },
            { In2_Cu,                       COLOR4D( LIGHTMAGENTA ) },
            { In3_Cu,                       COLOR4D( LIGHTRED ) },
            { In4_Cu,                       COLOR4D( CYAN ) },
            { In5_Cu,                       COLOR4D( GREEN ) },
            { In6_Cu,                       COLOR4D( BLUE ) },
            { In7_Cu,                       COLOR4D( DARKGRAY ) },
            { In8_Cu,                       COLOR4D( MAGENTA ) },
            { In9_Cu,                       COLOR4D( LIGHTGRAY ) },
            { In10_Cu,                      COLOR4D( MAGENTA ) },
            { In11_Cu,                      COLOR4D( RED ) },
            { In12_Cu,                      COLOR4D( BROWN ) },
            { In13_Cu,                      COLOR4D( LIGHTGRAY ) },
            { In14_Cu,                      COLOR4D( BLUE ) },
            { In15_Cu,                      COLOR4D( GREEN ) },
            { In16_Cu,                      COLOR4D( RED ) },
            { In17_Cu,                      COLOR4D( YELLOW ) },
            { In18_Cu,                      COLOR4D( LIGHTMAGENTA ) },
            { In19_Cu,                      COLOR4D( LIGHTRED ) },
            { In20_Cu,                      COLOR4D( CYAN ) },
            { In21_Cu,                      COLOR4D( GREEN ) },
            { In22_Cu,                      COLOR4D( BLUE ) },
            { In23_Cu,                      COLOR4D( DARKGRAY ) },
            { In24_Cu,                      COLOR4D( MAGENTA ) },
            { In25_Cu,                      COLOR4D( LIGHTGRAY ) },
            { In26_Cu,                      COLOR4D( MAGENTA ) },
            { In27_Cu,                      COLOR4D( RED ) },
            { In28_Cu,                      COLOR4D( BROWN ) },
            { In29_Cu,                      COLOR4D( LIGHTGRAY ) },
            { In30_Cu,                      COLOR4D( BLUE ) },
            { B_Cu,                         COLOR4D( GREEN ) },

            { B_Adhes,                      COLOR4D( BLUE ) },
            { F_Adhes,                      COLOR4D( MAGENTA ) },
            { B_Paste,                      COLOR4D( LIGHTCYAN ) },
            { F_Paste,                      COLOR4D( RED ) },
            { B_SilkS,                      COLOR4D( MAGENTA ) },
            { F_SilkS,                      COLOR4D( CYAN ) },
            { B_Mask,                       COLOR4D( BROWN ) },
            { F_Mask,                       COLOR4D( MAGENTA ) },
            { Dwgs_User,                    COLOR4D( LIGHTGRAY ) },
            { Cmts_User,                    COLOR4D( BLUE ) },
            { Eco1_User,                    COLOR4D( GREEN ) },
            { Eco2_User,                    COLOR4D( YELLOW ) },
            { Edge_Cuts,                    COLOR4D( YELLOW ) },
            { Margin,                       COLOR4D( LIGHTMAGENTA ) },
            { B_CrtYd,                      COLOR4D( DARKGRAY ) },
            { F_CrtYd,                      COLOR4D( LIGHTGRAY ) },
            { B_Fab,                        COLOR4D( BLUE ) },
            { F_Fab,                        COLOR4D( DARKGRAY ) },
            { User_1,                       COLOR4D( BLUE ) },
            { User_2,                       COLOR4D( BLUE ) },
            { User_3,                       COLOR4D( BLUE ) },
            { User_4,                       COLOR4D( BLUE ) },
            { User_5,                       COLOR4D( BLUE ) },
            { User_6,                       COLOR4D( BLUE ) },
            { User_7,                       COLOR4D( BLUE ) },
            { User_8,                       COLOR4D( BLUE ) },
            { User_9,                       COLOR4D( BLUE ) },

            { LAYER_3D_BACKGROUND_BOTTOM,   COLOR4D( 0.4, 0.4, 0.5, 1.0 ) },
            { LAYER_3D_BACKGROUND_TOP,      COLOR4D( 0.8, 0.8, 0.9, 1.0 ) },
            { LAYER_3D_BOARD,               COLOR4D( 0.2, 0.17, 0.09, 0.9 ) },
            { LAYER_3D_COPPER,              COLOR4D( 0.7, 0.61, 0.0, 1.0 ) },
            { LAYER_3D_SILKSCREEN_BOTTOM,   COLOR4D( 0.9, 0.9, 0.9, 1.0 ) },
            { LAYER_3D_SILKSCREEN_TOP,      COLOR4D( 0.9, 0.9, 0.9, 1.0 ) },
            { LAYER_3D_SOLDERMASK_BOTTOM,   COLOR4D( 0.08, 0.2, 0.14, 0.83 ) },
            { LAYER_3D_SOLDERMASK_TOP,      COLOR4D( 0.08, 0.2, 0.14, 0.83 ) },
            { LAYER_3D_SOLDERPASTE,         COLOR4D( 0.5, 0.5, 0.5, 1.0 ) }
        };

#endif
