# readable.awk --- library file to skip over unreadable files
#
# Arnold Robbins, arnold@skeeve.com, Public Domain
# October 2000
# December 2010

BEGIN {
    for (i = 1; i < ARGC; i++) {
        if (ARGV[i] ~ /^[[:alpha:]_][[:alnum:]_]*=.*/ \
            || ARGV[i] == "-" || ARGV[i] == "/dev/stdin")
            continue    # assignment or standard input
        else if ((getline junk < ARGV[i]) < 0) # unreadable
            delete ARGV[i]
        else
            close(ARGV[i])
    }
}
