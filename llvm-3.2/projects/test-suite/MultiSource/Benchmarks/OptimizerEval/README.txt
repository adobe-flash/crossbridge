This is Preston Brigg's optimizer evaluation framework adjusted slightly for LLVM.
Basically I turned down the number of iterations _dramatically_ to make it run in
a reasonable amount of time on both Sparc and LLVM.  Before it was taking 1/2 hr or
so to run on native sparc!

Since we don't care about the numbers output at this point, this is a reasonable
thing to do.

-Chris
