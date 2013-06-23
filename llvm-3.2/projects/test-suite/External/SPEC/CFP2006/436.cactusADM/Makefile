##===- Makefile --------------------------------------------*- Makefile -*-===##
#
# This makefile contains information for building the SPEC2006 cactusADM test.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../../..

TOPSources            := \
         PreLoop.F StaggeredLeapfrog1a.F StaggeredLeapfrog1a_TS.F              \
         StaggeredLeapfrog2.F planewaves.F teukwaves.F datestamp.c regex.c
PUGHSources           := \
         $(addprefix $(SPEC_BENCH_DIR)/src/PUGH/,                              \
           GHExtension.c FinishReceiveGA.c Startup.c Evolve.c Storage.c        \
           SetupGroup.c PostSendGA.c SetupPGH.c SetupPGV.c LoadAware.c Comm.c  \
           cctk_ThornBindings.c Overloadables.c PughUtils.c PostReceiveGA.c)
TIMESources           := \
         $(addprefix $(SPEC_BENCH_DIR)/src/Time/,                              \
           Courant.c Initialise.c cctk_ThornBindings.c Given.c Simple.c)
CACTUSSources         := \
         $(addprefix $(SPEC_BENCH_DIR)/src/Cactus/,                            \
           ScheduleTraverse.c Groups.c Dummies.c File.c CactusDefaultEvolve.c  \
           BinaryTree.c Hash.c Malloc.c CactusTimers.c CallStartupFunctions.c  \
           FortranBindingsIO.c ConfigData.c CactusDefaultMainLoopIndex.c       \
           Misc.c CactusDefaultComm.c Cache.c RegisterKeyedFunction.c          \
           Subsystems.c FortranWrappers.c Network.c Stagger.c                  \
           CactusDefaultInitialise.c Time.c Expression.c CactusSync.c          \
           ProcessCommandLine.c WarnLevel.c CommandLine.c Coord.c              \
           ScheduleInterface.c MainUtils.c Reduction.c GHExtensions.c          \
           StoreHandledData.c ShutdownCactus.c ProcessEnvironment.c getopt.c   \
           ParseFile.c OverloadIO.c StoreKeyedData.c getopt1.c                 \
           CactusDefaultShutdown.c Banner.c Termination.c                      \
           ProcessParameterDatabase.c ActiveThorns.c String.c SetupCache.c     \
           Table.c DebugDefines.c Interp.c Parameters.c GroupsOnGH.c           \
           InitialiseCactus.c IOMethods.c flesh.c ScheduleCreater.c            \
           SetParams.c cctk_ThornBindings.c OverloadComm.c Names.c             \
           InitialiseDataStructures.c StringList.c DefaultTimers.c             \
           StoreNamedData.c ScheduleSorter.c Complex.c OverloadMain.c          \
           Traverse.c SKBinTree.c snprintf.c)
IOUTILSources         := \
         $(addprefix $(SPEC_BENCH_DIR)/src/IOUtil/,                            \
           CheckpointRecovery.c Utils.c AdvertisedFiles.c Startup.c            \
           cctk_ThornBindings.c)
IDLINEARWAVESSources  := \
         $(SPEC_BENCH_DIR)/src/IDLinearWaves/cctk_ThornBindings.c
BENCHADMSRCSources    := \
         $(addprefix $(SPEC_BENCH_DIR)/src/BenchADMsrc/,                       \
           Startup.c ParamCheck.c cctk_ThornBindings.c)
CACTUSBINDINGSSources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/CactusBindings/,                    \
           ParameterRecoveryEinstein.c ParameterRecoveryCactus.c               \
           TIME_private.c OverloadThorns.c Cactus.c PUGH_Register.c            \
           EINSTEIN_restricted.c ParameterRecoveryIOASCII.c IOASCII.c          \
           CreateTimeParameters.c BOUNDARY_restricted.c                        \
           CreateIOBasicParameters.c Global.c ParameterRecoveryPUGH.c          \
           EINSTEIN_private.c CARTGRID3D_private.c                             \
           BindingsParameterRecovery.c PUGH.c ParameterRecoveryPUGHSlab.c      \
           SchedulePUGH.c IOUtil_Register.c TIME_restricted.c                  \
           Cactus_FortranWrapper.c ParameterRecoveryTime.c                     \
           BenchADM_FortranWrapper.c IOASCII_private.c PUGHReduce.c            \
           CACTUS_private.c Time.c ScheduleTime.c Boundary_Register.c          \
           PUGHReduce_Register.c CreateEinsteinParameters.c BindingsSchedule.c \
           CreateIOASCIIParameters.c CreatePUGHParameters.c BENCHADM_private.c \
           CreateBenchADMParameters.c CreateIOUtilParameters.c                 \
           CreateCartGrid3DParameters.c IDLINEARWAVES_private.c Boundary.c     \
           Einstein.c CreateIDLinearWavesParameters.c                          \
           ParameterRecoveryBenchADM.c ParameterRecoveryIOBasic.c              \
           PUGH_private.c ScheduleIOASCII.c PUGHSlab_Register.c                \
           ScheduleBoundary.c CreatePUGHReduceParameters.c IOBasic.c           \
           ScheduleEinstein.c ParameterRecoveryPUGHReduce.c CartGrid3D.c       \
           IOASCII_Register.c IDLinearWaves_FortranWrapper.c                   \
           PUGH_FortranWrapper.c ScheduleIDLinearWaves.c DummyThornFunctions.c \
           CreateBoundaryParameters.c IO_restricted.c PUGHSlab.c               \
           BenchADM_Register.c CartGrid3D_Register.c SchedulePUGHSlab.c        \
           ParameterRecoveryIDLinearWaves.c IOBASIC_private.c                  \
           SchedulePUGHReduce.c PUGHReduce_FortranWrapper.c ScheduleIOUtil.c   \
           Einstein_Register.c CreateCactusParameters.c Time_Register.c        \
           IOBasic_FortranWrapper.c CreatePUGHSlabParameters.c                 \
           CACTUS_restricted.c BindingsVariables.c IsOverloaded.c              \
           Cactus_Register.c Einstein_FortranWrapper.c                         \
           ParameterRecoveryIOUtil.c IOUtil.c ParameterRecoveryCartGrid3D.c    \
           PUGHSlab_FortranWrapper.c BENCHADM_restricted.c                     \
           BindingsParameters.c CartGrid3D_FortranWrapper.c                    \
           RegisterThornFunctions.c Boundary_FortranWrapper.c                  \
           ScheduleIOBasic.c BenchADM.c IOBasic_Register.c IDLinearWaves.c     \
           Time_FortranWrapper.c IDLinearWaves_Register.c                      \
           IOASCII_FortranWrapper.c ScheduleBenchADM.c ScheduleCactus.c        \
           ImplementationBindings.c DRIVER_restricted.c                        \
           IOUtil_FortranWrapper.c ScheduleCartGrid3D.c                        \
           FortranThornFunctions.c GRID_restricted.c                           \
           ParameterRecoveryBoundary.c)
CARTGRID3DSources     := \
         $(addprefix $(SPEC_BENCH_DIR)/src/CartGrid3D/,                        \
           SymmetryWrappers.c GHExtension.c DecodeSymParameters.c              \
           SetSymmetry.c Startup.c CartGrid3D.c Symmetry.c ParamCheck.c        \
           cctk_ThornBindings.c)
EINSTEINSources       := \
         $(addprefix $(SPEC_BENCH_DIR)/src/Einstein/,                          \
           Courant.c InitialEinstein.c MaskInit.c Slicing.c InitialFlat.c      \
           carttosphere.c InitSymBound.c LapseInits.c cctk_ThornBindings.c     \
           ShiftInits.c evaltrK.c ConfPhys.c)
PUGHREDUCESources     := \
         $(addprefix $(SPEC_BENCH_DIR)/src/PUGHReduce/,                        \
           ReductionNormInf.c ReductionMax.c ReductionMin.c ReductionSum.c     \
           Startup.c Reduction.c ReductionNorm1.c ReductionNorm2.c             \
           cctk_ThornBindings.c)
BOUNDARYSources       := \
         $(addprefix $(SPEC_BENCH_DIR)/src/Boundary/,                          \
           FlatBoundary.c ScalarBoundary.c RadiationBoundary.c RobinBoundary.c \
           CopyBoundary.c cctk_ThornBindings.c)
PUGHSLABSources       := \
         $(addprefix $(SPEC_BENCH_DIR)/src/PUGHSlab/,                          \
           DatatypeConversion.c GetHyperslab.c Mapping.c Hyperslab.c           \
           cctk_ThornBindings.c NewHyperslab.c)
IOASCIISources        := \
         $(addprefix $(SPEC_BENCH_DIR)/src/IOASCII/,                           \
           Output1D.c Output2D.c Output3D.c Startup.c cctk_ThornBindings.c     \
           ChooseOutput.c Write1D.c Write2D.c Write3D.c)
IOBASICSources        := \
         $(addprefix $(SPEC_BENCH_DIR)/src/IOBasic/,                           \
           WriteScalar.c OutputScalar.c OutputInfo.c Startup.c WriteInfo.c     \
           cctk_ThornBindings.c)

Source := $(notdir $(TOPSources))                                 \
          $(addprefix pugh_,$(notdir $(PUGHSources)))             \
          $(addprefix time_,$(notdir $(TIMESources)))             \
          $(addprefix cactus_,$(notdir $(CACTUSSources)))         \
          $(addprefix ioutil_,$(notdir $(IOUTILSources)))         \
          $(addprefix idlw_,$(notdir $(IDLINEARWAVESSources)))    \
          $(addprefix badmsrc_,$(notdir $(BENCHADMSRCSources)))   \
          $(addprefix cbnds_,$(notdir $(CACTUSBINDINGSSources)))  \
          $(addprefix cg3d_,$(notdir $(CARTGRID3DSources)))       \
          $(addprefix einstein_,$(notdir $(EINSTEINSources)))     \
          $(addprefix pughreduce_,$(notdir $(PUGHREDUCESources))) \
          $(addprefix boundary_,$(notdir $(BOUNDARYSources)))     \
          $(addprefix pughslab_,$(notdir $(PUGHSLABSources)))     \
          $(addprefix ioascii_,$(notdir $(IOASCIISources)))       \
          $(addprefix iobasic_,$(notdir $(IOBASICSources)))

CPPFLAGS += \
          -DCCODE                                 \
          -I$(SPEC_BENCH_DIR)/src/PUGH            \
          -I$(SPEC_BENCH_DIR)/src/Time            \
          -I$(SPEC_BENCH_DIR)/src/Cactus          \
          -I$(SPEC_BENCH_DIR)/src/IOUtil          \
          -I$(SPEC_BENCH_DIR)/src/IDLinearWaves   \
          -I$(SPEC_BENCH_DIR)/src/BenchADMsrc     \
          -I$(SPEC_BENCH_DIR)/src/CactusBindings  \
          -I$(SPEC_BENCH_DIR)/src/CartGrid3D      \
          -I$(SPEC_BENCH_DIR)/src/Einstein        \
          -I$(SPEC_BENCH_DIR)/src/PUGHReduce      \
          -I$(SPEC_BENCH_DIR)/src/Boundary        \
          -I$(SPEC_BENCH_DIR)/src/PUGHSlab        \
          -I$(SPEC_BENCH_DIR)/src/IOASCII         \
          -I$(SPEC_BENCH_DIR)/src/IOBasic         \
          -I$(SPEC_BENCH_DIR)/src                 \
          -I$(SPEC_BENCH_DIR)/src/include 

NAGFORTRAN_FLAGS += -maxcontin=39

include ../../Makefile.spec2006
include $(PROJ_SRC_ROOT)/Makefile.FORTRAN

RUN_OPTIONS      = benchADM
STDOUT_FILENAME := benchADM.out

##===----------------------------------------------------------------------===##
# Copy the source files to the working directory. Some files may have the
# same names but are in different directories, so we add a prefix to
# the filenames to prevent overwriting files.
##===----------------------------------------------------------------------===##

$(notdir $(TOPSources)) : % : \
$(SPEC_BENCH_DIR)/src/%
	cp $< $@
$(addprefix pugh_,$(notdir $(PUGHSources))) : pugh_% : \
$(SPEC_BENCH_DIR)/src/PUGH/%
	cp $< $@
$(addprefix time_,$(notdir $(TIMESources))) : time_% : \
$(SPEC_BENCH_DIR)/src/Time/%
	cp $< $@
$(addprefix cactus_,$(notdir $(CACTUSSources))) : cactus_% : \
$(SPEC_BENCH_DIR)/src/Cactus/%
	cp $< $@
$(addprefix ioutil_,$(notdir $(IOUTILSources))) : ioutil_% : \
$(SPEC_BENCH_DIR)/src/IOUtil/%
	cp $< $@
$(addprefix idlw_,$(notdir $(IDLINEARWAVESSources))) : idlw_% : \
$(SPEC_BENCH_DIR)/src/IDLinearWaves/%
	cp $< $@
$(addprefix badmsrc_,$(notdir $(BENCHADMSRCSources))) : badmsrc_% : \
$(SPEC_BENCH_DIR)/src/BenchADMsrc/%
	cp $< $@
$(addprefix cbnds_,$(notdir $(CACTUSBINDINGSSources))) : cbnds_% : \
$(SPEC_BENCH_DIR)/src/CactusBindings/%
	cp $< $@
$(addprefix cg3d_,$(notdir $(CARTGRID3DSources))) : cg3d_% : \
$(SPEC_BENCH_DIR)/src/CartGrid3D/%
	cp $< $@
$(addprefix einstein_,$(notdir $(EINSTEINSources))) : einstein_% : \
$(SPEC_BENCH_DIR)/src/Einstein/%
	cp $< $@
$(addprefix pughreduce_,$(notdir $(PUGHREDUCESources))) : pughreduce_% : \
$(SPEC_BENCH_DIR)/src/PUGHReduce/%
	cp $< $@
$(addprefix boundary_,$(notdir $(BOUNDARYSources))) : boundary_% : \
$(SPEC_BENCH_DIR)/src/Boundary/%
	cp $< $@
$(addprefix pughslab_,$(notdir $(PUGHSLABSources))) : pughslab_% : \
$(SPEC_BENCH_DIR)/src/PUGHSlab/%
	cp $< $@
$(addprefix ioascii_,$(notdir $(IOASCIISources))) : ioascii_% : \
$(SPEC_BENCH_DIR)/src/IOASCII/%
	cp $< $@
$(addprefix iobasic_,$(notdir $(IOBASICSources))) : iobasic_% : \
$(SPEC_BENCH_DIR)/src/IOBasic/%
	cp $< $@

##===----------------------------------------------------------------------===##
# Clean up all of the copied source files.
##===----------------------------------------------------------------------===##

clean::
	$(RM) -f $(Source)
