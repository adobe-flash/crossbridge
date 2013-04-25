MacOSX 64-bit plugin with support for profiling JIT'd code in Shark
Tested on OSX 10.6.

Note that each code chunk returned by CodeAlloc is treated as if it were a separate function,
so that you may land in the middle of the function and not get to see it all.  This is probably
infrequent in practice, because with profiling enabled, CodeAlloc grabs 1MB chunks to 
allocate code from.

Scott's original patch would allow Shark to display AS3 source code, not assembler, if file
and line information were compiled into the original AS3.  Since I've been workign with examples
for which source was not available, I haven't yet had the need to try this feature, so it may
or may not work.  [Ed 2/28/2012: I've tried this feature and still can't get it to work].

To install:

1) build player with #define AVMFEATURE_SHARK 1 or shell with configure.py --enable-shark
install the player if necessary.  consider saving the old one and making a symlink
to the plugin you just compiled.
2) cd utils/sharkprof; make; make install ## creates /tmp/jit-32.so and /tmp/jit-64.so

To profile:

1) Start Safari and visit the content you wish to profile, or run standalone player
2) Start Shark, select 'PluginProcess' as the target process (or FlashPlayer)
3) Click Start to begin profiling;  play the game; Stop profiling manually if needed.
   I just set a limit on the number of samples and play until sampling stops.
   I get good results with 25us sampling frequency.
4) run 'mksyms', which will find and convert log files in /tmp.  Or, run mksyms <pid>
   where <pid> is the pid of the plugin process, which should show up in Shark.
5) When Shark finishes analyzing samples, do a 'Get Info' and select 'Symbols' on the pane that drops down.
6) Find jit-32.so or jit-64.so in the list of modules, and click 'Symbolicate'.
7) Navigate to /tmp/<pid>-jit.so.  Ignore any message regarding mismatched names.

Now the "Unknown Library" addresses from AS3 JIT code show up as "jit-xxx.so"
with names from AS3 code.  Now you can use shark data mining.  for example:
 * flatten "FlashPlayer" to charge all player time to entrypoint functions
 * focus on an interesting function.
 * exclude time from ignorable functions

caveats:
 * at high optimization levels, gcc does tail-call optimization.  Some stack
   samples will show the tail-called function as the jit->player entry point.
 * you may still see "Unknown Library" addresses.  I suspect these are from
   software shaders but I have no proof.  
 * This scheme is untested only tested on MacOS 10.6.  YMMV with Instruments
   on 10.6 or 10.7.
