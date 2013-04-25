/*
   Win32CairoSurface.m

   Copyright (C) 2008 Free Software Foundation, Inc.

   Author: Xavier Glattard <xavier.glattard@online.fr>
   Based on the work of:
     Banlu Kemiyatorn <object at gmail dot com>

   This file is part of GNUstep.
   
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

#include "cairo/Win32CairoSurface.h"
#include <cairo-win32.h>

#define GSWINDEVICE ((HWND)gsDevice)

@implementation Win32CairoSurface

- (id) initWithDevice: (void *)device
{
  HDC hDC;
  WIN_INTERN *win;
  gsDevice = device;

  win = (WIN_INTERN *)GetWindowLong(GSWINDEVICE, GWL_USERDATA);
  if (win && win->useHDC)
    hDC = win->hdc;
  else
    hDC = GetDC(GSWINDEVICE);
  
  if (!hDC)
    {
      NSLog(@"Win32CairoSurface : %d : no device context",__LINE__);
      exit(1);
    }

  _surface = cairo_win32_surface_create(hDC);
  if (!(win && win->useHDC))
    ReleaseDC(GSWINDEVICE, hDC);
  if (cairo_surface_status(_surface))
    {
      DESTROY(self);
    }

  return self;
}

- (NSSize) size
{
  RECT sz;

  GetClientRect(GSWINDEVICE, &sz);
  return NSMakeSize(sz.right - sz.left, sz.top - sz.bottom);
}

@end
