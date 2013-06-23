# MultiSource/Benchmarks/MiBench Makefile

LEVEL = ../../..

## NOTE: This must remain in this order, so that the labels in the nightly 
## tester gnuplot scripts are correct.
PARALLEL_DIRS  := \
   automotive-basicmath \
   automotive-bitcount \
   automotive-susan \
   consumer-jpeg \
   consumer-lame \
   consumer-typeset \
   network-dijkstra \
   network-patricia \
   office-ispell \
   office-stringsearch \
   security-blowfish \
   security-rijndael \
   security-sha \
   telecomm-CRC32 \
   telecomm-FFT \
   telecomm-adpcm \
   telecomm-gsm

# FIXME: This is a hack, office-ispell requires termio which is less likely to
# be available on an embedded target. We should have configure support for this.
ifeq ($(ARCH),ARM)
  PARALLEL_DIRS := $(filter-out office-ispell, $(PARALLEL_DIRS))
endif

include $(LEVEL)/Makefile.programs
