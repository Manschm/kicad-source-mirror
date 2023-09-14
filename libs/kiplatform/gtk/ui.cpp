/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2020 Ian McInerney <Ian.S.McInerney at ieee.org>
 * Copyright (C) 2020-2021 KiCad Developers, see AUTHORS.txt for contributors.
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

#include <kiplatform/ui.h>

#include <wx/choice.h>
#include <wx/nonownedwnd.h>
#include <wx/settings.h>
#include <wx/window.h>

#include <gtk/gtk.h>
#include <gdk/gdk.h>

#ifdef GDK_WINDOWING_X11
#include <gdk/gdkx.h>
#endif

#ifdef GDK_WINDOWING_WAYLAND
#include <gdk/gdkwayland.h>
#endif

#ifdef KICAD_WAYLAND
#include "wayland-pointer-constraints-unstable-v1.h"
#endif

bool KIPLATFORM::UI::IsDarkTheme()
{
    wxColour bg = wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW );

    // Weighted W3C formula
    double brightness = ( bg.Red() / 255.0 ) * 0.299 +
                        ( bg.Green() / 255.0 ) * 0.587 +
                        ( bg.Blue() / 255.0 ) * 0.117;

    return brightness < 0.5;
}


wxColour KIPLATFORM::UI::GetDialogBGColour()
{
    return wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE );
}


void KIPLATFORM::UI::ForceFocus( wxWindow* aWindow )
{
    aWindow->SetFocus();
}


bool KIPLATFORM::UI::IsWindowActive( wxWindow* aWindow )
{
    if( !aWindow )
        return false;

    GtkWindow* window = GTK_WINDOW( aWindow->GetHandle() );

    if( window )
        return gtk_window_is_active( window );

    // We shouldn't really ever reach this point
    return false;
}


void KIPLATFORM::UI::ReparentQuasiModal( wxNonOwnedWindow* aWindow )
{
    // Not needed on this platform
}


void KIPLATFORM::UI::FixupCancelButtonCmdKeyCollision( wxWindow *aWindow )
{
    // Not needed on this platform
}


bool KIPLATFORM::UI::IsStockCursorOk( wxStockCursor aCursor )
{
    switch( aCursor )
    {
    case wxCURSOR_BULLSEYE:
    case wxCURSOR_HAND:
    case wxCURSOR_ARROW:
    case wxCURSOR_BLANK:
        return true;
    default:
        return false;
    }
}


/**
 * The following two functions are based on the "hack" contained in the attached patch at
 * https://gitlab.gnome.org/GNOME/gtk/-/issues/1910 which is supposed to speed up creation of
 * GTK choice boxes.
 *
 * The basic idea is to disable some of the event handlers on the menus for the choice box to
 * prevent them from running, which will speed up the creation of the choice box and its popup menu.
 */
static void disable_area_apply_attributes_cb( GtkWidget* pItem, gpointer userdata )
{
    // GTK needs this enormous chain to get the actual type of item that we want
    GtkMenuItem*   pMenuItem   = GTK_MENU_ITEM( pItem );
    GtkWidget*     child       = gtk_bin_get_child( GTK_BIN( pMenuItem ) );
    GtkCellView*   pCellView   = GTK_CELL_VIEW( child );
    GtkCellLayout* pCellLayout = GTK_CELL_LAYOUT( pCellView );
    GtkCellArea*   pCellArea   = gtk_cell_layout_get_area( pCellLayout );

    g_signal_handlers_block_matched( pCellArea, G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, userdata );
}


void KIPLATFORM::UI::LargeChoiceBoxHack( wxChoice* aChoice )
{
    AtkObject* atkObj = gtk_combo_box_get_popup_accessible( GTK_COMBO_BOX( aChoice->m_widget ) );

    if( !atkObj || !GTK_IS_ACCESSIBLE( atkObj ) )
        return;

    GtkWidget* widget = gtk_accessible_get_widget( GTK_ACCESSIBLE( atkObj ) );

    if( !widget || !GTK_IS_MENU( widget ) )
        return;

    GtkMenu* menu = GTK_MENU( widget );

    gtk_container_foreach( GTK_CONTAINER( menu ), disable_area_apply_attributes_cb, menu );
}


void KIPLATFORM::UI::EllipsizeChoiceBox( wxChoice* aChoice )
{
    // This function is based on the code inside the function post_process_ui in
    // gtkfilechooserwidget.c
    GList* cells = gtk_cell_layout_get_cells( GTK_CELL_LAYOUT( aChoice->m_widget ) );

    if( !cells )
        return;

    GtkCellRenderer* cell = (GtkCellRenderer*) cells->data;

    if( !cell )
        return;

    g_object_set( G_OBJECT( cell ), "ellipsize", PANGO_ELLIPSIZE_END, nullptr );

    // Only the list of cells must be freed, the renderer isn't ours to free
    g_list_free( cells );
}


double KIPLATFORM::UI::GetPixelScaleFactor( const wxWindow* aWindow )
{
    double val = 1.0;

    GtkWidget* widget = static_cast<GtkWidget*>( aWindow->GetHandle() );

    if( widget && gtk_check_version( 3, 10, 0 ) == nullptr )
        val = gtk_widget_get_scale_factor( widget );

    return val;
}


double KIPLATFORM::UI::GetContentScaleFactor( const wxWindow* aWindow )
{
    // TODO: Do we need something different here?
    return GetPixelScaleFactor( aWindow );
}


wxSize KIPLATFORM::UI::GetUnobscuredSize( const wxWindow* aWindow )
{
    return wxSize( aWindow->GetSize().GetX() - wxSystemSettings::GetMetric( wxSYS_VSCROLL_X ),
                   aWindow->GetSize().GetY() - wxSystemSettings::GetMetric( wxSYS_HSCROLL_Y ) );
}


void KIPLATFORM::UI::SetOverlayScrolling( const wxWindow* aWindow, bool overlay )
{
    gtk_scrolled_window_set_overlay_scrolling( GTK_SCROLLED_WINDOW( aWindow->GetHandle() ),
                                               overlay );
}


bool KIPLATFORM::UI::AllowIconsInMenus()
{
    gboolean allowed = 1;

    g_object_get( gtk_settings_get_default(), "gtk-menu-images", &allowed, NULL );

    return !!allowed;
}

#ifdef KICAD_WAYLAND

static struct zwp_pointer_constraints_v1* pointer_constraints = NULL;

static void handle_global( void* data, struct wl_registry* registry, uint32_t name,
                           const char* interface, uint32_t version )
{
    if( strcmp( interface, zwp_pointer_constraints_v1_interface.name ) == 0 )
    {
        pointer_constraints = static_cast<zwp_pointer_constraints_v1*>( wl_registry_bind(
                registry, name, &zwp_pointer_constraints_v1_interface, version ) );
    }
}

static const struct wl_registry_listener registry_listener = {
	.global = handle_global,
	.global_remove = NULL,
};

#endif


void KIPLATFORM::UI::WarpPointer( wxWindow* aWindow, int aX, int aY )
{
    if( !wxGetEnv( wxT( "WAYLAND_DISPLAY" ), nullptr ) )
    {
        aWindow->WarpPointer( aX, aY );
    }
    else
    {
        GtkWidget* widget = static_cast<GtkWidget*>( aWindow->GetHandle() );

        GdkDisplay* disp = gtk_widget_get_display( widget );
        GdkSeat*    seat = gdk_display_get_default_seat( disp );
        GdkDevice*  ptrdev = gdk_seat_get_pointer( seat );

#if defined( GDK_WINDOWING_WAYLAND ) && defined KICAD_WAYLAND
        if( GDK_IS_WAYLAND_DISPLAY( disp ) )
        {
            GdkWindow* win = aWindow->GTKGetDrawingWindow();

            wl_display* wldisp = gdk_wayland_display_get_wl_display( disp );

            struct wl_registry* registry = wl_display_get_registry( wldisp );
            int lret = wl_registry_add_listener( registry, &registry_listener, NULL );

            wl_display_roundtrip( wldisp );

            wl_surface* wlsurf = gdk_wayland_window_get_wl_surface( win );
            wl_pointer* wlptr = gdk_wayland_device_get_wl_pointer( ptrdev );

            struct zwp_locked_pointer_v1* locked_pointer = zwp_pointer_constraints_v1_lock_pointer(
                    pointer_constraints, wlsurf, wlptr, NULL,
                    ZWP_POINTER_CONSTRAINTS_V1_LIFETIME_ONESHOT );

            gint wx, wy;
            gtk_widget_translate_coordinates( widget, gtk_widget_get_toplevel( widget ), 0, 0, &wx,
                                              &wy );

            zwp_locked_pointer_v1_set_cursor_position_hint(
                    locked_pointer, wl_fixed_from_int( aX + wx ), wl_fixed_from_int( aY + wy ) );

            wl_surface_commit( wlsurf );
            wl_display_roundtrip( wldisp );

            zwp_locked_pointer_v1_destroy( locked_pointer );
        }
#endif
#ifdef GDK_WINDOWING_X11
        if( GDK_IS_X11_DISPLAY( disp ) )
        {
            GdkWindow* win = gdk_device_get_window_at_position( ptrdev, nullptr, nullptr );
            GdkCursor* blank_cursor = gdk_cursor_new_for_display( disp, GDK_BLANK_CURSOR );
            GdkCursor* cur_cursor = gdk_window_get_cursor( win );

            if( cur_cursor )
                g_object_ref( cur_cursor );

            gdk_window_set_cursor( win, blank_cursor );
            aWindow->WarpPointer( aX, aY );
            gdk_window_set_cursor( win, cur_cursor );
        }
#endif
    }
}


void KIPLATFORM::UI::ImmControl( wxWindow* aWindow, bool aEnable )
{
}