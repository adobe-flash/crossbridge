# MultiSource/mediabench Makefile:  Build all subdirectories automatically

LEVEL = ../../..
PARALLEL_DIRS  := adpcm g721 gsm jpeg mpeg2
#epic fails with lli, needs more investigation
#ghostscript  complex Makefile structure makes it tough to fit it
#             in llvm-test Makefile structure
#mesa library 
#pgp hard to automate test
#pegwit doesn't compile with gcc on linux
#rasta includes a solaris specific library


include $(LEVEL)/Makefile.programs
