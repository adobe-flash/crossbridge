##===- Makefile --------------------------------------------*- Makefile -*-===##
#
# This makefile contains information for building the SPEC2006 wrf test.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../../..

FP_TOLERANCE    = 5e-2
FP_ABSTOLERANCE = 1.0e-2

TOPSources := \
         wrf_num_bytes_between.c pack_utils.c module_driver_constants.F90      \
         module_domain.F90 module_integrate.F90 module_timing.F90              \
         module_configure.F90 module_tiles.F90 module_machine.F90              \
         module_nesting.F90 module_wrf_error.F90 module_state_description.F90  \
         module_sm.F90 module_io.F90 module_dm_stubs.F90                       \
         module_quilt_outbuf_ops.F90 module_io_quilt.F90 module_bc.F90	       \
         module_io_wrf.F90 module_date_time.F90 module_io_domain.F90           \
         module_bc_time_utilities.F90 module_model_constants.F90               \
         module_soil_pre.F90 module_bl_mrf.F90 module_sf_myjsfc.F90            \
         module_bl_myjpbl.F90 module_bl_ysu.F90 module_cu_bmj.F90              \
         module_mp_kessler.F90 module_mp_ncloud5.F90 module_ra_sw.F90          \
         module_sf_sfclay.F90 module_cu_kf.F90 module_cu_kfeta.F90             \
         module_mp_lin.F90 module_mp_wsm3.F90 module_mp_wsm5.F90               \
         module_mp_wsm6.F90 module_surface_driver.F90 module_cu_gd.F90         \
         module_sf_sfcdiags.F90 module_ra_gsfcsw.F90 module_sf_slab.F90        \
         module_sf_noahlsm.F90 module_sf_ruclsm.F90 module_mp_ncloud3.F90      \
         module_mp_etanew.F90 module_ra_rrtm.F90 module_ra_gfdleta.F90         \
         module_physics_init.F90 module_physics_addtendc.F90                   \
         module_solvedebug_em.F90 module_bc_em.F90 module_advect_em.F90        \
         module_diffusion_em.F90 module_small_step_em.F90                      \
         module_big_step_utilities_em.F90 module_em.F90                        \
         module_init_utilities.F90 module_optional_si_input.F90                \
         ESMF_Alarm.F90 ESMF_Base.F90 ESMF_BaseTime.F90 ESMF_Calendar.F90      \
         ESMF_Clock.F90 ESMF_Fraction.F90 ESMF_Mod.F90 ESMF_Time.F90           \
         ESMF_TimeInterval.F90 Meat.F90 wrf_shutdown.F90 collect_on_comm.c     \
         mediation_integrate.F90 mediation_feedback_domain.F90                 \
         mediation_force_domain.F90 mediation_interp_domain.F90                \
         mediation_wrfmain.F90 wrf_auxhist1in.F90 wrf_auxhist1out.F90          \
         wrf_auxhist2in.F90 wrf_auxhist2out.F90 wrf_auxhist3in.F90             \
         wrf_auxhist3out.F90 wrf_auxhist4in.F90 wrf_auxhist4out.F90            \
         wrf_auxhist5in.F90 wrf_auxhist5out.F90 wrf_auxinput1in.F90            \
         wrf_auxinput1out.F90 wrf_auxinput2in.F90 wrf_auxinput2out.F90         \
         wrf_auxinput3in.F90 wrf_auxinput3out.F90 wrf_auxinput4in.F90          \
         wrf_auxinput4out.F90 wrf_auxinput5in.F90 wrf_auxinput5out.F90         \
         wrf_bdyin.F90 wrf_bdyout.F90 wrf_histin.F90 wrf_histout.F90           \
         wrf_inputin.F90 wrf_inputout.F90 wrf_restartin.F90 wrf_restartout.F90 \
         couple_or_uncouple_em.F90 interp_domain_em.F90 interp_fcn.F90         \
         nest_init_utils.F90 set_timekeeping.F90 sint.F90 solve_interface.F90  \
         start_domain.F90 module_pbl_driver.F90 module_radiation_driver.F90    \
         module_cumulus_driver.F90 module_microphysics_driver.F90              \
         solve_em.F90 start_em.F90 internal_header_util.F90 io_int.F90         \
         init_modules_em.F90 init_modules.F90 wrf_io.f90 field_routines.f90    \
         wrf.F90
NETCDFSources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/netcdf/,                            \
           attr.c dim.c error.c fort-attio.c fort-control.c fort-dim.c         \
           fort-genatt.c fort-geninq.c fort-genvar.c fort-lib.c fort-misc.c    \
           fort-v2compat.c fort-var1io.c fort-varaio.c fort-vario.c            \
           fort-varmio.c fort-varsio.c libvers.c nc.c ncx.c posixio.c putget.c \
           string.c v1hpg.c v2i.c var.c typeSizes.f90 netcdf.f90)

Source := \
         $(notdir $(TOPSources)) \
         $(addprefix netcdf_,$(notdir $(NETCDFSources)))

FPPFLAGS += -w -m literal.pm                                                   \
            -DINTIO -DIWORDSIZE=4 -DDWORDSIZE=8 -DRWORDSIZE=4 -DLWORDSIZE=4    \
            -DNETCDF  -DTRIEDNTRUE -DLIMIT_ARGS -DEM_CORE=1  -DNMM_CORE=0      \
            -DNMM_MAX_DIM=1000 -DCOAMPS_CORE=0  -DEXP_CORE=0 -DF90_STANDALONE  \
            -DCONFIG_BUF_LEN=8192 -DMAX_DOMAINS_F=21 -DNO_NAMELIST_PRINT

CPPFLAGS += \
         -I$(SPEC_BENCH_DIR)/src/netcdf         \
         -I$(SPEC_BENCH_DIR)/src/netcdf/include \
         -I$(SPEC_BENCH_DIR)/src                \
         -I$(SPEC_BENCH_DIR)/src/include 


include ../../Makefile.spec2006
include $(PROJ_SRC_ROOT)/Makefile.FORTRAN

STDOUT_FILENAME := rsl.out.0000

##===----------------------------------------------------------------------===##
# Copy the source files to the working directory. Some files may have the
# same names but are in different directories, so we add a prefix to
# the filenames to prevent overwriting files.
##===----------------------------------------------------------------------===##

$(notdir $(TOPSources)) : % : \
$(SPEC_BENCH_DIR)/src/%
	cp $< $@
$(addprefix netcdf_,$(notdir $(NETCDFSources))) : netcdf_% : \
$(SPEC_BENCH_DIR)/src/netcdf/%
	cp $< $@

##===----------------------------------------------------------------------===##
# Clean up all of the copied source files.
##===----------------------------------------------------------------------===##

clean::
	$(RM) -f $(Source)
