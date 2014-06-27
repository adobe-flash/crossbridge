##===- Makefile --------------------------------------------*- Makefile -*-===##
#
# This makefile contains information for building the SPEC2006 calculix test.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../../..

FP_TOLERANCE    = 1.0e-9
FP_ABSTOLERANCE = 1.0e-9

TOPSources := \
         CalculiX.c add_pr.f add_sm_ei.f add_sm_st.f allocation.f amplitudes.f \
         anisotropic.f beamsections.f bounadd.f boundaries.f buckles.f         \
         calinput.f cfluxes.f changedepterm.f cloads.f conductivities.f        \
         controlss.f couptempdisps.f creeps.f cychards.f cycsymmods.f dasol.f  \
         datest.f datri.f defplasticities.f defplas.f densities.f depvars.f    \
         deuldlag.f dfluxes.f dgesv.f diamtr.f dloads.f dot.f dredu.f dsort.f  \
         dynamics.f dynsolv.f el.f elastics.f elements.f elprints.f envtemp.f  \
         equations.f expansions.f extrapolate.f e_c3d.f e_c3d_th.f e_c3d_rhs.f \
         fcrit.f films.f finpro.f forcadd.f frd.F frdclose.f frequencies.f     \
         fsub.f fsuper.f gen3delem.f genran.f getnewline.f graph.f headings.f  \
         heattransfers.f hyperel.f hyperelastics.f hyperfoams.f ident.f        \
         ident2.f include.f incplas.f initialconditions.f inputerror.f         \
         isorti.f isortid.f isortidc.f isortii.f isortiid.f label.f linel.f    \
         lintemp.f lintemp_th.f loadadd.f loadaddt.f mafillpr.f mafillsm.f     \
         mafillsmcs.f massflowrates.f matdata_co.f matdata_he.f matdata_tg.f   \
         materialdata.f materials.f modaldampings.f modaldynamics.f mpcs.f     \
         nident.f nident2.f near2d.f noanalysis.f nodalthicknesses.f           \
         nodeprints.f nodes.f noelfiles.f noelsets.f nonlinmpc.f normals.f     \
         norshell.f number.f onf.f op.f openfile.f orientations.f orthonl.f    \
         orthotropic.f out.f parser.f physicalconstants.f planempc.f           \
         plastics.f plcopy.f plinterpol.f plmix.f polynom.f profil.f           \
         radflowload.f radiates.f ranewr.f rearrange.f rectcyl.f renumber.f    \
         restartread.f restarts.f restartshort.f restartwrite.f results.f      \
         rhs.f rigidbodies.f rigidmpc.f rootls.f rubber.f saxpb.f              \
         selcycsymmods.f shape3tri.f shape4q.f shape4tet.f shape6tri.f         \
         shape6w.f shape8h.f shape8q.f shape10tet.f shape15w.f shape20h.f      \
         shellsections.f skip.f solidsections.f spcmatch.f specificheats.f     \
         statics.f steps.f stiff2mat.f stop.f str2mat.f straightmpc.f          \
         surfaces.f temperatures.f tempload.f ties.f transformatrix.f          \
         transforms.f ucreep.f uhardening.f umat.f umat_aniso_creep.f          \
         umat_aniso_plas.f umat_elastic_fiber.f umat_ideal_gas.f               \
         umat_lin_iso_el.f umat_single_crystal.f umat_tension_only.f           \
         umat_user.f umpc_mean_rot.f umpc_user.f usermaterials.f usermpc.f     \
         viscos.f wcoef.f writebv.f writeev.f writeevcs.f writempc.f           \
         writesummary.f cascade.c frdcyc.c insert.c mastruct.c mastructcs.c    \
         nonlingeo.c pcgsolver.c preiter.c prespooles.c profile.c remastruct.c \
         spooles.c strcmp1.c strcpy1.c u_calloc.c
A2Sources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/SPOOLES/A2/src/,                    \
           A2_IO.c A2_basics.c A2_init.c A2_instance.c A2_norms.c A2_sort.c    \
           A2_util.c)
BKLSources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/SPOOLES/BKL/src/,                   \
           BKL_basics.c BKL_evalfcn.c BKL_exhSearch.c BKL_fidmat.c BKL_init.c  \
           BKL_util.c)
BPGSources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/SPOOLES/BPG/src/,                   \
           BPG_IO.c BPG_basics.c BPG_init.c BPG_makeGraphs.c BPG_pseudo.c)
CHVSources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/SPOOLES/Chv/src/,                   \
           Chv_IO.c Chv_assemble.c Chv_basics.c Chv_copy.c Chv_factor.c        \
           Chv_findPivot.c Chv_init.c Chv_instance.c Chv_search.c Chv_swap.c   \
           Chv_update.c Chv_util.c)
CHVLISTSources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/SPOOLES/ChvList/src/,               \
           ChvList_basics.c ChvList_init.c ChvList_util.c)
CHVMANAGERSources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/SPOOLES/ChvManager/src/,            \
           ChvManager_basics.c ChvManager_init.c ChvManager_util.c)
DSTREESources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/SPOOLES/DSTree/src/,                \
           DSTree_basics.c DSTree_init.c DSTree_instance.c DSTree_stages.c     \
           DSTree_util.c)
DVSources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/SPOOLES/DV/src/,                    \
           DV_IO.c DV_basics.c DV_init.c DV_instance.c DV_util.c)
DENSEMTXSources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/SPOOLES/DenseMtx/src/,              \
           DenseMtx_IO.c DenseMtx_basics.c DenseMtx_init.c DenseMtx_instance.c \
           DenseMtx_permute.c DenseMtx_util.c)
DRANDSources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/SPOOLES/Drand/src/,                 \
           Drand_basics.c Drand_init.c Drand_util.c)
ETREESources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/SPOOLES/ETree/src/,                 \
           ETree_IO.c ETree_basics.c ETree_compress.c ETree_init.c             \
           ETree_instance.c ETree_permute.c ETree_transform.c ETree_util.c)
FRONTMTXSources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/SPOOLES/FrontMtx/src/,              \
           FrontMtx_IO.c FrontMtx_basics.c FrontMtx_factor.c                   \
           FrontMtx_factorUtil.c FrontMtx_init.c FrontMtx_instance.c           \
           FrontMtx_loadEntries.c FrontMtx_permute.c FrontMtx_postProcess.c    \
           FrontMtx_postponed.c FrontMtx_solve.c FrontMtx_solveUtil.c          \
           FrontMtx_split.c FrontMtx_storeFront.c FrontMtx_update.c            \
           FrontMtx_util.c)
GPARTSources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/SPOOLES/GPart/src/,                 \
           GPart_DDsepInfo.c GPart_DDviaFishnet.c GPart_DDviaProjection.c      \
           GPart_RBviaDDsep.c GPart_TwoSetViaBKL.c GPart_basics.c              \
           GPart_domSegMap.c GPart_identifyWideSep.c GPart_init.c              \
           GPart_makeYCmap.c GPart_smoothBisector.c GPart_smoothBy2layers.c    \
           GPart_smoothYSep.c GPart_split.c GPart_util.c)
GRAPHSources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/SPOOLES/Graph/src/,                 \
           Graph_IO.c Graph_basics.c Graph_compress.c Graph_equivMap.c         \
           Graph_init.c Graph_util.c)
I2OHASHSources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/SPOOLES/I2Ohash/src/,               \
           I2Ohash_basics.c I2Ohash_init.c I2Ohash_util.c)
IIHEAPSources := \
         $(SPEC_BENCH_DIR)/src/SPOOLES/IIheap/src/IIheap_basics.c
IVSources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/SPOOLES/IV/src/,                    \
           IV_IO.c IV_basics.c IV_init.c IV_instance.c IV_util.c)
IVLSources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/SPOOLES/IVL/src/,                   \
           IVL_IO.c IVL_basics.c IVL_init.c IVL_instance.c IVL_listmanip.c     \
           IVL_util.c)
IDEQSources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/SPOOLES/Ideq/src/,                  \
           Ideq_basics.c Ideq_resize.c Ideq_util.c)
INPMTXSources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/SPOOLES/InpMtx/src/,                \
           InpMtx_IO.c InpMtx_basics.c InpMtx_fullAdj.c InpMtx_init.c          \
           InpMtx_input.c InpMtx_instance.c InpMtx_permute.c InpMtx_util.c)
LOCKSources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/SPOOLES/Lock/src/,                  \
           Lock_basics.c Lock_init.c Lock_util.c)
MSMDSources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/SPOOLES/MSMD/src/,                  \
           MSMD_MSMDinfo.c MSMD_MSMDvtx.c MSMD_basics.c MSMD_cleanReachSet.c   \
           MSMD_eliminate.c MSMD_findInodes.c MSMD_frontETree.c MSMD_init.c    \
           MSMD_order.c MSMD_update.c)
NETWORKSources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/SPOOLES/Network/src/,               \
           Network_IO.c Network_addArc.c Network_augmentPath.c                 \
           Network_basics.c Network_findAugmentingPath.c Network_findMaxFlow.c \
           Network_findMincut.c Network_init.c)
PENCILSources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/SPOOLES/Pencil/src/,                \
           Pencil_IO.c Pencil_basics.c Pencil_init.c)
SUBMTXSources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/SPOOLES/SubMtx/src/,                \
           SubMtx_IO.c SubMtx_basics.c SubMtx_init.c SubMtx_instance.c         \
           SubMtx_scalevec.c SubMtx_solve.c SubMtx_solveH.c SubMtx_solveT.c    \
           SubMtx_solveupd.c SubMtx_solveupdH.c SubMtx_solveupdT.c             \
           SubMtx_sort.c SubMtx_util.c)
SUBMTXLISTSources := \
         $(SPEC_BENCH_DIR)/src/SPOOLES/SubMtxList/src/SubMtxList_util.c
SUBMTXMANAGERSources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/SPOOLES/SubMtxManager/src/,         \
           SubMtxManager_basics.c SubMtxManager_init.c SubMtxManager_util.c)
SYMBFACSources := \
         $(SPEC_BENCH_DIR)/src/SPOOLES/SymbFac/src/SymbFac_symbfac.c
TREESources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/SPOOLES/Tree/src/,                  \
           Tree_IO.c Tree_basics.c Tree_compress.c Tree_init.c Tree_metrics.c  \
           Tree_perms.c Tree_util.c)
UTILITIESSources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/SPOOLES/Utilities/src/,             \
           Utilities_CV.c Utilities_DV.c Utilities_I2OP.c Utilities_IP.c       \
           Utilities_IV.c Utilities_PIV.c Utilities_ZV.c Utilities_iohb.c      \
           Utilities_newsort.c Utilities_sortAndCompress.c)
ZVSources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/SPOOLES/ZV/src/,                    \
           ZV_basics.c ZV_init.c ZV_instance.c)
MISCSources := \
         $(SPEC_BENCH_DIR)/src/SPOOLES/misc/src/misc_orderViaBestOfNDandMS.c

Source := \
         $(notdir $(TOPSources))                               \
         $(addprefix a2_,$(notdir $(A2Sources)))               \
         $(addprefix bkl_,$(notdir $(BKLSources)))             \
         $(addprefix bpg_,$(notdir $(BPGSources)))             \
         $(addprefix chv_,$(notdir $(CHVSources)))             \
         $(addprefix chvl_,$(notdir $(CHVLISTSources)))        \
         $(addprefix chvm_,$(notdir $(CHVMANAGERSources)))     \
         $(addprefix dstree_,$(notdir $(DSTREESources)))       \
         $(addprefix dv_,$(notdir $(DVSources)))               \
         $(addprefix dmtx_,$(notdir $(DENSEMTXSources)))       \
         $(addprefix drand_,$(notdir $(DRANDSources)))         \
         $(addprefix etree_,$(notdir $(ETREESources)))         \
         $(addprefix fmtx_,$(notdir $(FRONTMTXSources)))       \
         $(addprefix gpart_,$(notdir $(GPARTSources)))         \
         $(addprefix graph_,$(notdir $(GRAPHSources)))         \
         $(addprefix i2oh_,$(notdir $(I2OHASHSources)))        \
         $(addprefix iiheap_,$(notdir $(IIHEAPSources)))       \
         $(addprefix iv_,$(notdir $(IVSources)))               \
         $(addprefix ivl_,$(notdir $(IVLSources)))             \
         $(addprefix ideq_,$(notdir $(IDEQSources)))           \
         $(addprefix inpmtx_,$(notdir $(INPMTXSources)))       \
         $(addprefix lock_,$(notdir $(LOCKSources)))           \
         $(addprefix msmd_,$(notdir $(MSMDSources)))           \
         $(addprefix network_,$(notdir $(NETWORKSources)))     \
         $(addprefix pencil_,$(notdir $(PENCILSources)))       \
         $(addprefix smtx_,$(notdir $(SUBMTXSources)))         \
         $(addprefix smtxl_,$(notdir $(SUBMTXLISTSources)))    \
         $(addprefix smtxm_,$(notdir $(SUBMTXMANAGERSources))) \
         $(addprefix symbfac_,$(notdir $(SYMBFACSources)))     \
         $(addprefix tree_,$(notdir $(TREESources)))           \
         $(addprefix utils_,$(notdir $(UTILITIESSources)))     \
         $(addprefix zv_,$(notdir $(ZVSources)))               \
         $(addprefix misc_,$(notdir $(MISCSources)))

CPPFLAGS += \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/A2/src                \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/BKL/src               \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/BPG/src               \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/Chv/src               \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/ChvList/src           \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/ChvManager/src        \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/DSTree/src            \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/DV/src                \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/DenseMtx/src          \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/Drand/src             \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/ETree/src             \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/FrontMtx/src          \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/GPart/src             \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/Graph/src             \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/I2Ohash/src           \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/IIheap/src            \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/IV/src                \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/IVL/src               \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/Ideq/src              \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/InpMtx/src            \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/Lock/src              \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/MSMD/src              \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/Network/src           \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/Pencil/src            \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/SubMtx/src            \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/SubMtxList/src        \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/SubMtxManager/src     \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/SymbFac/src           \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/Tree/src              \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/ZV/src                \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/misc/src              \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES                       \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES/Utilities/src         \
         -I$(SPEC_BENCH_DIR)/src                               \
         -I$(SPEC_BENCH_DIR)/src/include 

NAGFORTRAN_FLAGS += \
         -I$(SPEC_BENCH_DIR)/src                               \
         -I$(SPEC_BENCH_DIR)/src/SPOOLES                       \
         -maxcontin=256

include ../../Makefile.spec2006
include $(PROJ_SRC_ROOT)/Makefile.FORTRAN

ifeq ($(RUN_TYPE),test)
  RUN_OPTIONS     := -i beampic
  STDOUT_FILENAME := beampic.dat
else
  RUN_OPTIONS     := -i stairs
  STDOUT_FILENAME := stairs.dat
endif

##===----------------------------------------------------------------------===##
# Copy the source files to the working directory. Some files may have the
# same names but are in different directories, so we add a prefix to
# the filenames to prevent overwriting files.
##===----------------------------------------------------------------------===##

$(notdir $(TOPSources)) : % : \
$(SPEC_BENCH_DIR)/src/%
	cp $< $@
$(addprefix a2_,$(notdir $(A2Sources))) : a2_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/A2/src/%
	cp $< $@
$(addprefix bkl_,$(notdir $(BKLSources))) : bkl_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/BKL/src/%
	cp $< $@
$(addprefix bpg_,$(notdir $(BPGSources))) : bpg_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/BPG/src/%
	cp $< $@
$(addprefix chv_,$(notdir $(CHVSources))) : chv_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/Chv/src/%
	cp $< $@
$(addprefix chvl_,$(notdir $(CHVLISTSources))) : chvl_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/ChvList/src/%
	cp $< $@
$(addprefix chvm_,$(notdir $(CHVMANAGERSources))) : chvm_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/ChvManager/src/%
	cp $< $@
$(addprefix dstree_,$(notdir $(DSTREESources))) : dstree_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/DSTree/src/%
	cp $< $@
$(addprefix dv_,$(notdir $(DVSources))) : dv_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/DV/src/%
	cp $< $@
$(addprefix dmtx_,$(notdir $(DENSEMTXSources))) : dmtx_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/DenseMtx/src/%
	cp $< $@
$(addprefix drand_,$(notdir $(DRANDSources))) : drand_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/Drand/src/%
	cp $< $@
$(addprefix etree_,$(notdir $(ETREESources))) : etree_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/ETree/src/%
	cp $< $@
$(addprefix fmtx_,$(notdir $(FRONTMTXSources))) : fmtx_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/FrontMtx/src/%
	cp $< $@
$(addprefix gpart_,$(notdir $(GPARTSources))) : gpart_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/GPart/src/%
	cp $< $@
$(addprefix graph_,$(notdir $(GRAPHSources))) : graph_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/Graph/src/%
	cp $< $@
$(addprefix i2oh_,$(notdir $(I2OHASHSources))) : i2oh_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/I2Ohash/src/%
	cp $< $@
$(addprefix iiheap_,$(notdir $(IIHEAPSources))) : iiheap_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/IIheap/src/%
	cp $< $@
$(addprefix iv_,$(notdir $(IVSources))) : iv_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/IV/src/%
	cp $< $@
$(addprefix ivl_,$(notdir $(IVLSources))) : ivl_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/IVL/src/%
	cp $< $@
$(addprefix ideq_,$(notdir $(IDEQSources))) : ideq_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/Ideq/src/%
	cp $< $@
$(addprefix inpmtx_,$(notdir $(INPMTXSources))) : inpmtx_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/InpMtx/src/%
	cp $< $@
$(addprefix lock_,$(notdir $(LOCKSources))) : lock_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/Lock/src/%
	cp $< $@
$(addprefix msmd_,$(notdir $(MSMDSources))) : msmd_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/MSMD/src/%
	cp $< $@
$(addprefix network_,$(notdir $(NETWORKSources))) : network_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/Network/src/%
	cp $< $@
$(addprefix pencil_,$(notdir $(PENCILSources))) : pencil_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/Pencil/src/%
	cp $< $@
$(addprefix smtx_,$(notdir $(SUBMTXSources))) : smtx_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/SubMtx/src/%
	cp $< $@
$(addprefix smtxl_,$(notdir $(SUBMTXLISTSources))) : smtxl_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/SubMtxList/src/%
	cp $< $@
$(addprefix smtxm_,$(notdir $(SUBMTXMANAGERSources))) : smtxm_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/SubMtxManager/src/%
	cp $< $@
$(addprefix symbfac_,$(notdir $(SYMBFACSources))) : symbfac_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/SymbFac/src/%
	cp $< $@
$(addprefix tree_,$(notdir $(TREESources))) : tree_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/Tree/src/%
	cp $< $@
$(addprefix utils_,$(notdir $(UTILITIESSources))) : utils_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/Utilities/src/%
	cp $< $@
$(addprefix zv_,$(notdir $(ZVSources))) : zv_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/ZV/src/%
	cp $< $@
$(addprefix misc_,$(notdir $(MISCSources))) : misc_% : \
$(SPEC_BENCH_DIR)/src/SPOOLES/misc/src/%
	cp $< $@

##===----------------------------------------------------------------------===##
# Clean up all of the copied source files.
##===----------------------------------------------------------------------===##

clean::
	$(RM) -f $(Source)
