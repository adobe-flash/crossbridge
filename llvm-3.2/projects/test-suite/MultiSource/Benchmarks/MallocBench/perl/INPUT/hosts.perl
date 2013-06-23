#!./perl
#
# Transform ugly unixops /etc/hosts into nice CS /etc/hosts
#

while (<>) {
    #
    # Line with comment
    #
    if (/(.*)(\#.*)/) {
	$v = $1;
	$comment = $2;
    } else {
	#
	# No comment
	# 
	if (/^[0-9]/) {
	    #
	    # Strip off the newline if there is one...
	    #
	    if (/.*\n/) {
		chop;
	    }
	    $v = $_;
	    $comment = "";
	} else {
	    $comment = $_;
	    $v = "";
	}
    }
    @x = split(/[ \t]+/,$v) ;
    if ( @x[0] !~ /^[0-9]/ ) {
	print $_;
    } else {
	printf("%-16s", @x[0]);
	shift(@x);
	#
	# print FQDN first
	#
	@others = ();
	$gotOne = 0;
	foreach $item ( @x ) {
	    if ( $item =~ /(.*)\.[Cc]olorado\.[eE][dD][uU]/ ) {
		print "$1.Colorado.EDU ";
		$gotOne = 1;
	    } else {
		#
		# Ok, maybe it's not a local domain, but it's a FQDN
		#
		if ( $item =~ /.*\..*/ ) {
		    print "$item", " ";
		    $gotOne = 1;
		} else {
		    push(@others,$item);
		}
	    }
	}
	if ( $gotOne == 0 ) {
	    print STDERR "Could not find FQDN for ", $_, "\n"; 
	}
	foreach $item ( @others ) {
	    print $item; print " ";
	}
	print "	", $comment, "\n";
    }
}
