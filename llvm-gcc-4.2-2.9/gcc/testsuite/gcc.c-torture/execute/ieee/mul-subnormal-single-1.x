if [istarget "mips-sgi-irix6*"] {
    # IRIX 6 sets the MIPS IV flush to zero bit by default, so this test
    # isn't expected to work for n32 and n64 on MIPS IV targets.
    return 1
}
# APPLE LOCAL begin ARM no hw subnormal support
if [istarget "arm*-apple-darwin*"] {
    lappend additional_flags "-msoft-float"
}
# APPLE LOCAL end ARM no hw subnormal support
return 0
