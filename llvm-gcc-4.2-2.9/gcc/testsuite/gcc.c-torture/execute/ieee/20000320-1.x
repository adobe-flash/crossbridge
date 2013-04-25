# APPLE LOCAL file ARM no hw subnormal support
if [istarget "arm*-apple-darwin*"] {
    lappend additional_flags "-msoft-float"
}

return 0
