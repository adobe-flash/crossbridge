LEVEL = ../../../../..
RUN_OPTIONS = -cpl $(SourceDir)/../data/clinton.pcm
PROG = toast
CPPFLAGS += -DNeedFunctionPrototypes=1 -DSASR

Source = add.c         gsm_decode.c   gsm_option.c  rpe.c          toast.c \
         code.c        gsm_destroy.c  gsm_print.c   short_term.c   toast_lin.c\
         debug.c       gsm_encode.c   long_term.c   table.c     toast_ulaw.c\
         decode.c      gsm_explode.c  lpc.c         toast_alaw.c \
	 gsm_create.c  gsm_implode.c  preprocess.c  toast_audio.c


include $(LEVEL)/MultiSource/Makefile.multisrc



