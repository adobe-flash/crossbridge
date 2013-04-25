# create shims in sdk/usr/bin_shim for all exes in sdk/usr/bin

`mkdir -p build/shim`;
my $shim = $ENV{PWD} . "/build/shim/shim";
`gcc tools/shim.c -o $shim`;
`mkdir -p sdk/usr/bin_shim`;
my @exes = map { -e $_ ? ($_) : () } <sdk/usr/bin/*>;
for my $exe (@exes)
{
  my $shimmed = $exe;
  $shimmed =~ s/\/bin\//\/bin_shim\//;

  `ln -s $shim $shimmed`;
}
