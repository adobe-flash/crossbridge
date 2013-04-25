/* Generic header info common to X backends for GNUstep

   Copyright (C) 2000 Free Software Foundation, Inc.

   Written by:  Richard Frith-Macdonald <rfm@gnu.org>
   Date: Mar 2000
   
   This file is part of the GNUstep project

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; see the file COPYING.LIB.
   If not, see <http://www.gnu.org/licenses/> or write to the 
   Free Software Foundation, 51 Franklin Street, Fifth Floor, 
   Boston, MA 02110-1301, USA.
*/

#ifndef	INCLUDED_XGGENERIC_H
#define	INCLUDED_XGGENERIC_H

/*
 * Flags to indicate which protocols the WindowManager follows
 */
typedef	enum {
  XGWM_UNKNOWN = 0,
  XGWM_WINDOWMAKER = 1,
  XGWM_GNOME = 2,
  XGWM_KDE = 4,
  XGWM_EWMH = 8
} XGWMProtocols;

typedef struct {
  Atom win_type_atom;
  Atom win_desktop_atom;
  Atom win_normal_atom;
  Atom win_floating_atom;
  Atom win_menu_atom;
  Atom win_dock_atom;
  Atom win_modal_atom;
  Atom win_utility_atom;
  Atom win_splash_atom;
  Atom win_override_atom;
  Atom win_topmenu_atom;
  Atom win_popup_menu_atom;
  Atom win_dropdown_menu_atom;
  Atom win_tooltip_atom;
  Atom win_notification_atom;
  Atom win_combo_atom;
  Atom win_dnd_atom;
} XGWMWinTypes;

typedef struct {
  Atom net_wm_state_atom;
  Atom new_wm_state_modal_atom;
  Atom net_wm_state_sticky_atom;
  Atom net_wm_state_maximized_vert_atom;
  Atom net_wm_state_maximized_horz_atom;
  Atom net_wm_state_shaded_atom;
  Atom net_wm_state_skip_taskbar_atom;
  Atom net_wm_state_skip_pager_atom;
  Atom net_wm_state_hidden_atom;
  Atom net_wm_state_fullscreen_atom;
  Atom net_wm_state_above_atom;
  Atom net_wm_state_below_atom;
  Atom net_wm_state_demands_attention_atom;
} XGWMNetStates;

/*
 * Frame offsets for window inside parent decoration window.
 */
typedef struct {
  short	l;	// offset from left
  short	r;	// offset from right
  short	t;	// offset from top
  short	b;	// offset from bottom
  BOOL	known;	// obtained from Reparent event or just guessed?
} Offsets;

/*
 * Structure containing ivars that are common to all X backend contexts.
 */
struct XGGeneric {
  int   		wm;
  struct {
    unsigned	useWindowMakerIcons:1;
    unsigned    appOwnsMiniwindow:1;
    unsigned    doubleParentWindow:1;
  } flags;
  // Time of last X event
  Time			lastTime;
  // Approximate local time for last X event, used to decide 
  // if the last X event time is still valid.
  NSTimeInterval lastTimeStamp;
  // last reference time on X server, used to prevent time drift between
  // local machine and X server.
  Time baseXServerTime;
  Time			lastClick;
  Window		lastClickWindow;
  int			lastClickX;
  int			lastClickY;
  Time			lastMotion;
  Atom			protocols_atom;
  Atom			delete_win_atom;
  Atom			take_focus_atom;
  Atom			wm_state_atom;
  Atom			net_wm_ping_atom;
  Atom                  net_wm_sync_request_atom;
  Atom                  net_wm_sync_request_counter_atom;
  Atom			miniaturize_atom;
  Atom			win_decor_atom;
  Atom			titlebar_state_atom;
  char			*rootName;
  long			currentFocusWindow;
  long			desiredFocusWindow;
  unsigned long		focusRequestNumber;
  unsigned char		lMouse;
  unsigned char		mMouse;
  unsigned char		rMouse;
  unsigned char		upMouse;
  unsigned char		downMouse;
  unsigned char		scrollLeftMouse;
  unsigned char		scrollRightMouse;
  int			lMouseMask;
  int			mMouseMask;
  int			rMouseMask;
  Window		appRootWindow;
  void			*cachedWindow;	// last gswindow_device_t used.
  Offsets		offsets[16];
  XGWMWinTypes          wintypes;
  XGWMNetStates		netstates;
};

/* GNOME Window layers */
#define WIN_LAYER_DESKTOP                0
#define WIN_LAYER_BELOW                  2
#define WIN_LAYER_NORMAL                 4
#define WIN_LAYER_ONTOP                  6
#define WIN_LAYER_DOCK                   8
#define WIN_LAYER_ABOVE_DOCK             10
#define WIN_LAYER_MENU                   12

/* NET WM State */
#define _NET_WM_STATE_REMOVE        0    /* remove/unset property */
#define _NET_WM_STATE_ADD           1    /* add/set property */
#define _NET_WM_STATE_TOGGLE        2    /* toggle property  */

#endif

