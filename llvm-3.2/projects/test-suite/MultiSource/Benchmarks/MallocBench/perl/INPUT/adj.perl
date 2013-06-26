#!/usr/local/bin/perl
eval "exec /usr/local/bin/perl -S $0 $*"
    if $running_under_some_shell;
			# this emulates #! processing on NIH machines.
			# (remove #! line above if indigestible)

eval '$'.$1.'$2;' while $ARGV[0] =~ /^([A-Za-z_]+=)(.*)/ && shift;
			# process any FOO=bar switches

# adj.nawk -- adjust lines of text per options
#
# NOTE:  this nawk program is called from the shell script "adj"
#    see that script for usage & calling conventions
#
# author:
#    Norman Joseph (amanue!oglvee!norm)

$[ = 1;			# set array base to 1
$FS = ' ';		# set field separator

$FS = "\n";
$blankline = "^[ \t]*\\$";
$startblank = "^[ \t]+[^ \t]+";
$startwords = "^[^ \t]+";

while (<>) {
    chop;	# strip record separator
    if ($_ =~ $blankline) {
	if ($type eq 'b') {
	    &putline($outline . "\n");
	}
	else {
	    &putline(&adjust($outline, $type) . "\n");
	}
	&putline("\n");
	$outline = '';
    }

    if ($_ =~ $startblank) {
	if ($outline ne '') {
	    if ($type eq 'b') {
		&putline($outline . "\n");
	    }
	    else {
		&putline(&adjust($outline, $type) . "\n");
	    }
	}

	$firstword = '';
	$i = 1;
	while (substr($_, $i, 1) =~ "[ \t]") {
	    $firstword = $firstword . substr($_, $i, 1);
	    $i++;
	}
	$inline = substr($_, $i, 999999);
	$outline = $firstword;

	$nf = (@word = split("[ \t]+", $inline, 9999));

	for ($i = 1; $i <= $nf; $i++) {
	    if ($i == 1) {
		$testlen = length($outline . $word[$i]);
	    }
	    else {
		$testlen = length($outline . ' ' . $word[$i]);
		if ('.!?:;' =~ ("\\" . substr($outline, length($outline),
		  1)) && ($RLENGTH = length($&), $RSTART = length($`)+1)) {
		    $testlen++;
		}
	    }

	    if ($testlen > $linelen) {	#???
		&putline(&adjust($outline, $type) . "\n");
		$outline = '';
	    }

	    if ($outline eq '') {
		$outline = $word[$i];
	    }
	    elsif ($i == 1) {
		$outline = $outline . $word[$i];
	    }
	    else {
		if ('.!?:;' =~ ("\\" . substr($outline, length($outline),
		  1)) && ($RLENGTH = length($&), $RSTART = length($`)+1)) {
		    $outline = $outline . '  ' . $word[$i];# 2 spaces
		    ;
		}
		else {
		    $outline = $outline . ' ' . $word[$i];# 1 space
		    ;
		}
	    }
	}
    }

    if ($_ =~ $startwords) {
	$nf = (@word = split("[ \t]+", $_, 9999));

	for ($i = 1; $i <= $nf; $i++) {
	    if ($outline eq '') {
		$testlen = length($word[$i]);
	    }
	    else {
		$testlen = length($outline . ' ' . $word[$i]);
		if ('.!?:;' =~ ("\\" . substr($outline, length($outline),
		  1)) && ($RLENGTH = length($&), $RSTART = length($`)+1)) {
		    $testlen++;
		}
	    }

	    if ($testlen > $linelen) {	#???
		&putline(&adjust($outline, $type) . "\n");
		$outline = '';
	    }

	    if ($outline eq '') {
		$outline = $word[$i];
	    }
	    else {
		if ('.!?:;' =~ ("\\" . substr($outline, length($outline),
		  1)) && ($RLENGTH = length($&), $RSTART = length($`)+1)) {
		    $outline = $outline . '  ' . $word[$i];# 2 spaces
		    ;
		}
		else {
		    $outline = $outline . ' ' . $word[$i];# 1 space
		    ;
		}
	    }
	}
    }

    #
    # -- support functions --
    #
}

if ($type eq 'b') {
    &putline($outline . "\n");
}
else {
    &putline(&adjust($outline, $type) . "\n");
}
printf "Checksums: e = %d k = %d s = %d\n", $ecount, $kcount, $scount;

sub putline {
    local($line, $fmt) = @_;
    $ecount += ((@dummy = split(/e/, $line, 9999)) - 1);
    $kcount += ((@dummy = split(/k/, $line, 9999)) - 1);
    $scount += ((@dummy = split(/s/, $line, 9999)) - 1);

    #    if ( indent ) {
    #        fmt = "%" indent "s%s"
    #        printf( fmt, " ", line )
    #    } else
    #        printf( "%s", line )
}

sub adjust {
    local($line, $type, $fill, $fmt) = @_;
    if ($type ne 'l') {
	$fill = $linelen - length($line);
    }
    if ($fill > 0) {
	if ($type eq 'c') {
	    $fmt = '%' . ($fill + 1) / 2 . 's%s';
	    $line = sprintf($fmt, ' ', $line);
	}
	elsif ($type eq 'r') {
	    $fmt = '%' . $fill . 's%s';
	    $line = sprintf($fmt, ' ', $line);
	}
	elsif ($type eq 'b') {
	    $line = &fillout($line, $fill);
	}
    }

    $line;
}

sub fillout {
    local($line, $need, $i, $newline, $nextchar, $blankseen) = @_;
    while ($need) {
	$newline = '';
	$blankseen = 0;

	if ($dir == 0) {
	    for ($i = 1; $i <= length($line); $i++) {
		$nextchar = substr($line, $i, 1);
		if ($need) {
		    if ($nextchar eq ' ') {
			if (!$blankseen) {
			    $newline = $newline . ' ';
			    $need--;
			    $blankseen = 1;
			}
		    }
		    else {
			$blankseen = 0;
		    }
		}
		$newline = $newline . $nextchar;
	    }
	}
	elsif ($dir == 1) {
	    for ($i = length($line); $i >= 1; $i--) {
		$nextchar = substr($line, $i, 1);
		if ($need) {
		    if ($nextchar eq ' ') {
			if (!$blankseen) {
			    $newline = ' ' . $newline;
			    $need--;
			    $blankseen = 1;
			}
		    }
		    else {
			$blankseen = 0;
		    }
		}
		$newline = $nextchar . $newline;
	    }
	}

	$line = $newline;

	$dir = 1 - $dir;
    }

    $line;
}
