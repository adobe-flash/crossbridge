#! /usr/bin/env perl

# groffer - display groff files

# Source file position: <groff-source>/contrib/groffer/perl/man.pl
# Installed position: <prefix>/lib/groff/groffer/man.pl

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

use strict;
use warnings;

########################################################################
# functions for apropos, man, whatis
########################################################################

##########
# apropos_filespec()
#
# Compose temporary file for filspec.
#
# Globals:  in: $main::Opt{'APROPOS'}, $main::Special_Setup, $main::Filespec_Arg,
#               $main::Apropos_Prog, $main::Opt{'APROPOS_SECTIONS'}, $main::Opt{'SECTIONS'}
#          out: $main::Special_Filespec
#
sub apropos_filespec {
  return 0 unless $main::Opt{'APROPOS'};
  die 'apropos_filespec(): apropos_setup() must be run first;'
    unless $main::Special_Setup;
  die 'apropos_filespec(): no $main::Filespec_Arg is set;'
    unless defined $main::Filespec_Arg;
  $main::Special_Filespec = 1;

  my $s;
  if ($main::No_Filespecs) {
    &to_tmp_line('.SH no filespec');
    $s = `$main::Apropos_Prog`;
    $s =~ s/^/\\\&/;
    &to_tmp_line($s);
    return 1;
  }

  $s = $main::Filespec_Arg;
  $s =~ s/[^\\]-/\\-/g;
  &to_tmp_line(".SH $s");

  if ( $main::Opt{'APROPOS_SECTIONS'} ) {
    $s = qr/^[^\(]*\([$main::Opt{'APROPOS_SECTIONS'}]/;
  } else {
    if ( $main::Opt{'SECTIONS'} ) {
      $s = $main::Opt{'SECTIONS'};
      $s = qr/^[^\(]*\([$s]/;
    } else {
      $s = qr/^.*\(.+\).*$/;
    }
  }

### apropos_filespec()
  my $filespec = $main::Filespec_Arg;
  $filespec =~ s#/#\\/#g;
  $filespec =~ s#\.#\\./#g;
  my @ap;
  foreach ( `$main::Apropos_Prog $main::Filespec_Arg 2>$main::Dev_Null` ) {
    chomp;
    if (/^$filespec:\s/) {	# for error messages of this script
      my $line = $_;
      $line =~ s/^(.*)$/\\\&$1/s;
      push @ap, $line;
    }
    if (/$s/) {
      push @ap, $_;
    }
  }
  my @res;
  foreach (sort @ap) {
    s/^
      ([^\(]+\(+[$main::Man{'AUTO_SEC_CHARS'}][^\)]*\)+)
      (\s*-*)*\s*
      (.*)
      $/.br\n.TP 15\n.BR "$1"\n\\\&$3\n/sx;
    push @res, $_;
  }
  &to_tmp_line(@res);
  1;
}				# apropos_filespec()


##########
# apropos_setup()
#
# Setup for the --apropos* options, just 2 global variables are set.
#
# Globals:  in: $main::Opt{'APROPOS'}
#          out: $main::Special_Setup, $main::Apropos_Prog
#
sub apropos_setup {
  return 0 unless $main::Opt{'APROPOS'};
  if ( &where_is_prog('apropos') ) {
    $main::Apropos_Prog = 'apropos';
  } elsif ( &where_is_prog('man') ) {
    if (! system("man --apropos man >$main::Dev_Null 2>$main::Dev_Null")) {
      $main::Apropos_Prog = 'man --apropos';
    } elsif (! system("man -k man >$main::Dev_Null 2>$main::Dev_Null")) {
      $main::Apropos_Prog = 'man --k';
    }
  }				# test man
  die 'apropos_setup(): no apropos program available;'
    unless $main::Apropos_Prog;
  &to_tmp_line('.TH GROFFER APROPOS');
  $main::Special_Setup = 1;
  $main::Opt{'TITLE'} = 'apropos' unless $main::Opt{'TITLE'};
  1;
}				# apropos_setup()


##########
# is_man(<man_sec_ext-hash-ref>)
#
# Test whether the argument hash exists as man page (from is_man()).
#
# Globals: in: $main::Man{AUTO_SEC_CHARS},$main::Man{SEC_CHARS},
#              $main::Man{EXT}, $tmpdir
#         out: $main::Manspec
#
# Arguments: 1, a hash refernce with keys 'name', 'sec', 'ext', where
#               'sec' is a string of one or several section characters
#
sub is_man {
  my $n = @_;
  die "is_man(): one argument is needed, you used $n;"
    unless $n == 1;
  die "is_man(): the argument is not a hash reference `$_[0]';"
    if ref($_[0]) ne 'HASH';
  die 'is_man(): temporary directory is not set;' unless $main::tmpdir;
  die 'is_man(): man_setup() must be run first;' unless $main::Man{'IS_SETUP'};

  my $name = $_[0]->{'name'};
  unless ($name) {
    $main::Manspec = '';
    return 0;
  }

  my $sec;
  $sec = $_[0]->{'sec'};
  $sec = $main::Man{'SEC_CHARS'} unless $sec;
  $sec = $main::Man{'AUTO_SEC_CHARS'} unless $sec;
  $sec = '' unless defined $sec;
  # take care, $sec may be several sections

### is_man()
  my $ext;
  $ext = $_[0]->{'ext'};
  $ext = $main::Man{'EXT'} unless $ext;
  $ext = '' unless defined $ext;

  $main::Manspec = "$name.$sec,$ext";
  my @m;
  if (exists $main::Man{'MANSPEC'}{$main::Manspec}) {
    return 1;
  } else {
    foreach my $p ( @{$main::Man{'PATH'}} ) {
      foreach my $s (split //, $sec) {
	my $dir = File::Spec->catdir($p, "man$s");
	next unless -d $dir;
	my $file = File::Spec->catfile($dir, "$name.$s$ext");
	push @m, glob("$file*");
      }
    }
    $main::Man{'MANSPEC'}{$main::Manspec} = \@m;
  }
  return 0 unless (@m);
  return 1;
} # is_man()


##########
# man_get (<man_sec_ext-hash-ref>)
#
# Write a man page to the temporary file.
#
# Globals in: $main::Manspec, $main::Man{MANSPEC}, $main::Man{SEC_CHARS},
#             $main::Man{EXT}, $main::Man{ALL}
#
# Arguments: 1, a hash refernce with keys 'name', 'sec', 'ext', where
#               'sec' is a string of one or several section characters
#
sub man_get {
  my $n = @_;
  die "man_get(): one argument is needed, you used $n;"
    unless $n == 1;
  die "man_get(): the argument is not a hash reference `$_[0]';"
    if ref($_[0]) ne 'HASH';
  die "man_get(): is_man() must be run first on the argument;"
    unless $main::Manspec;
  die "man_get(): wrong hash reference `$_[0]', no 'name' key;"
    unless exists $_[0]->{'name'};

  my ($name, $sec, $ext, $f, $path);
  $name = $_[0]->{'name'};
  die "man_get(): empty 'name' key in the argument;" unless $name;

  $sec = $_[0]->{'sec'};
  $sec = $main::Man{'SEC_CHARS'} if (! $sec) and $main::Man{'SEC_CHARS'};
  $sec = '' unless defined $sec;
  # take care $sec may be several sections

### man_get()
  $ext = $_[0]->{'ext'};
  $ext = $main::Man{'EXT'} unless $ext;
  $ext = '' unless defined $ext;

  die 'man_get(): $main::Manspec does not suit the arguments;'
    if ($main::Manspec ne "$name.$sec,$ext") and
      (! exists $main::Man{'MANSPEC'}{$main::Manspec});

  if ($main::Man{'ALL'}) {
    my $ok;
    my %list;
    foreach ( @{$main::Man{'MANSPEC'}{$main::Manspec}} ) {
      next if exists $list{$_};
      if (-f $_ and -r $_) {
	$list{$_} = 1;
	&to_tmp($_);
	$ok = 1;
      }
    }
    &register_title("man:$name") if $ok;
    return 1;
  }

  # not $main::Man{'ALL'}

  if ($_[0]->{'sec'}) {
    my $path = File::Spec->catfile('', "man$sec", $name);
    if ($ext) {
      foreach $f ( @{$main::Man{'MANSPEC'}{$main::Manspec}} ) {
	if ($f =~ m#$path\.$sec$ext($|\..*$)#) {
	  if (-f $f && -r $f) {
	    &register_file($f);
	    return 1;
	  }
	}			# if $f =~
      }				# foreach $f
### man_get()
      foreach $f ( @{$main::Man{'MANSPEC'}{$main::Manspec}} ) {
	if ($f =~ m#$path\.$sec$ext.*$#) {
	  if (-f $f && -r $f) {
	    &register_file($f);
	    return 1;
	  }
	}			# if $f =~
      }				# foreach $f
    } else {			# $ext is empty
      foreach $f ( @{$main::Man{'MANSPEC'}{$main::Manspec}} ) {
	if ($f =~ m#$path\.$sec($|\..*$)#) {
	  if (-f $f && -r $f) {
	    #	    &to_tmp($f) && &register_file($f);
	    &register_file($f);
	    return 1;
	  }
	}			# if $f =~
      }				# foreach $f
      foreach $f ( @{$main::Man{'MANSPEC'}{$main::Manspec}} ) {
	if ($f =~ m#$path\.$sec.*$#) {
	  if (-f $f && -r $f) {
	    #	    &to_tmp($f) && &register_file($f);
	    &register_file($f);
	    return 1;
	  }
	}			# if $f =~
      }				# foreach $f
    }				# if $ext
### man_get()
  } else {			# sec is empty
    my $m = $main::Man{'SEC_CHARS'};
    $m = $main::Man{'AUTO_SEC_CHARS'} unless $m;
    foreach my $s (split //, $m) {
      my $path = File::Spec->catfile('', "man$s", $name);
      unless ($ext) {
	foreach my $f ( @{$main::Man{'MANSPEC'}{$main::Manspec}} ) {
	  if ( $f =~ m#$path\.$s($|\..*$)# ) {
	    if (-f $f && -r $f) {
	      #	      &to_tmp($f) && &register_file($f);
	      &register_file($f);
	      return 1;
	    }
	  }			# if $f =~
	}			# foreach $f
      }				# unless $ext
      foreach $f ( @{$main::Man{'MANSPEC'}{$main::Manspec}} ) {
	if ($f =~ m#$path\.$s$ext.*$#) {
	  if (-f $f && -r $f) {
	    #	    &to_tmp($f) && &register_file($f);
	    &register_file($f);
	    return 1;
	  }
	}			# if $f =~
      }				# foreach $f
    }				# foreach $s
  }				# if sec

  1;
} # man_get()


##########
# man_setup ()
#
# Setup the variables in %MAN needed for man page searching.
#
# Globals:
#   in:     %OPT, $LANG, $LC_MESSAGES, $LC_ALL,
#           $MANPATH, $MANSEC, $PAGER, $SYSTEM, $MANOPT.
#   out:    $main::Man{PATH}, $main::Man{LANG}, $main::Man{LANG2}, $main::Man{SYS},
#           $main::Man{SEC}, $main::Man{ALL}
#   in/out: $main::Man{ENABLE}
#
# The precedence for the variables related to `man' is that of GNU
# `man', i.e.
#
# $LANG; overridden by
# $LC_MESSAGES; overridden by
# $LC_ALL; this has the same precedence as
# $MANPATH, $MANSEC, $PAGER, $SYSTEM; overridden by
# $MANOPT; overridden by
# the groffer command line options.
#
# $MANROFFSEQ is ignored because grog determines the preprocessors.
#
sub man_setup {
  return 1 if $main::Man{'IS_SETUP'};
  $main::Man{'IS_SETUP'} = 1;
  return 1 unless $main::Man{'ENABLE'};

  # determine basic path for man pages
  my $path;
  if (defined $main::Opt{'MANPATH'}) {
    $path = $main::Opt{'MANPATH'};
  } elsif ($ENV{'MANPATH'}) {
    $path = $ENV{'MANPATH'};
  } elsif ( &where_is_prog('manpath') ) {
    $path = `manpath 2>$main::Dev_Null`;
  }
  if ($path) {
    chomp $path;
    $main::Man{'PATH'} = [split /:/, $path];
  } else {
    $main::Man{'PATH'} = [];
    &manpath_set_from_path();
  }
  unless ( @{$main::Man{'PATH'}} ) {
    $main::Man{'ENABLE'} = 0;
    warn "man_setup(): man path is empty;";
    return 1;
  }

### man_setup()
  # make man path list consisting of unique existing directories
  @{$main::Man{'PATH'}} = &path_uniq( @{$main::Man{'PATH'}} );

  unless ($main::Man{'ALL'}) {
    $main::Man{'ALL'} = $main::Opt{'ALL'} ? 1 : 0;
  }

  # handle man systems
  my $sys = $ENV{'SYSTEM'};
  $sys = $main::Opt{'SYSTEMS'} if $main::Opt{'SYSTEMS'};
  if ($sys) {
    chomp $sys;
    $main::Man{'SYS'} = [split /,/, $sys];
  } else {
    $main::Man{'SYS'} = [];
    $sys = '';
  }

### man_setup()
  # handle language
  my $lang = '';
  $lang = $main::Opt{'LANG'} if $main::Opt{'LANG'};
  unless ($lang) {
    foreach ('LC_ALL', 'LC_MESSAGES', 'LANG') {
      if ($ENV{$_}) {
	$lang = $ENV{$_};
	last;
      }
    }
  }
  if ($lang) {
    if ($lang eq 'C' or $lang eq 'POSIX') {
      $main::Man{'LANG'} = '';
      $main::Man{'LANG2'} = '';
    } elsif ($lang =~ /^.$/) {
      $main::Man{'LANG'} = $lang;
      $main::Man{'LANG2'} = '';
    } elsif ($lang =~ /^..$/) {
      $main::Man{'LANG'} = $lang;
      $main::Man{'LANG2'} = $lang;
    } else {
      $main::Man{'LANG'} = $lang;
      $main::Man{'LANG2'} = $lang;
      $main::Man{'LANG2'} =~ s/^(..).*$/$1/;
    }
  }				# if ($lang)
  # from now on, use only $main::Man{LANG*}, forget about $main::Opt{LANG}, $ENV{LC_*}.

  &manpath_add_lang_sys();

### man_setup()
  # section
  my $sec;
  $sec = $main::Opt{'SECTIONS'} if $main::Opt{'SECTIONS'};
  unless ($sec) {
    $sec = $ENV{'MANSEC'} if $ENV{'MANSEC'};
  }
  $main::Man{'SEC'} = [];
  $main::Man{'SEC_CHARS'} = '';
  if ($sec) {
    foreach (split /:/, $sec) {
      push @{$main::Man{'SEC'}}, $_ if /^[$main::Man{'AUTO_SEC_CHARS'}]$/;
    }
    $main::Man{'SEC_CHARS'} = join '', @{$main::Man{'SEC'}} if @{$main::Man{'SEC'}};
  }				# if ($sec)

  # extension
  my $ext = '';
  $ext = $main::Opt{'EXTENSION'} if $main::Opt{'EXTENSION'};
  unless ($ext) {
    $ext = $ENV{'EXTENSION'} if $ENV{'EXTENSION'};
  }
  $main::Man{'EXT'} = $ext;

  # creation of man temporary is omitted, because of globs in perl
  1;
} # man_setup()


##########
# manpath_add_lang_sys()
#
# Add language and operating system specific directories to man path.
#
# Globals:
#   in:     $main::Man{SYS}: a list of names of operating systems.
#           $main::Man{LANG} and $main::Man{LANG2}: each a single name
#   in/out: @{$main::Man{PATH}}: list of directories which shall have the `man?'
#           subdirectories.
#
sub manpath_add_lang_sys {
  return 1 unless $main::Man{'PATH'};
  return 1 unless @{$main::Man{'PATH'}};

  my @mp;

  if ( @{$main::Man{'SYS'}} ) {
    foreach ( @{$main::Man{'SYS'}} ) {
      if ($_ eq 'man') {
	@mp = (@mp, @{$main::Man{'PATH'}});
      } elsif ($_) {
	my $sys = $_;
	foreach my $p (@{$main::Man{'PATH'}}) {
	  my $dir = File::Spec->catdir($p, $sys);
	  push @mp, $dir;
	}
      }				# if eq 'man'
    }				# foreach SYS
  } else {			# no SYS
    @mp = @{$main::Man{'PATH'}};
  }

  if (@mp && $main::Man{'LANG'}) {
    my @lang_path;
    my $man_lang2 = '';
    $man_lang2 = $main::Man{'LANG2'} if $main::Man{'LANG'} ne $main::Man{'LANG2'};
    foreach my $i ($main::Man{'LANG'}, $man_lang2) {
      next unless $i;
      my $lang = $i;
      foreach my $p (@mp) {
	my $dir = File::Spec->catdir($p, $lang);
	push @lang_path, $dir;
      }				# foreach $p
    }				# foreach $i
    @mp = (@lang_path, @mp);
  }				# if $mp

  $main::Man{PATH} = [&path_uniq(@mp)];
  1;
} # manpath_add_lang_sys()


##########
# manpath_set_from_path()
#
# Determine basic search path for man pages from $PATH.
#
# Return:    `1' if a valid man path was retrieved.
# Output:    none
# Globals:
#   in:  $PATH
#   out: $_MAN_PATH
#
sub manpath_set_from_path {
  my @path =
    qw( /usr/local /usr /usr/X11R6 /usr/openwin /opt /opt/gnome /opt/kde );
  # get a basic man path from $ENV{PATH}
  foreach (@main::Path) {
    s#bin/*$##;
    push @path, $_;
  }
  @path = &path_uniq(@path);
  foreach my $d (@path) {
    foreach my $e ( File::Spec->catdir( qw(share man) ),
		 File::Spec->catdir( qw(share MAN) ),
		 'man',  'MAN' ) {
      my $dir = File::Spec->catdir($d, $e);
      push @{$main::Man{'PATH'}}, $dir if -d $dir;
    }
  }
  1;
} # manpath_set_from_path()


##########
# special_filespec()
#
# Handle special modes like whatis and apropos.  Run their filespec
# functions if suitable.
#
# Globals:  in: $main::Opt{'APROPOS'}, $main::Opt{'WHATIS'}, $main::Special_Setup
#          out: $main::Special_Filespec (internal)
#
sub special_filespec {
  return 0 unless $main::Opt{'APROPOS'} || $main::Opt{'WHATIS'};
  die 'special_filespec(): setup for apropos or whatis must be run first;'
    unless $main::Special_Setup;
  if ( $main::Opt{'APROPOS'} ) {
    die "special_setup(): both apropos and whatis option variables are set;"
      if $main::Opt{'WHATIS'};
    return &apropos_filespec() ? 1 : 0;
  }
  if ( $main::Opt{'WHATIS'} ) {
    return &whatis_filespec() ? 1 : 0;
  }
  0;
} # special_filespec


##########
# special_setup()
#
# Handle special modes like whatis and apropos.  Run their setup
# functions if suitable.
#
sub special_setup {
  if ( $main::Opt{'APROPOS'} ) {
    die "special_setup(): both --apropos and --whatis option variables " .
      "are set;" if $main::Opt{'WHATIS'};
    return &apropos_setup() ? 1 : 0;
  }
  if ( $main::Opt{'WHATIS'} ) {
    return &whatis_setup() ? 1 : 0;
  }
  0;
} # special_setup()


##########
# whatis_filename(<filename>)
#
# Interpret <filename> as a man page and display its `whatis'
# information as a fragment written in the groff language.
#
# Globals:  in: $main::Opt{'WHATIS'}, $main::Special_Setup, $main::Special_Filespec,
#               $main::Filespec_Arg
#
sub whatis_filename {
  my $n = @_;
  die "whatis_filename(): one argument is needed; you used $n;"
    unless $n == 1;
  die "whatis_filename(): whatis is not setup;" unless $main::Opt{'WHATIS'};
  die "whatis_filename(): setup for whatis must be run first;"
    unless $main::Special_Setup;
  die "whatis_filename(): whatis_filespec() must be run first;"
    unless $main::Special_Filespec;
  die 'whatis_filename(): no $main::Filespec_Arg is set;'
    unless defined $main::Filespec_Arg;

  my $arg = $_[0];
  die "whatis_file_name(): $arg is not an existing file" unless -f $arg;
  die "whatis_file_name(): could not read file $arg" unless -r $arg;
  return 1 if -z $arg;
  $arg = 'stdin' if $main::Filespec_Arg eq '-';

  &to_tmp_line('.br', '\\f[CR]' . $arg . '\\f[]:', '.br');

### whatis_filename()
  # get the parts of the file name according to the naming of man pages
  my ($dir, $name) = &split_path( $_[0] );
  my $section = $_[0];
  my $path = File::Spec->catfile('', 'man');
  $section =~ s#^.*$path([$main::Man{'AUTO_SEC_CHARS'}]).*$#$1#;
  $section = '' if $section eq $_[0];
  if ($section) {
    if ($name =~ m#^.*\.$section.*$#) {
      $name =~ s/^(.*)\.$section.*$/$1/;
    } else {
      $section = '';
    }
  }

  # traditional man style; grep the line containing `.TH' macro, if any
  my @catz = &cat_z($_[0]);
  my $res;
  my $test;
  foreach (@catz) {
    if (/^[\.']\s*TH\s/) {		# search .TH, traditional man style
      $test = 1;
      last;
    }
  }				# foreach (@catz)
### whatis_filename()
  if ($test) {			# traditional man style
    # get the first line after the first `.SH' macro before the next `.SH'
    my $test1;
    foreach (@catz) {
      if ($test1) {
	chomp;
	next unless $_;
	next if /^[\.']?\s*$/;
	last if /^[\.']/;		# especially for .SH
	if ($res) {
	  $res = "$res $_";
	} else {
	  $res = $_;
	}
      } else {			# $test1 is empty
	$test1 = 1 if (/^[\.']\s*SH\s/);
      }				# if ($test1)
    }				# foreach (@catz)

    if ($section) {
      if ($res =~ m|^
		    $name
		    \s
		    [^-]*
		    -
		    \s*
		    (.*)
		    $|sx) {
	$res = "$name ($section) \\[em] $1";
      }
    }				# if ($section)
### whatis_filename()
    &to_tmp_line($res, '.br', '', '.br');
    return 1;
  }				# if ($test)

  # mdoc style (BSD doc); grep the line containing `.Nd' macro, if any
  foreach (@catz) {
    if (/^[\.']\s*Nd\s/) {		# BSD doc style
      $res =~ s/^(.*)$/$name ($section) \\[em] $1/;
      &to_tmp_line($res);
      return 1;
    }
  }

  &to_tmp_line('is not a man page', '.br', '', '.br');
  1;
} # whatis_filename()


##########
# whatis_filespec()
#
# Print the filespec name as .SH to the temporary cat file.
#
# Globals: in: $main::Opt{'WHATIS'}, $main::Filespec_Arg
#      in/out: $main::Special_Filespec
#
sub whatis_filespec {
  return 0 unless ($main::Opt{'WHATIS'});
  die 'whatis_filespec(): whatis_setup() must be run first.'
    unless $main::Special_Setup;
  die 'whatis_filespec(): no $main::Filespec_Arg is set;'
    unless defined $main::Filespec_Arg;
  $main::Special_Filespec = 1;
  my $arg = $main::Filespec_Arg;
  $arg =~ s/[^\\]-/\\-/g;
  to_tmp_line(".SH $arg");
} # whatis_filespec()


##########
# whatis_setup ()
#
# Print the whatis header to the temporary cat file; this is the setup
# for whatis.
#
# Globals:  in: $main::Opt{'WHATIS'}
#          out: $main::Special_Setup
sub whatis_setup {
  if ( $main::Opt{'WHATIS'} ) {
    &to_tmp_line('.TH GROFFER WHATIS');
    $main::Special_Setup = 1;
    $main::Opt{'TITLE'} = 'whatis' unless $main::Opt{'TITLE'};
    return 1;
  }
  0;
} # whatis_setup ()

1;
