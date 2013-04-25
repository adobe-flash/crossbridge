# To the extent possible under law, the author(s) have dedicated all 
# copyright and related and neighboring rights to this software to the 
# public domain worldwide. This software is distributed without any warranty. 
# You should have received a copy of the CC0 Public Domain Dedication along 
# with this software. 
# If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 

# base-files version 4.1-1

# /etc/profile.d/lang.csh: sourced by /etc/profile.

# The latest version as installed by the Cygwin Setup program can
# always be found at /etc/defaults/etc/profile.d/lang.csh

# Modifying /etc/profile.d/lang.csh directly will prevent
# setup from updating it.

# System-wide lang.csh file

# if no locale variable is set, indicate terminal charset via LANG
if ( $?LC_ALL == 0 && $?LC_CTYPE == 0 && $?LANG == 0 ) setenv LANG `/usr/bin/locale -uU`
