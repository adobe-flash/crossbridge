#
# range.awk -- perform associated ops to create a range
#
#	$1 -- the operation to perform
#	$2 -- the column to operate on
#
BEGIN {
	base = ARGV[1];
	top = ARGV[3];
	if (ARGV[4] == "by") {
		incr = ARGV[5];
	} else {
		incr = 1;
	}

	if (incr > 0) {
		for (i = base; i <= top; i += incr) {
			printf "%d ", i;
		}
	} else {
		for (i = base; i >= top; i += incr) {
			printf "%d ", i;
		}
	}
	printf "\n";
	exit;
}
