#!/usr/bin/perl -w
#
# Program:  GenerateReport.pl
#
# Synopsis: Summarize a big log file into a table of values, commonly used for
#           testing.  This can generate either a plaintext table, HTML table,
#           or Latex table, depending on whether the -html or -latex options are
#           specified.
#
#           This script reads a report description file to specify the fields
#           and descriptions for the columns of interest.  In reads the raw log
#           input from stdin and writes the table to stdout.
#
# Syntax:   GenerateReport.pl [-html] [-latex] [-graphs] [-csv] <ReportDesc>
#                    < Input > Output
#

# Default values for arguments
my $HTML = 0;
my $LATEX = 0;
my $GRAPHS = 0;
my $CSV = 0;

# Parse arguments...
while ($_ = $ARGV[0], /^[-+]/) {
  shift;
  last if /^--$/;  # Stop processing arguments on --

  # List command line options here...
  if (/^-html$/)   { $HTML = 1; next; }
  if (/^-latex$/)  { $LATEX = 1; next; }
  if (/^-graphs$/) { $GRAPHS = 1; next; }
  if (/^-csv$/)    { $CSV = 1; next; }

  print "Unknown option: $_ : ignoring!\n";
}

#
# Parameters which may be overriden by the report description file.
#

# The column to sort by, to be overridden as necessary by the report description
my $SortCol = 0;
my $SortReverse = 0;
my $SortNumeric = 0;   # Sort numerically or textually?

# If the report wants us to trim repeated path prefixes off of the start of the
# strings in the first column of the report, we can do that.
my $TrimRepeatedPrefix = 0;
my $TrimAllDirectories = 0;

# Helper functions which may be called by the report description files...
sub SumCols {
  my ($Cols, $Col, $NumRows) = @_;
  $Val = 0;
  while ($NumRows) {
    $Col--; $NumRows--;
    $Val += $Cols->[$Col] if ($Cols->[$Col] ne "*");
  }
  return $Val;
}

sub AddColumns {
  my ($Cols, $Col, @Indices) = @_;
  my $result = 0;

  foreach $Idx (@Indices) {
    if ($Cols->[$Col+$Idx] ne "*") {
      $result += $Cols->[$Col+$Idx];
    }
  }

  return $result;
}

# Check command line arguments...
die "Must specify a report description option" if (scalar(@ARGV) < 1);

# Read file input in one big gulp...
undef $/;

# Read raw data file and split it up into records.  Each benchmarks starts with
# a line with a >>> prefix
#
my @Records = split />>> ========= /, <STDIN>;

# Delete the first "entry" which is really stuff printed prior to starting the
# first test.
shift @Records;

# Read and eval the report description file now.  This defines the Fields array
# and may potentially modify some of our global settings like the sort key.
#
my $ReportFN = $ARGV[0];
#print "Reading report description from $ReportFN\n";
open(REPORTDESC, $ReportFN) or
  die "Couldn't open report description '$ReportFN'!";

# HilightColumns - Filled in by the report if desired in HTML mode.  This
# contains a column number if the HTML version of the output should highlight a
# cell in green/red if it is gt/lt 1.0 by a significant margin.
my %HilightColumns;

my @LatexColumns;  # Filled in by report if it supports Latex mode
my %LatexColumnFormat;  # Filled in by report if supports latex mode
my @Graphs;        # Filled in by the report if supports graph mode

# Fill in all of the fields from the report description
my @Fields = eval <REPORTDESC>;


#
# Read data into the table of values...
#
my @Values;
foreach $Record (@Records) {
  my @RowValues;
  my $Col = 0;
  for $Row (@Fields) {
    my $Val = "*";
    if (scalar(@$Row)) {            # An actual value to read?
      if (ref ($Row->[1])) {        # Code to be executed?
        $Val = &{$Row->[1]}(\@RowValues, $Col);
      } else {                      # Field to be read...
        $Record =~ m/$Row->[1]/;
        if (!defined($1)) {
          $Val = "*";
        } else {
          # If there is a formatting function, run it now...
          $Val = $1;
          if (scalar(@$Row) > 2) {
            $Val = &{$Row->[2]}($Val);
          }
        }
      }
    } else {                        # Just add a seperator...
      $Val = "|";
    }

    push @RowValues, $Val;
    $Col++;
  }

  my $Assert = "";
  if ($Record =~ m/Assertion/) {
    # If an assertion failure occured, print it out.
    $Assert = sprintf "\n\t\t\t%s", (grep /Assertion/, (split "\n", $Record));
  }
  push @RowValues, $Assert if (!$HTML);
  push @Values, [@RowValues];
}


# If the report wants it, we can trim excess cruft off of the beginning of the
# first column (which is often a path).
if ($TrimRepeatedPrefix and scalar(@Values)) {
  OuterLoop: while (1) {
    # Figure out what the first path prefix is:
    $Values[0]->[0] =~ m|^([^/]*/).|;
    last OuterLoop if (!defined($1));

    # Now that we have the prefix, check to see if all of the entries in the
    # table start with this prefix.
    foreach $Row (@Values) {
      last OuterLoop if ((substr $Row->[0], 0, length $1) ne $1);
    }

    # If we get here, then all of the entries have the prefix.  Remove it now.
    foreach $Row (@Values) {
      $Row->[0] = substr $Row->[0], length $1;
    }
  }
}

# If the report wants it, we can trim of all of the directories part of the
# first column.
if ($TrimAllDirectories and scalar(@Values)) {
  foreach $Row (@Values) {
    $Row->[0] =~ s|^.*/||g;
  }
}


#
# Sort table now...
#
if ($SortNumeric) {
  @Values = sort { $lhs = $a->[$SortCol]; $rhs = $b->[$SortCol];
                   $lhs = 0 if ($lhs eq "*");
                   $rhs = 0 if ($rhs eq "*");
                   $lhs <=> $rhs } @Values;
} else {
  @Values = sort { $a->[$SortCol] cmp $b->[$SortCol] } @Values;
}
@Values = reverse @Values if ($SortReverse);

#
# Condense the header into an easier to access array...
#
my @Header;
for $Row (@Fields) {
  if (scalar(@$Row)) {   # Non-empty row?
    push @Header, $Row->[0];
  } else {               # Empty row, just add seperator
    push @Header, "|";
  }
}

if ($HTML) {
  sub printCell {
    my $Str = shift;
    my $ColNo = shift;
    my $IsWhite = shift;
    my $Attrs = "";
    if ($Str eq '|') {
      $Attrs = " bgcolor='black' width='1'";
      $Str = "";
    } else {
      # If  the user requested that we highlight this column, check to see what
      # number it is.  If it is > 1.05, we color it green, < 0.95 we use red.
      # If it's not a number, ignore it.
      if ($HilightColumns{$ColNo}) {
        if ($Str =~ m/^([0-9]+).?[0-9.]*$/) {
          if ($Str <= 0.85) {
            $Attrs = " bgcolor='#FF7070'";
          } elsif ($Str <= 0.95) {
            $Attrs = " bgcolor='#FFAAAA'";
          } elsif ($Str >= 1.15) {
            $Attrs = " bgcolor='#80FF80'";
          } elsif ($Str >= 1.05) {
            $Attrs = " bgcolor='#CCFFCC'";
          }
        }

        if (!$IsWhite && $Attrs eq "") {
          # If it's not already white, make it white now.
          $Attrs = " bgcolor=white";
        }
      }
    };
    print "<td$Attrs>$Str</td>";
    "";
  }

  print "<table border='0' cellspacing='0' cellpadding='0'>\n";
  print "<tr bgcolor=#FFCC99>\n";
  map {
    $_ = "<center><b><a href=\"#$_\">$_</a></b></center>"
      if $_ ne "|";
    printCell($_, -1)
  } @Header;
  print "\n</tr><tr bgcolor='black' height=1>";
  print "</tr>\n";
  my $RowCount = 0;
  foreach $Row (@Values) {
    my $IsWhite;
    $IsWhite = ++$RowCount <= 2;
    print "<tr bgcolor='" . ($IsWhite ? "white" : "#CCCCCC") . "'>\n";
    $RowCount = 0 if ($RowCount > 3);
    my $ColCount = 0;
    map { printCell($_, $ColCount++, $IsWhite); } @$Row;
    print "\n</tr>\n";
  }
  print "\n</table>\n";
} elsif ($GRAPHS) {      # Graph output...
  print "Generating gnuplot data files:\n";
  my $GraphNo = 0;
  foreach $Graph (@Graphs) {
    my @Graph = @$Graph;
    my $Type = shift @Graph;
    die "Only scatter graphs supported right now, not '$Type'!"
      if ($Type ne "scatter");

    my $Filename = shift @Graph;

    print "Writing '$Filename'...\n";
    open (FILE, ">$Filename") or die ("Could not open file '$Filename'!");

    my ($XCol, $YCol) = @Graph;
    foreach $Row (@Values) {
      print FILE $$Row[$XCol] . "\t" . $$Row[$YCol] . "\n";
    }
    close FILE;
    ++$GraphNo;
  }

} else {
  # Add the header for the report to the table after sorting...
  unshift @Values, [@Header];

  #
  # Figure out how wide each field should be...
  #
  my @FieldWidths = (0) x scalar(@Fields);
  foreach $Value (@Values) {
    for ($i = 0; $i < @$Value-1; $i++) {
      if (length($$Value[$i]) > $FieldWidths[$i]) {
        $FieldWidths[$i] = length($$Value[$i])
      }
    }
  }

  if ($LATEX) {
    #
    # Print out the latexified table...
    #
    shift @Values;  # Don't print the header...

    # Make sure the benchmark name field is wide enough for any aliases.
    foreach $Name (@LatexRowMapOrder) {
      $FieldWidths[0] = length $Name if (length($Name) > $FieldWidths[0]);
    }

    # Print out benchmarks listed in the LatexRowMapOrder
    for ($i = 0; $i < @LatexRowMapOrder; $i += 2) {
      my $Name = $LatexRowMapOrder[$i];
      if ($Name eq '-') {
        print "\\hline\n";
      } else {
        # Output benchmark name...
        printf "%-$FieldWidths[0]s", $LatexRowMapOrder[$i+1];

        # Find the row that this benchmark name corresponds to...
        foreach $Row (@Values) {
          if ($Row->[0] eq $Name) {
            for $ColNum (@LatexColumns) {
              # Print a seperator...
              my $Val = $Row->[$ColNum];
              if (exists $LatexColumnFormat{$ColNum}) {
                # If a column format routine has been specified, run it now...
                $Val = &{$LatexColumnFormat{$ColNum}}($Val);
              }

              # Escape illegal latex characters
              $Val =~ s/([%#])/\\$1/g;

              printf " & %-$FieldWidths[$ColNum]s", $Val;
            }
            goto Done;
          }
        }
        print "UNKNOWN Benchmark name: " . $Name;
      Done:
        print "\\\\\n";
      }
    }
  } elsif ($CSV && scalar(@LatexRowMapOrder)) {
    #
    # Print out the table as csv in the row-order specified by LatexRowMapOrder
    #
    for ($i = 0; $i < @LatexRowMapOrder; $i += 2) {
      my $Name = $LatexRowMapOrder[$i];
      if ($Name eq '-') {
        print "----\n";
      } else {
        # Output benchmark name.
        printf "$LatexRowMapOrder[$i+1]";

        # Find the row that this benchmark name corresponds to.
        foreach $Row (@Values) {
          if ($Row->[0] eq $Name) {
            for ($j = 1; $j < @$Row-1; $j++) {
              print ",$$Row[$j]";
            }
            goto Done;
          }
        }
        print "UNKNOWN Benchmark name: " . $Name;
      Done:
        print "\\\\\n";
      }
    }

  } elsif ($CSV) {
    #
    # Print out the table as csv
    #
    my $firstrow = 1;
    foreach $Value (@Values) {
      printf "$$Value[0]";
      for ($i = 1; $i < @$Value-1; $i++) {
        print ",$$Value[$i]" if ($$Value[$i] ne "|");
      }
      if ($firstrow) {
        # Print an extra column for the header.
        print ",$$Value[@$Value-1]";
        $firstrow = 0;
      }
      print "\n";
    }
  } else {
    #
    # Print out the table in plaintext format now...
    #
    foreach $Value (@Values) {
      for ($i = 0; $i < @$Value-1; $i++) {
        printf "%-$FieldWidths[$i]s ", $$Value[$i];
      }
      
      # Print the assertion message if existant...
      print "$$Value[@$Value-1]\n";
    }
  }
}
