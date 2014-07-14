# -----------------------------------------------------------------------------

BEGIN {
    required[1]  = "message"
    required[2]  = "song"
    required[3]  = "back"
    required[4]  = "grad"
    required[5]  = "shot"
    required[6]  = "goal"
    required[7]  = "time"
    required[8]  = "time_hs"
    required[9]  = "goal_hs"
    required[10] = "coin_hs"
    required[11] = "version"
    required[12] = "author"
}

# -----------------------------------------------------------------------------

# Fix up newlines.
/\r$/ {
    sub(/\r$/, "")
}

# -----------------------------------------------------------------------------

# Entity open...
!entity && /^\{/ {
    entity = 1

    for (key in attribs)
        delete attribs[key]

    next
}

entity && !brush && /^[[:space:]]*"[^"]*"[[:space:]]*"[^"]*"/ {
    split($0, fields, "\"")

    if (fields[2] == "classname" && fields[4] == "worldspawn")
        seen_worldspawn = 1
    else
        attribs[fields[2]] = fields[4]
}

# Brush open...
entity && /^\{/ {
    brush = 1
    next
}

# ...brush closed.
brush && /^\}/ {
    brush = 0
    next
}

# ...entity closed.
entity && /^\}/ {
    entity = 0

    # Dump worlspawn attribs.

    if (seen_worldspawn)
    {
        missing = ""

        for (i in required)
        {
            key = required[i]

            if (!key in attribs || attribs[key] !~ /[^[:space:]]/)
                missing = missing " " key
        }

        if (missing)
            print FILENAME ":" missing

        seen_worldspawn = 0

        nextfile
    }

    next
}

# -----------------------------------------------------------------------------
