LEVEL = ../../../../..
RUN_OPTIONS = -b $(SourceDir)/../data/mei16v2.m2v -r -f -o0 \
              $(PROJ_OBJ_DIR)/tmp%d
PROG = mpeg2decode
LIBS += -lm
LDFLAGS += -lm


Source = display.c  gethdr.c  idct.c     mpeg2dec.c  store.c    verify.c\
         getbits.c  getpic.c  idctref.c  recon.c     subspic.c\
         getblk.c   getvlc.c  motion.c   spatscal.c  systems.c

include $(LEVEL)/MultiSource/Makefile.multisrc
