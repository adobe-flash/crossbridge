BEGIN {
    print "static const char *table[][2] = {"
}

/^\tkeynames\[[^\]]+] =/ {
    if (match($0, /"[^"]+"/))
    {
        name = substr($0, RSTART + 1, RLENGTH - 2)
        print "    { \"" name "\", " "N_(\"" name "\") },"
    }
}

END {
    print "};"
}

