LEVEL = ../../..
include $(LEVEL)/Makefile.config

# We can only compile these SPEC benchmarks with F2C
ifdef USE_F2C
PARALLEL_DIRS := \
  101.tomcatv    \
  102.swim       \
  103.su2cor     \
  104.hydro2d    \
  107.mgrid      \
  110.applu      \
  145.fpppp      \
  146.wave5
else
PARALLEL_DIRS :=
endif

# Broken due to f2c: the output of f2c does not compile natively
#  125.turb3d, 141.apsi

include $(LEVEL)/External/Makefile.external
