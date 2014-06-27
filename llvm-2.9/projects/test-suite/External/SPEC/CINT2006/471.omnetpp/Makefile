##===- Makefile --------------------------------------------*- Makefile -*-===##
#
# This makefile contains information for building the SPEC2006 omnetpp test.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../../..

FP_TOLERANCE    = 0.00001
FP_ABSTOLERANCE = 0.000001

TOPSources    := \
         $(addprefix $(SPEC_BENCH_DIR)/src/,                                   \
           EtherAppCli.cc EtherAppCli_n.cc EtherAppSrv.cc EtherAppSrv_n.cc     \
           EtherApp_m.cc EtherBus.cc EtherBus_n.cc EtherCtrl_m.cc              \
           EtherEncap.cc EtherEncap_n.cc EtherFrame_m.cc EtherHost_n.cc        \
           EtherHub.cc EtherHub_n.cc EtherLLC.cc EtherLLC_n.cc EtherMAC.cc     \
           EtherMAC_n.cc EtherSwitch_n.cc LargeNet_n.cc MACAddress.cc          \
           MACAddress_m.cc MACRelayUnitBase.cc MACRelayUnitNP.cc               \
           MACRelayUnitNP_n.cc MACRelayUnitPP.cc MACRelayUnitPP_n.cc           \
           MACRelayUnit_n.cc Networks_n.cc eth-index_n.cc utils.cc)
CMDENVSources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/libs/cmdenv/,                       \
           cmdenv.cc enumstr.cc heap.cc)
ENVIRSources  := \
         $(addprefix $(SPEC_BENCH_DIR)/src/libs/envir/,                        \
           akoutvectormgr.cc args.cc cenvir.cc cinifile.cc filemgrs.cc main.cc \
           omnetapp.cc patmatch.cc platdep.cc seeds.cc slaveapp.cc speedmtr.cc)
SIMSources    := \
         $(addprefix $(SPEC_BENCH_DIR)/src/libs/sim/,                          \
           carray.cc cexception.cc cmessage.cc cpar.cc ctypes.cc task.cc       \
           cchannel.cc cfsm.cc cmodule.cc cpsquare.cc cvarhist.cc util.cc      \
           ccoroutine.cc cgate.cc cmsgheap.cc cqueue.cc cwatch.cc cdensity.cc  \
           chead.cc cnetmod.cc csimul.cc distrib.cc cdetect.cc chist.cc        \
           cobject.cc cstat.cc errmsg.cc cdispstr.cc cksplit.cc coutvect.cc    \
           cstruct.cc onstartup.cc cenum.cc cllist.cc cpacket.cc ctopo.cc      \
           random.cc)
SIMSTDSources := \
         $(SPEC_BENCH_DIR)/src/libs/sim/std/netpack.cc
SPECSources   := \
         $(SPEC_BENCH_DIR)/src/libs/spec/spec_qsort.cc

Source := $(notdir $(TOPSources))                           \
          $(addprefix cmdenv_,$(notdir $(CMDENVSources)))   \
          $(addprefix envir_,$(notdir $(ENVIRSources)))     \
          $(addprefix sim_,$(notdir $(SIMSources)))         \
          $(addprefix simstd_,$(notdir $(SIMSTDSources)))   \
          $(addprefix spec_,$(notdir $(SPECSources)))

CPPFLAGS += -DNDEBUG=1                            \
            -I$(SPEC_BENCH_DIR)/src/omnet_include \
            -I$(SPEC_BENCH_DIR)/src/libs/cmdenv   \
            -I$(SPEC_BENCH_DIR)/src/libs/envir    \
            -I$(SPEC_BENCH_DIR)/src/libs/sim      \
            -I$(SPEC_BENCH_DIR)/src/libs/spec     \
            -I$(SPEC_BENCH_DIR)/src               \
            -I$(SPEC_BENCH_DIR)/src/include

RUN_OPTIONS      = omnetpp.ini
STDOUT_FILENAME := omnetpp.log

LDFLAGS = -lstdc++ -lm
LIBS    = -lm

include ../../Makefile.spec2006

##===----------------------------------------------------------------------===##
# Copy the source files to the working directory. Some files may have
# the same names but are in different directories, so we add a prefix
# to the filenames to prevent overwriting files.
##===----------------------------------------------------------------------===##

$(notdir $(TOPSources)) : % : $(SPEC_BENCH_DIR)/src/%
	cp $< $@
$(addprefix cmdenv_,$(notdir $(CMDENVSources))) : cmdenv_% : \
$(SPEC_BENCH_DIR)/src/libs/cmdenv/%
	cp $< $@
$(addprefix envir_,$(notdir $(ENVIRSources))) : envir_% : \
$(SPEC_BENCH_DIR)/src/libs/envir/%
	cp $< $@
$(addprefix sim_,$(notdir $(SIMSources))) : sim_% : \
$(SPEC_BENCH_DIR)/src/libs/sim/%
	cp $< $@
$(addprefix simstd_,$(notdir $(SIMSTDSources))) : simstd_% : \
$(SPEC_BENCH_DIR)/src/libs/sim/std/%
	cp $< $@


$(addprefix spec_,$(notdir $(SPECSources)) ): spec_% : $(SPEC_BENCH_DIR)/src/libs/spec/%
	cp $< $@

##===----------------------------------------------------------------------===##
# Clean up all of the copied source files.
##===----------------------------------------------------------------------===##

clean::
	$(RM) -f $(Source)
