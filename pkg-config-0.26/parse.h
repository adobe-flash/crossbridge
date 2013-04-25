/* 
 * Copyright (C) 2001, 2002 Red Hat Inc.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef PKG_CONFIG_PARSE_H
#define PKG_CONFIG_PARSE_H

#include "pkg.h"

Package *parse_package_file (const char *path, gboolean ignore_requires,
			     gboolean ignore_private_libs,
			     gboolean ignore_requires_private);

GSList  *parse_module_list (Package *pkg, const char *str, const char *path);

#endif



