dofile("run_bench_tests.lua");
-- must be run last, because one of the tests sets a debug tracer
-- we don't want that active during the benchmark
dofile("tests.lua");
