
dos2unix - DOS/Mac to Unix and vice versa text file format converter.


FILE LIST

        README.txt      : This file.
        INSTALL.txt     : How to build and install.
        COPYING.txt     : distribution license.
        NEWS.txt        : Basic change log for users.
        ChangeLog.txt   : Detailed change log for programmers.
        TODO.txt        : Things to do.
        BUGS.txt        : Known bugs and instructions on reporting new ones.
        dos2unix.txt    : Dos2unix manual, text format.
        dos2unix.ps     : Dos2unix manual, PostScript format.
        dos2unix.pdf    : Dos2unix manual, PDF format.
        dos2unix.htm    : Dos2unix manual, HTML format.


HISTORY

        This is an update of Benjamin Lin's implementations of dos2unix and
        unix2dos.  Benjamin Lin's implementations of dos2unix and unix2dos are
        part of several Linux distributions such as RedHat, Fedora, Suse and
        others.  This update includes all RedHat patches and fixes several
        other problems.  Internationalization has been added and ports to DOS,
        Windows, Cygwin and OS/2 Warp have been made. 

        These implementations of dos2unix and unix2dos have been modelled after
        dos2unix/unix2dos under SunOS/Solaris.  They have similar conversion
        modes, namely ascii, 7bit and iso.  The first versions were made by
        John Birchfield in 1989, and in 1995 rewritten from scratch by Benjamin
        Lin. Mac to Unix conversion was added by Bernd Johannes Wuebben in
        1998, Unix to Mac conversion by Erwin Waterlander in 2010.

        Features

        * Native language support: Dutch, English, Esperanto, German, Spanish.
        * Automatically skips binary and non-regular files
        * In-place, paired, or stdio mode conversion.
        * Keep original file dates option.
        * 7-bit and iso conversion modes like SunOS dos2unix.
        * Conversion of Windows UTF-16 files to Unix UTF-8.


AUTHORS

        Erwin Waterlander       version 3.2-6.0    2009-2012 
        Christian Wurll         version 3.1        1998
        Bernd Johannes Wuebben  version 3.0        1998
        Benjamin Lin            version 1.1-2.3    1994-1995
        John Birchfield         version 1.0        1989

TRANSLATORS

        Dutch           Erwin Waterlander <waterlan@xs4all.nl>
        Esperanto       Rugxulo <rugxulo@gmail.com>
        German          Philipp Thomas <psmt@opensuse.org>
        Spanish         Julio A. Freyre-Gonzalez <jfreyreg@gmail.com>

ACKNOWLEDGEMENTS

        Rugxulo               query_con_codepage(), code page detection in DOS.
        Rugxulo               DJGPP stubs for mac2unix and unix2mac.
        Jari Aalto            Improvements man page.
        Jari Aalto            Improvments Makefile.
        Ben Pfaff             Treat form feeds as valid text.
        Marc Gianzero         Darwin OS port.
        Elbert Pol            OS/2 port.
        Tim Waugh             Various patches.
        Mike Harris           Man page update.
        Bernard RosenKraenzer segfault patch.
        Tim Powers            mkstemp patch.
        Miloslav Trmac        safeconv patch.
        Charles Wilson        Follow symbolic links.
        Charles Wilson        Makefile and code cleanup for Cygwin.
        Christopher Williams  Maintain file ownership in old file mode.
        Steven H. Levine      Support wildcard expansion on OS/2.
        Steve Hay             Support wildcard expansion on win64.

CONTACT INFORMATION

        Project page             : http://waterlan.home.xs4all.nl/dos2unix.html
        SourceForge project page : http://sourceforge.net/projects/dos2unix/
        Freecode project page    : http://freecode.com/projects/dos2unix/

        Erwin Waterlander
        waterlan@xs4all.nl
        http://waterlan.home.xs4all.nl/

