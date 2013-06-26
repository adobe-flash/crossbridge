LEVEL = ../../../..
RUN_OPTIONS = -c

ifeq ($(ENDIAN),big)
STDIN_FILENAME := ctl.big
else
STDIN_FILENAME := ctl.lit
CPPFLAGS += -DLEHOST
endif

STDOUT_FILENAME = $(RUN_TYPE).out

Source := addd.c \
          adds.c \
          alignd.c \
          aligns.c \
          asm.c \
          asmcmdstr.c \
          bf.c \
          bm.c \
          br.c \
          bs.c \
          ckiob.c \
          classify.c \
          cm.c \
          cmdparser.c \
          cmdstruct.c \
          cmmu.c \
          cmmu_atc.c \
          cmmu_cache.c \
          cmmu_ctl.c \
          cmmu_debug.c \
          cmmu_func.c \
          cmmu_init.c \
          converters.c \
          ctlregs.c \
          dc.c \
          dis.c \
          divd.c \
          divs.c \
          dmem.c \
          dpath.c \
          fadd.c \
          fadd64.c \
          fadds.c \
          fcdi.c \
          fcds.c \
          fcid.c \
          fcis.c \
          fcmp.c \
          fcmp64.c \
          fcmps.c \
          fcsd.c \
          fcsi.c \
          fdiv.c \
          fdiv64.c \
          fdivs.c \
          floaterr.c \
          flt.c \
          fmul.c \
          fmul64.c \
          fmuls.c \
          fpunimp.c \
          fsub.c \
          fsub64.c \
          fsubs.c \
          go.c \
          he.c \
          id.c \
          instab.c \
          int.c \
          interface.c \
          lo.c \
          main.c \
          map.c \
          md.c \
          mem.c \
          mm.c \
          multd.c \
          mults.c \
          normalized.c \
          normalizes.c \
          opn_output.c \
          pc.c \
          rd.c \
          reserved.c \
          reserves.c \
          returnd.c \
          returns.c \
          rm.c \
          round.c \
          roundd.c \
          rounds.c \
          runsim.c \
          sdsr.c \
          show.c \
          signals.c \
          sim_io.c \
          sim_printf.c \
          simload.c \
          simtime.c \
          stats.c \
          symbols.c \
          sysVbcs.c \
          sysface.c \
          table.c \
          trap.c \
          updstat.c

include ../../Makefile.spec95

$(REF_IN_DIR)/ctl.$(EXTENSION): $(REF_IN_DIR)/ctl.raw
	$(SED) -e s/%endian%/$(EXTENSION)/ < $< > $@

