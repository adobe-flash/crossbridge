# APPLE LOCAL file fix-and-continue 6360409

if { [istarget "i?86-*-darwin*"] } {
  set additional_flags "-mno-fix-and-continue"
}
return 0
