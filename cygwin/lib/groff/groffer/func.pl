#! /usr/bin/env perl

# groffer - display groff files

# Source file position: <groff-source>/contrib/groffer/perl/func.pl
# Installed position: <prefix>/lib/groff/groffer/func.pl

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
# some functions
########################################################################

##########
# cat_z(<file>)
#
# Decompress or cat the <file>.
#
# Return: the decompressed file as array
#
sub cat_z {
  my $n = @_;
  die "cat_z(): one argument is needed; you used $n;"
    unless $n == 1;

  my $file = $_[0];
  die "cat_z(): `$file' is not a readable file;" unless -f $file && -r $file;
  return () if -z $file;

  my @res;
  if ($main::Has_Compression) {
    if ($main::Has_bzip) {
      # test whether bz2 compressed, shell return must be inverted
      unless ( system("bzip2 -t $file 2>$main::Dev_Null") ) {
	@res = `bzip2 -c -d $file 2>$main::Dev_Null`;
	return @res;
      }
      # if not compressed with gz, gzip will act like `cat'
      @res = `gzip -c -d -f $file 2>$main::Dev_Null`;
      return @res;
    }
  } else {
    my $fh;
    open $fh, "<$file" or die "cat_z(): could not open $file";
    @res = <$fh>;
    close $fh;
    return @res;
  }
  ();
} # cat_z()


##########
# clean_up()
#
# Remove the temporary directory and restore the system.
#
sub clean_up {
  umask $main::Umask;
  chdir $main::Start_Dir;
  if ($main::Debug{'KEEP'} && -d $main::tmpdir) {
    my $glob = File::Spec->catfile($main::tmpdir, '*');
    unlink glob($glob);		# rm $tmpdir/*
    rmdir $main::tmpdir;
  }
} # clean_up()


##########
# get_dirname(<path>)
#
# Split the path and return the directory name part
#
# Return: string of directory name
#
sub get_dirname {
  my $n = @_;
  die "get_filename(): one argument is needed; you used $n;" unless $n == 1;
  return '' unless $_[0];

  my ($dirname, $filename) = &split_path($_[0]);
  $dirname;
}				# get_dirname()


##########
# get_filename(<path>)
#
# Split the path and return the file name part
#
# Return: string of file name
#
sub get_filename {
  my $n = @_;
  die "get_dirname(): one argument is needed; you used $n;" unless $n == 1;
  return '' unless $_[0];

  my ($dirname, $filename) = &split_path($_[0]);
  $filename;
}				# get_filename()


##########
# is_X()
#
# Test whether X Windows is running.
#
sub is_X {
  return 1 if $ENV{'DISPLAY'};
  return 0;
}				# is_X()


##########
# list_has(<list_ref>, <string>)
#
# Determine if <list_ref> has <string> as element.
#
sub list_has {
  my $n = @_;
  die "list_has(): 2 arguments are needed; you used $n;"
    unless $n == 2;

  my $list_ref = $_[0];
  my $string = $_[1];
  die "list_has(): first argument must be an array reference;"
    unless ref($list_ref) eq 'ARRAY';

  foreach ( @$list_ref ) {
    return 1 if $_ eq $string;
  }
  0;
}


##########
# path_uniq(<dir>...)
#
# make path having unique existing directories
#
sub path_uniq {
  my @a;
  my %h;
  foreach (@_) {
    next if exists $h{$_};
    next unless -d $_;
    push @a, $_;
    $h{$_} = 1;
  }
  @a;
}				# path_uniq()


##########
# print_hash(<hash_or_ref>)
#
# print the elements of a hash or hash reference
#
sub print_hash {
  my $hr;
  my $n = scalar @_;
  if ($n == 0) {
    print "empty hash\n;";
    return 1;
  } elsif ($n == 1) {
    if (ref($_[0]) eq 'HASH') {
      $hr = $_[0];
    } else {
      warn 'print_hash(): the argument is not a hash or hash reference;';
      return 0;
    }
  } else {
    if ($n % 2) {
      warn 'print_hash(): the arguments are not a hash;';
      return 0;
    } else {
      my %h = @_;
      $hr = \%h;
    }
  }

### print_hash()
  unless (%$hr) {
    print "empty hash\n";
    return 1;
  }
  print "hash (ignore the ^ characters):\n";
  for my $k (sort keys %$hr) {
    my $hk = $hr->{$k};
    print "  $k => ";
    if (defined $hk) {
      print "^$hk^";
    } else {
      print "undef";
    }
    print "\n";
  }

  1;
}				# print_hash()


##########
# print_times(<string>)
#
# print the time, result: user, system, child process user, child system
#
sub print_times {
  my @t = times;
  my $s = '';
  $s = $_[0] if $_[0];
#  print STDERR "$s: @t\n";
}				# print_times()

BEGIN { &print_times("start"); }
END { &print_times("end"); }


##########
# split_path(<path>)
#
# Split the path into directory and file name parts
#
# Return: array with 2 elements consisting of directory and file name
#
sub split_path {
  my $n = @_;
  die "split_path(): one argument is needed; you used $n;" unless $n == 1;
  my $arg = $_[0];
  return () unless $arg;

  my $basename = basename($arg);
  if ( $basename ne $arg ) { # path with dir
    # fileparse() is from File::Basename
    my($filename, $dirname, $suffix) = fileparse($arg);
    return ($dirname, $basename);
  }
  return ('', $arg);
}


{
  my $nr_file = 0;
  my $nr_so = 0;

  my $tmp_file_base;
  my $tmp_so_base;

  my $soelim_r = '';
  $soelim_r = '-r'
    if ! system("echo -n '' | soelim -r 2>$main::Dev_Null >$main::Dev_Null");

  ##########
  # to_tmp (<filename>)
  #
  # Print file (decompressed) to the temporary cat file with handling .SO
  # for man pages.
  #
  sub to_tmp {
    my $n = @_;
    die "to_tmp(): one argument is needed; you used $n;"
      unless $n == 1;

    my $arg = $_[0];
    return 1 unless $arg;
    die "to_tmp(): $arg is not an existing file;" unless -f $arg;
    die "to_tmp(): could not read file $arg;" unless -r $arg;
    return 1 if -z $arg;

    $tmp_file_base = File::Spec->catfile($main::tmpdir, ',file')
      unless $tmp_file_base;
    $tmp_so_base = File::Spec->catfile($main::tmpdir, ',so')
      unless $tmp_so_base;

    open $main::fh_cat, ">>$main::tmp_cat" or
      die "to_tmp(): could not open temporary cat file";

    if ($main::Opt{'WHATIS'}) {
      &whatis_filename($arg);
      return 1;
    }

    ### to_tmp()
    my $dir = &get_dirname($arg);

    my ($fh_file, $tmp_file, $fh_tmp, $tmp_tmp);
    ++$nr_file;
    $tmp_file = $tmp_file_base . $nr_file;
    $tmp_tmp = File::Spec->catfile($main::tmpdir, ',tmp');

    print STDERR "file: $arg\n" if $main::Debug{'FILENAMES'};

    if ($main::Filespec_Is_Man) {
      my ($fh_so, $tmp_so);

      open $fh_file, ">$tmp_file" or
	die "to_tmp(): could not open $tmp_file;";
      foreach ( &cat_z($arg) ) {
	print $fh_file "$_";
      }
      close $fh_file;

      open $fh_file, "<$tmp_file" or
	die "to_tmp(): could not open $tmp_file;";
      my @list;
      foreach (<$fh_file>) {
	if (/^[\.']\s*so\s/) {
	  chomp;
	  s/^[\.']\s*so\s*//;
	  push @list, $_;
	}
      }
      close $fh_file;

      if ( @list && $main::Debug{'KEEP'} ) {
	my $f = $tmp_file . '+man';
	copy($tmp_file, $f);
      }

      ### to_tmp()
    DO_MAN_SO:  foreach (@list) {
	# start of _do_man_so() in shell version
	my $so = $_;
	my $soname = $so;
	$soname =~ s/\\\s/ /g;

	my $sofound;
	my $path = File::Spec->rootdir();
	if ($soname =~ m#^$path#) {	  # absolute path name
	  next DO_MAN_SO if -f $soname;
	  foreach ('.gz', '.Z', '.bz2') {
	    my $name = $soname . $_;
	    if (-f $name) {
	      $sofound = $name;
	      last;
	    }
	  }			# foreach
	  next DO_MAN_SO unless $sofound;
	} else {		# relative to man path
	LOOP: foreach my $ext ('', '.gz', '.Z', '.bz2') {
	    foreach my $p ( @{$main::Man{'PATH'}} ) {
	      my $f = File::Spec->catfile($p, "$soname$ext");
	      if (-f $f) {
		$sofound = $f if -f $f;
		last LOOP;
	      }
	    }			# foreach
	  }			# LOOP:
	  next DO_MAN_SO unless $sofound;
	}			# if on path

	print STDERR "file from .so: $so\n" if $main::Debug{'FILENAMES'};

	### to_tmp()
	++$nr_so;
	$tmp_so = $tmp_so_base . $nr_so;
	unlink $tmp_so if -e $tmp_so;
	open $fh_so, ">$tmp_so" or
	  die "to_tmp(): could not open $tmp_so;";
	foreach ( &cat_z($sofound) ) {
	  print $fh_so $_;
	}
	close $fh_so;

	my $esc = $so;
	$esc =~ s/\\/\\\\/g;
	open $fh_file, "<$tmp_file" or
	  die "to_tmp(): could not open $tmp_file;";
	open $fh_tmp, ">$tmp_tmp" or
	  die "to_tmp(): could not open $tmp_tmp;";
	foreach (<$fh_file>) {
	  s#^([\.'])\s*so\s+($so|$esc|$soname)\s*\n$#${1}so $tmp_so\n#s;
	  print $fh_tmp $_;
	}
	### to_tmp()
	close $fh_tmp;
	close $fh_file;
	unlink $tmp_file if -e $tmp_file;
	rename $tmp_tmp, $tmp_file;
	# end of _do_man_so() in shell version
      }				# foreach (@list)

      if ( @list && $main::Debug{'KEEP'} ) {
	my $f = $tmp_file . '+tmp';
	copy($tmp_file, $f);
      }

      unlink $tmp_tmp if -e $tmp_tmp;
      rename $tmp_file, $tmp_tmp;
      system("soelim -I$dir $soelim_r $tmp_tmp >$tmp_file");
      unlink $tmp_tmp if -e $tmp_tmp;

    } else {			# $Filespec_Is_Man is empty
      open $fh_tmp, ">$tmp_tmp" or
	die "to_tmp(): could not open $tmp_tmp;";
      foreach (cat_z $arg) {
	print $fh_tmp $_;
      }
      close $fh_tmp;
      if ($dir) {
	system("soelim -I$dir $soelim_r $tmp_tmp >$tmp_file");
      } else {
	system("soelim $soelim_r $tmp_tmp >$tmp_file");
      }
      unlink $tmp_tmp;
    }				# if ($Filespec_Is_Man)

    ### to_tmp()
    my $grog = `grog $tmp_file`;
    die "to_tmp(): grog error on $tmp_file;" if $?;
    chomp $grog;
    print STDERR "grog output: $grog\n" if $main::Debug{'GROG'};
    if ($grog =~ /^.*\s-m.*$/) {
      $grog =~ s/\s+/ /g;
      $grog =~ s/ -m / -m/g;
      $grog =~ s/ -mm([^ ]) / -m$1/g;
      foreach my $g (split / /, $grog) {
	if ($g =~ /^-m/) {
	  my $ref = \@main::Macro_Packages;
	  if ( &list_has($ref, $g) ) {
	    if (! $main::Macro_Pkg) {
	      $main::Macro_Pkg = $g;
	    } elsif ($main::Macro_Pkg eq $g) {
	      1;
	    } elsif ($main::Macro_Pkg =~ /^-m/) {
	      warn "to_tmp(): Ignore $arg because it needs $g " .
		"instead of $main::Macro_Pkg";
	      unlink $tmp_file unless $main::Debug{'KEEP'};
	      return 0;
	    } elsif ($main::Macro_Pkg ne $g) {
	      die "to_tmp(): \$Macro_Pkg does not start with -m: " .
		"$main::Macro_Pkg";
	    }			# if (! $main::Macro_Pkg)
	  }			# if (&list_has
	}			# if (/^-m/)
      }				# foreach my $g
    }				# if $grog

    open $fh_file, "<$tmp_file" or
      die "to_tmp(): could not open $tmp_file for reading;";
    open $main::fh_cat, ">>$main::tmp_cat" or
      die "to_tmp(): could not open $main::tmp_cat for appending;";
    foreach (<$fh_file>) {
      print $main::fh_cat $_;
    }
    close $main::fh_cat;
    close $fh_file;

    unless ( $main::Debug{'KEEP'} ) {
      unlink $tmp_file;
      foreach ( glob("$tmp_so_base*") ) {
	unlink $_;
      }
    }
    1;
  }				# to_tmp()
}

##########
# to_tmp_line (<text>...)
#
# Print array of lines with <text> to the temporary cat file.  \n is added
# if a line does not end with \n.
#
sub to_tmp_line {
  my $n = @_;
  return 1 if $n == 0;
  open $main::fh_cat, ">>$main::tmp_cat" or
    die "to_tmp_line(): could not open temporary cat file";
  foreach (@_) {
    my $line = $_;
    chomp($line);
    print $main::fh_cat "$line\n";
  }
  close $main::fh_cat;
  1;
} # to_tmp_line()


##########
# usage()
#
# Print usage information for --help.
#
sub usage {
  print "\n";
  &version();
  print <<EOF;

Usage: groffer [option]... [filespec]...

Display roff files, standard input, and/or Unix manual pages with an X
Window viewer or in several text modes.  All input is decompressed
on-the-fly with all formats that gzip can handle.

"filespec" is one of
  "filename"       name of a readable file
  "-"              for standard input
  "man:name(n)"    man page "name" in section "n"
  "man:name.n"     man page "name" in section "n"
  "man:name"       man page "name" in first section found
  "name(n)"        man page "name" in section "n"
  "name.n"         man page "name" in section "n"
  "n name"         man page "name" in section "n"
  "name"           man page "name" in first section found
where `section' is a single character out of [1-9on], optionally followed
by some more letters that are called the `extension'.

-h --help         print this usage message.
-T --device=name  pass to groff using output device "name".
-v --version      print version information.
-V                display the groff execution pipe instead of formatting.
-X                display with "gxditview" using groff -X.
-Z --ditroff --intermediate-output
                  generate groff intermediate output without
                  post-processing and viewing, like groff -Z.
All other short options are interpreted as "groff" formatting options.

The most important groffer long options are

--apropos=name    start man's "apropos" program for "name".
--apropos-data=name
                  "apropos" for "name" in man's data sections 4, 5, 7.
--apropos-devel=name
                  "apropos" for "name" in development sections 2, 3, 9.
--apropos-progs=name
                  "apropos" for "name" in man's program sections 1, 6, 8.
--auto            choose mode automatically from the default mode list.
--default         reset all options to the default value.
--default-modes=mode1,mode2,...
                  set sequence of automatically tried modes.
--dvi             display in a viewer for TeX device independent format.
--dvi-viewer=prog choose the viewer program for dvi mode.
--groff           process like groff, disable viewing features.
--help            display this helping output.
--html            display in a web browser.
--html-viewer=program
                  choose the web browser for html mode.
--man             check file parameters first whether they are man pages.
--mode=auto|dvi|groff|html|pdf|ps|source|text|tty|www|x|X
                  choose display mode.
--no-man          disable man-page facility.
--no-special      disable --all, --apropos*, and --whatis
--pager=program   preset the paging program for tty mode.
--pdf             display in a PDF viewer.
--pdf-viewer=prog choose the viewer program for pdf mode.
--ps              display in a Postscript viewer.
--ps-viewer=prog  choose the viewer program for ps mode.
--source          output as roff source.
--text            output in a text device without a pager.
--to-stdout       output the content of the mode file without display.
--tty             display with a pager on text terminal even when in X.
--tty-viewer=prog select a pager for tty mode; same as --pager.
--whatis          display the file name and description of man pages
--www             same as --html.
--www-viewer=prog same as --html-viewer
--x --X           display with "gxditview" using an X* device.
--x-viewer=prog   choose viewer program for x mode (X mode).
--X-viewer=prog   same as "--xviewer".

The usual X Windows toolkit options transformed into GNU long options:
--background=color, --bd=size, --bg=color, --bordercolor=color,
--borderwidth=size, --bw=size, --display=Xdisplay, --fg=color,
--fn=font, --font=font, --foreground=color, --geometry=geom, --iconic,
--resolution=dpi, --rv, --title=text, --xrm=resource

Long options of GNU "man":
--all, --ascii, --ditroff, --extension=suffix, --locale=language,
--local-file=name, --location, --manpath=dir1:dir2:...,
--sections=s1:s2:..., --systems=s1,s2,..., --where, ...

Development options that are not useful for normal usage:
--debug, --debug-filenames, --debug-grog, --debug-keep, --debug-params,
--debug-tmpdir, --do-nothing, --print=text

EOF
} # usage()


##########
# version()
#
# Get version information from version.sh and print a text with this.
#
sub version {
  my $groff_version;
  my $program_version = '';
  my $last_update = '';
  my $groff_version_preset = '';

  die "$main::File_version_sh does not exist;"
    unless -f "$main::File_version_sh";
  my $fh;
  open $fh, "<$main::File_version_sh";
  foreach (<$fh>) {
    chomp;
    if (/^\s*_PROGRAM_VERSION\s*=\s*['"]*([^'"]*)['"]*\s*;?\s*$/) {
      $program_version = $1;
      next;
    }
    if (/^\s*_LAST_UPDATE\s*=\s*['"]*([^'"]*)['"]*\s*;?\s*$/) {
      $last_update = $1;
      next;
    }
    if (/^\s*_GROFF_VERSION_PRESET\s*=\s*['"]*([^'"]*)['"]*\s*;?\s*$/) {
      # this setting of the groff version is only used before make is run,
      # otherwise @VERSION@ will set it, see groffer.sh.
      $groff_version_preset = $1;
      next;
    }
  }
  close $fh;

  if ($main::Groff_Version) {
    $groff_version = $main::Groff_Version;
  } else {
    $groff_version = $groff_version_preset;
  }
  my $year = $last_update;
  $year =~ s/^.* //;
  print <<EOF;
groffer $program_version of $last_update (Perl version)
is part of groff version $groff_version.
Copyright (C) $year Free Software Foundation, Inc.
GNU groff and groffer come with ABSOLUTELY NO WARRANTY.
You may redistribute copies of groff and its subprograms
under the terms of the GNU General Public License.
EOF
} # version()


##########
# where_is_prog(<program>)
#
# Test whether <program> without its arguments exists or is a program
# in $PATH.
#
# Arguments : 1, <program> can have spaces and arguments.
# Return    : a hash with `dir', `file', `fullname', `args' if
#             argument exists or is a program in $PATH, empty hash else.
#
sub where_is_prog {
  scalar @_ eq 1 or die "where_is_prog(): only one argument is allowed";
  my $p1 = $_[0];
  return () unless $p1;

  $p1 =~ s/\s+/ /g;
  $p1 =~ s/(\\)+ / /g;
  $p1 =~ s/^ | $//g;
  return () unless $p1;

  my $noarg = $p1;
  $noarg =~ s/ -.*$//;
  return () unless $noarg;

  my $args;
  if ($p1 =~ /^.* -.*$/) {
    $args = $p1;
    $args =~ s#^$noarg ##;
  }
  $args = '' unless defined $args;

  my %result;

  # test whether $noarg has directory, so it is not tested with $PATH
  my ($dir, $name) = &split_path($noarg);
  $result{'dir'} = $dir;

### where_is_prog()
  if (-f $noarg && -x $noarg) {
    $result{'args'} = $args;
    $result{'file'} = $name;
    $result{'fullname'} = File::Spec->catfile($dir, $name);
    return %result;
  }

  if ($dir) {			# $noarg has name with directory
    # now $wip_noarg (with directory) is not an executable file

    # test name with space
    if ($name =~ / /) {
      my @base = split(/ /, $name);
      my $n = pop @base;
      my @baseargs = ($n);
      while (@base) {
	my $base = join(' ', @base);
	my $file = File::Spec->catpath($dir, $base);
	if (-f $file && -x $file) {
	  my $baseargs = join(' ', @baseargs);
	  ### where_is_prog()
	  if ($args) {
	    $args = "$baseargs $args";
	  } else {
	    $args = $baseargs;
	  }
	  $result{'args'} = $args;
	  $result{'file'} = $base;
	  $result{'fullname'} = $file;
	  return %result;
	}			# file test $file
	$n = pop @base;
	unshift(@baseargs, $n);
      }				# while (@base)
    }				# end of test name with space
    return ();
  }				# test on $dir

  # now $noarg doesn't have a directory part

  $name = $noarg;

  # check with $PATH

  # test path with $win_noarg as a whole, evt. with spaces
  foreach my $d (@main::Path) {
    my $file = File::Spec->catfile($d, $name);
    if (-f $file && -x $file) {
      $result{'args'} = $args;
      $result{'fullname'} = $file;
      ($result{'dir'}, $result{'file'}) = &split_path($file);
      return %result;
    }				# file test $file
  }				# foreach (@main::Path)

### where_is_prog()
  if ($name =~ / /) {
    my @base = split(/ /, $name);
    my $n = pop @base;
    my @baseargs = ($n);
    while (@base) {
      my $base = join(' ', @base);
      foreach my $d (@maon::Path) {
	my $file = File::Spec->catfile($d, $base);
	if (-f $file && -x $file) {
	  my $baseargs = join(' ', @baseargs);
	  if ($args) {
	    $args = "$baseargs $args";
	  } else {
	    $args = $baseargs;
	  }
	  $result{'args'} = $args;
	  $result{'fullname'} = $file;
	  ($result{'dir'}, $result{'file'}) = &split_path($file);
	  return %result;
	}			# file test $file
      }				# foreach (@main::Path)
      $n = pop @base;
      unshift(@baseargs, $n);
    }				# while (@base)
  }				# test $name on space
  return ();
} # where_is_prog()


##########
# wait()
#
# stop for checking temp files, etc.
#
sub wait {
  print "push Ctrl-D";
  my @x = <STDIN>;
  print "\n";
}				# wait()

1;
