/* WIN32Server - Implements window handling for MSWindows

   Copyright (C) 2005 Free Software Foundation, Inc.

   Written by: Fred Kiefer <FredKiefer@gmx.de>
   Date: March 2002
   Part of this code have been re-written by:
   Tom MacSween <macsweent@sympatico.ca>
   Date August 2005
   This file is part of the GNU Objective C User Interface Library.

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

#include <AppKit/NSEvent.h>
#include <AppKit/NSWindow.h>
#include <Foundation/NSBundle.h>

#include "win32/WIN32Server.h"
#include "win32/WIN32Geometry.h"

@implementation WIN32Server (w32_create)

- (LRESULT) decodeWM_NCCREATEParams: (WPARAM)wParam : (LPARAM)lParam 
                                   : (HWND)hwnd
{
  return TRUE;
}

- (LRESULT) decodeWM_CREATEParams: (WPARAM)wParam : (LPARAM)lParam 
                                 : (HWND)hwnd
{
  WIN_INTERN *win;
  NSBackingStoreType type = (NSBackingStoreType)((LPCREATESTRUCT)lParam)->lpCreateParams;
  NSBundle *bundle = [NSBundle mainBundle];
  NSString *iconName = nil;
  NSString *iconPath = nil;

  // Initialize the window. 
  NSDebugLLog(@"NSEvent", @"Got Message %s for %d", "CREATE", hwnd);
  /* For windows with backingstore we create a compatible DC, that 
	   is stored in the extra fields for this window. Drawing operations 
	   work on this buffer. */
  win = malloc(sizeof(WIN_INTERN));
  SetWindowLong(hwnd, GWL_USERDATA, (int)win);
	
  if (type != NSBackingStoreNonretained)
    {
      HDC hdc, hdc2;
      HBITMAP hbitmap;
      RECT r;

      GetClientRect((HWND)hwnd, &r);
      hdc = GetDC(hwnd);
      hdc2 = CreateCompatibleDC(hdc);
      hbitmap = CreateCompatibleBitmap(hdc, r.right - r.left, 
                                       r.bottom - r.top);
      win->old = SelectObject(hdc2, hbitmap);

      win->hdc = hdc2;
      win->useHDC = YES;
	    
      ReleaseDC(hwnd, hdc);
    }
  else
    {
      win->useHDC = NO;
    }

  // Find the icon file, assume it has the same name as the "icon" which
  // was specified in the bundle's dictionary...
  iconName = [[bundle infoDictionary] objectForKey: @"NSIcon"];
  if(iconName == nil)
    {
      iconName = [[bundle infoDictionary] 
		    objectForKey: @"CFBundleIconFile"];
    }

  // If the icon name is set, get the path...
  if(iconName != nil)
    {
      iconName = [iconName stringByDeletingPathExtension];
      iconPath = [[NSBundle mainBundle] pathForResource: iconName 
						 ofType: @"ico"];
      iconPath = [iconPath stringByStandardizingPath];
    }
  
  // If the path is set, load the icon file and set it as the
  // icon on the window.
  if(iconPath != nil)
    {
      HICON icon = NULL;
      const char *cpath = [iconPath cString];
      
      icon = LoadImage(NULL, 
		       cpath,
		       IMAGE_ICON,0,0,
		       LR_DEFAULTSIZE|LR_LOADFROMFILE);
      SetClassLongPtr(hwnd,GCLP_HICON,(LONG_PTR)icon);
    }

  return 0;
}
@end
