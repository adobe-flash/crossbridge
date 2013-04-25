#! /usr/bin/env perl

# groffer - display groff files

# Source file position: <groff-source>/contrib/groffer/perl/perl_test.sh
# Installed position: <prefix>/lib/groff/groffer/perl_test.sh

# Copyright (C) 2006, 2009 Free Software Foundation, Inc.
# Written by Bernd Warken.

# Last update: 5 Jan 2009

# This file is part of `groffer', which is part of `groff'.

# `groff' is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# `groff' is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

########################################################################

# This file tests whether perl has a suitable version.  It is used by
# groffer.pl and Makefile.sub.

# require 5.004_05;
require v5.6.1;
