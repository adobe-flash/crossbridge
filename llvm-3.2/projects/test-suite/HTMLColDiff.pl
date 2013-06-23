#!/usr/bin/perl -w

use strict;

# htmlcoldiff - Diff two files (specified on the command line) laid out in
# columns.  Output a copy of the second file with differences from the first
# hi-lighted in bold (with HTML tags).  This assumes that the first column of
# the file is an identifier that keys two rows of the files together.
#

# Don't show changes smaller than 1% by default
my $showThreshold = 1;
# Highlight changes larger than 5% by default
my $highlightThreshold = 5;
# Don't highlight changes that have a smaller absolute difference than this
my $highlightAbsDiff = .05;
# Don't show rows that have no changes shown
my $excludeNoShow = 0;
# Don't show rows that have no changes highlighted.
my $excludeNoHighlight = 0;

my $Input1 = 0;
my $Input2 = 0;

my $usage = "You should specify two filenames\n\n";
$usage .= "Usage: ./HTMLColDiff.pl [-t showThreshold] [-T highlightThreshold] [-A absDiff] [-x] [-X] report.nightly.txt.a report.nightly.txt.b\n\n";
$usage .= "Where showThreshold and highlightThreshold are the amount of change needed to a value to be shown or highlighted resp. (in percent) and absDiff is the minimum amount of absolute difference for a highlight. ";
$usage .= "When -x is specified, any rows that have no changes shown (per the -t option) are excluded. When -X is specified, any rows that have no highlights (per the -T options) are excluded.\n\n";
$usage .= "The default options are -t $showThreshold -T $highlightThreshold -A $highlightAbsDiff\n";

while (@ARGV) {
  if ($ARGV[0] eq "-t") {
    shift;
    $showThreshold = int($ARGV[0]);
  } elsif ($ARGV[0] eq "-T") {
    shift;
    $highlightThreshold = int($ARGV[0]);
  } elsif ($ARGV[0] eq "-A") {
    shift;
    $highlightAbsDiff = $ARGV[0];
  } elsif ($ARGV[0] eq "-x") {
    $excludeNoShow = 1;
  } elsif ($ARGV[0] eq "-X") {
    $excludeNoHighlight = 1;
  } else {
    if (!$Input1) {
      $Input1 = $ARGV[0];
    } elsif (!$Input2) {
      $Input2 = $ARGV[0];
    } else {
      warn("Extra filename found: " . $ARGV[0]); 
    }
  }
  shift;
}

if (!$Input1 || !$Input2) {
  warn($usage);
  exit 1;
}

open(INPUT1, $Input1) or die "Couldn't open '$Input1'!";
open(INPUT2, $Input2) or die "Couldn't open '$Input2'!";

# Read all of the lines of both files into arrays:
my @File1 = <INPUT1>;
my @File2 = <INPUT2>;

# Convert the first file from an array of lines to a hash from the key to the
# whole line.
my %File1Map;
foreach my $Line (@File1) {
  my $Key = (split /[\s|]+/, $Line, 2)[0];
  $File1Map{$Key} = $Line;
  #print "Map '$Key' => '$Line'\n";
}

print("<p>Showing differences between $Input1 and $Input2</p>");
print("<p>");
if ($showThreshold == 0) {
  print "Showing all changes. ";
} else {
  print "Hiding changes smaller than $showThreshold%. ";
}
print "Highlighting changes larger than $highlightThreshold%.";
print "</p>\n";
if ($excludeNoHighlight) {
  print ("<p>Excluding rows that have no highlighted cells</p>\n");
} elsif ($excludeNoShow) {
  print ("<p>Excluding rows that have no significant difference</p>\n");
}

print "<table border='0' cellspacing='0' cellpadding='0'>\n";

# Loop over the second file, outputting it, surrounding field with <B> tags if
# they differ from the first file.

my $LineNo = -1;
foreach my $Line (@File2) {
  # Try to remove spaces from the headers. This is a bit of a hack, really...
  $Line =~ s/([^ ]) (codegen|compile)/$1\&nbsp;$2/g;

  my @Fields = split /([\s|]+)/, $Line;  # Keep the seperators as well...
  my $Key = $Fields[0];
  my $CompareLine = $File1Map{$Key};

  # Try to remove spaces from the headers. This is a bit of a hack, really...
  $CompareLine =~ s/([^ ]) (codegen|compile)/$1&nbsp;$2/g;

  my @FieldsCompare = split /[\s|]+/, $CompareLine;  # Do not keep seperators

  # Loop over all of the Fields, outputting them with an optional tag...
  my $FieldNo = 0;

  # Give the header row a different colour and then alternate betwen
  # grey and white every two rows.
  my $RowColour;
  if ($LineNo == -1) {
    $RowColour = "#FFCC99";
  } elsif (($LineNo / 2) % 2) {
    $RowColour = "#CCCCCC";
  } else {
    $RowColour = "#FFFFFF";
  }
  my $row = "<tr bgcolor=\"$RowColour\"><td>";
  my $showCells = 0;
  my $highlightCells = 0;
  foreach my $Field (@Fields) {
    if ($Field =~ /[\s|]+/) {    # If it's a seperator, just output it
      #print $Field;
      $row .= "</td><td>";
    } else {                  # If it's not a seperator, diff it...
      my $Show      = 0;
      my $Highlight = 0;
      my $Colour    = 0;

      if (@FieldsCompare > $FieldNo) {
        my $FieldComp = $FieldsCompare[$FieldNo];
        if ($Field =~ /^[0-9.]+$/ && $FieldComp =~ /^[0-9.]+$/ &&
            $FieldComp != 0 && $Field ne $FieldComp) {              # Handle numbers special

          # How much did the field change?
          my $Change = $Field/$FieldComp;

          # Check to see if the change is really small, don't show it
          # then.
          $Show = 1 if ($Change > 1 + ($showThreshold / 100)
                        || $Change < 1 - ($showThreshold / 100));

          # Highlight the change if it is large enough, both relatively
          # and absolutely
          $Highlight = 1 if ($Change > 1 + ($highlightThreshold / 100)
                           || $Change < 1 - ($highlightThreshold / 100));
          $Highlight = 0 if (abs($Field - $FieldComp) < $highlightAbsDiff);
          
          # Show and colour changes if the change is large enough
          if ($Show) {
            my $Percent = sprintf("%+.2f%%", ($Change-1)*100);
            $Field = "$FieldComp => $Field ($Percent)";
            $Colour = ($Change > 1 ? "red" : "green");
          }
          
        } elsif ($Field ne $FieldComp) {
          $Show    = 1;
          # Specially handle tests that are failing to compile but were not
          # doing so previously
          if ($Field eq "*") {
            $Highlight = 1;
            $Colour = "red";
            $Field = "FAIL";
          }
          # Always show but never highlight other changes in non-numeric
          # fields
          $Field = "$FieldComp => $Field";
        }
      }
      # Remove spaces, we don't want wrapping inside cells
      $Field =~ s/ /\&nbsp;/g;
      $row .= "<b><u>" if $Highlight;
      $row .= "<font color=\"$Colour\">" if $Colour;
      $row .= $Field;
      $row .= "</font>" if $Colour;
      $row .= "</u></b>" if $Highlight;
    

      # Keep track that at least one cell was shown/highlighted in this
      # row.
      $highlightCells = 1 if ($Highlight);
      $showCells = 1 if ($Show);

      $FieldNo++;
    }
  }
  $row .= "</td></tr>\n";
  # Always show the first row, but exclude rows that don't have any
  # changes or highlights if those options are enabled.
  if ($LineNo == -1 || (!$excludeNoHighlight || $highlightCells) && (!$excludeNoShow || $showCells)) {
    print $row;
    $LineNo++;
  }
}
print "</table>\n";
