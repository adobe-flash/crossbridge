# adj.nawk -- adjust lines of text per options
#
# NOTE:  this nawk program is called from the shell script "adj"
#    see that script for usage & calling conventions
#
# author:
#    Norman Joseph (amanue!oglvee!norm)

BEGIN  {
    FS = "\n"
    blankline  = "^[ \t]*$"
    startblank = "^[ \t]+[^ \t]+"
    startwords = "^[^ \t]+"
}

$0 ~ blankline {
    if ( type == "b" )
        putline( outline "\n" )
    else
        putline( adjust( outline, type ) "\n" )
    putline( "\n" )
    outline = ""
}

$0 ~ startblank {
    if ( outline != "" ) {
        if ( type == "b" )
            putline( outline "\n" )
        else
            putline( adjust( outline, type ) "\n" )
    }

    firstword = ""
    i = 1
    while ( substr( $0, i, 1 ) ~ "[ \t]" ) {
        firstword = firstword substr( $0, i, 1 )
        i++
    }
    inline = substr( $0, i )
    outline = firstword

    nf = split( inline, word, "[ \t]+" )

    for ( i = 1;  i <= nf;  i++ ) {
        if ( i == 1 ) {
            testlen = length( outline word[i] )
        } else {
            testlen = length( outline " " word[i] )
            if ( match( ".!?:;", "\\" substr( outline, length( outline ), 1 )) )
                testlen++
        }

        if ( testlen > linelen ) {
            putline( adjust( outline, type ) "\n" )
            outline = ""
        }

        if ( outline == "" )
            outline = word[i]
        else if ( i == 1 )
            outline = outline word[i]
        else {
            if ( match( ".!?:;", "\\" substr( outline, length( outline ), 1 )) )
                outline = outline "  " word[i]     # 2 spaces
            else
                outline = outline " " word[i]      # 1 space
        }
    }
}

$0 ~ startwords  {
    nf = split( $0, word, "[ \t]+" )

    for ( i = 1;  i <= nf;  i++ ) {
        if ( outline == "" )
            testlen = length( word[i] )
        else {
            testlen = length( outline " " word[i] )
            if ( match( ".!?:;", "\\" substr( outline, length( outline ), 1 )) )
                testlen++
        }

        if ( testlen > linelen ) {
            putline( adjust( outline, type ) "\n" )
            outline = ""
        }

        if ( outline == "" )
            outline = word[i]
        else {
            if ( match( ".!?:;", "\\" substr( outline, length( outline ), 1 )) )
                outline = outline "  " word[i]     # 2 spaces
            else
                outline = outline " " word[i]      # 1 space
        }
    }
}

END  {
    if ( type == "b" )
        putline( outline "\n" )
    else
        putline( adjust( outline, type ) "\n" )
    printf ("Checksums: e = %d k = %d s = %d\n", ecount, kcount, scount);
}


#
# -- support functions --
#

function putline( line,    fmt )
{
    ecount += (split(line, dummy, "e") -1)
    kcount += (split(line, dummy, "k") -1)
    scount += (split(line, dummy, "s") -1)

#    if ( indent ) {
#        fmt = "%" indent "s%s"
#        printf( fmt, " ", line )
#    } else
#        printf( "%s", line )
}


function adjust( line, type,    fill, fmt )
{
    if ( type != "l" )
        fill = linelen - length( line )

    if ( fill > 0 ) {
        if        ( type == "c" ) {
            fmt = "%" (fill+1)/2 "s%s"
            line = sprintf( fmt, " ", line )
        } else if ( type == "r" ) {
            fmt = "%" fill "s%s"
            line = sprintf( fmt, " ", line )
        } else if ( type == "b" ) {
            line = fillout( line, fill )
        }
    }

    return line
}


function fillout( line, need,    i, newline, nextchar, blankseen )
{
    while ( need ) {
        newline = ""
        blankseen = 0

        if ( dir == 0 ) {
            for ( i = 1;  i <= length( line );  i++ ) {
                nextchar = substr( line, i, 1 )
                if ( need ) {
                    if ( nextchar == " " ) {
                        if ( ! blankseen ) {
                            newline = newline " "
                            need--
                            blankseen = 1
                        }
                    } else {
                        blankseen = 0
                    }
                }
                newline = newline nextchar
            }

        } else if ( dir == 1 ) {
            for ( i = length( line );  i >= 1;  i-- ) {
                nextchar = substr( line, i, 1 )
                if ( need ) {
                    if ( nextchar == " " ) {
                        if ( ! blankseen ) {
                            newline = " " newline
                            need--
                            blankseen = 1
                        }
                    } else {
                        blankseen = 0
                    }
                }
                newline = nextchar newline
            }
        }

        line = newline

        dir = 1 - dir
    }

    return line
}
