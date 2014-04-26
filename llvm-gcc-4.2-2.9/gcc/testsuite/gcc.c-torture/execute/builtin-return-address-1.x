# APPLE LOCAL file radar 4204303
# skip for -fomit-frame-pointer
if { [istarget "*-*-darwin*"] } {
	set additional_flags "-fno-omit-frame-pointer"
}

return 0
