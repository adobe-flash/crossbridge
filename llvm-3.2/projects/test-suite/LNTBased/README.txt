llvm-test LNT-based tests
=========================

This directory contains test components designed to run as part of the LNT based
"nightly test" infrastructure.

These test components are designed to impose minimal requirements on the code
being tested or on how the test is run. Instead, the tests must provide an
extension module that will be run by LNT and will be passed the user parameters
(compiler to test, optimization flags, etc.), and is expected to return back an
LNT testing report.

Tests are auto-discovered by the LNT 'nt' test by looking for the TestModule
extension module files. See the 'Examples' sub-directory for more information.
