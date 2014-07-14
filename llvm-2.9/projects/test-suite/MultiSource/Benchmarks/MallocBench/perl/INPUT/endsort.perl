#
# endsort - sort a file with the primary key being the last item on the line
#

while (<>) {
	@entry = split;
	$key = pop(@entry);
	$reentry = join(' ', $key, @entry);
	push(@list, $reentry);
}

@list = sort @list;

foreach $i (0..$#list) {
	@entry = split (' ', $list[$i]);
	$key = shift(@entry);
	print join(' ', @entry), ' ' if ($#entry gt -1);
	print "$key\n" ;
}
