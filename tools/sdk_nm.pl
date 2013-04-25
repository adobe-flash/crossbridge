# does a global nm of the sdk -- prints out ALL sym types found for each symbol
use strict;

my @nm = split("\n", `2>/dev/null find sdk -type f -exec sdk/usr/bin/nm \\{\\} \\;`);
my %syms;

for my $entry (@nm)
{
      #00000000 T _SDL_FreeYUV_SW
  if($entry =~ /^........ ([a-zA-Z]) (.*)/)
    { $syms{$2} .= $1 }
}

for my $sym (keys(%syms))
{
  my %types = map { ($_, 1) } split("", $syms{$sym});

  print join("", sort(keys(%types))) . "\t$sym\n";
}

