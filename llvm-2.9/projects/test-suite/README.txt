llvm-test Hierarchy
-------------------

Files in this hierarchy comprise complete programs that are to be compiled from
source, linked (optionally), and then compiled to machine code.  Since these
programs are complete and runnable, they are then executed, once in LLI and
once natively.  The output is recorded and diff'ed.

In the future, given 'make thorough' we can add trace instrumentation and diff
the trace logs as well.
