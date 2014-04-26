# LLVM LOCAL file
# This test has a mismatch between caller and callee arguments.
# llvm gives a warning that cannot be suppressed.  There seems to
# be no way to tell the testsuite to ignore the warning, so
# suppress the test.

load_lib target-supports.exp

if { [is_llvm] } {
	return 1
}

return 0
