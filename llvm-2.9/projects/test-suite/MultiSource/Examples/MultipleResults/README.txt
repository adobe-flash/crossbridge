This example shows how to generate multiple numeric results from a single tests.

The test is expected to create an output file:
  Output/FOO.extra-results.txt
where FOO is the tests' name.

The format of the file should be one result per line, with each line in the
form:
--
Result-SomeName: NNN.NNN
--
where SomeName is the name that will be given to the pseudo-test created for the
result, and NNN.NNN is the value for the result.

There is currently no support for assocating a pass/fail indicator with subtest
results.
