LEVEL = ../../../

Source = Calignm1.c constants.c defs.c Falign.c fft.c fftFunctions.c Galign11.c\
         genalign11.c genGalign11.c Halignmm.c io.c Lalign11.c Lalignmm.c\
         mltaln9.c MSalign11.c MSalignmm.c mtxutl.c pairlocalalign.c\
         partQalignmm.c partSalignmm.c Qalignmm.c Ralignmm.c rna.c SAalignmm.c\
         Salignmm.c suboptalign11.c tddis.c
PROG = pairlocalalign
CPPFLAGS = -DLLVM
RUN_OPTIONS = -b 62 -g 0.100 -f 2.00 -h 0.100 -L
STDIN_FILENAME = $(PROJ_SRC_DIR)/pyruvate_decarboxylase.fasta
LDFLAGS = -lm
FP_TOLERANCE = 0.00001
HASH_PROGRAM_OUTPUT = 1
include $(LEVEL)/MultiSource/Makefile.multisrc

clean::
	rm -f pre hat2 trace
