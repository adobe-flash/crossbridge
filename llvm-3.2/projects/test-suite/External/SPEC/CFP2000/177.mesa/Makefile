LEVEL = ../../../..
FP_ABSTOLERANCE := 4

ifdef LARGE_PROBLEM_SIZE
RUN_OPTIONS := -frames 500
else
ifdef SMALL_PROBLEM_SIZE
RUN_OPTIONS := -frames 10
else
RUN_OPTIONS := -frames 100
endif
endif

RUN_OPTIONS += -meshfile mesa.in -ppmfile mesa.ppm
CPPFLAGS =
HASH_PROGRAM_OUTPUT = 1

Source = $(addprefix $(SPEC_BENCH_DIR)/src/,  \
           accum.c alpha.c alphabuf.c api1.c api2.c attrib.c bitmap.c blend.c \
           clip.c colortab.c context.c copypix.c depth.c dlist.c drawpix.c    \
           enable.c eval.c feedback.c fog.c get.c hash.c image.c light.c      \
           lines.c logic.c masking.c matrix.c misc.c mmath.c osmesa.c pb.c    \
           pixel.c pointers.c points.c polygon.c quads.c rastpos.c readpix.c  \
           rect.c scissor.c shade.c span.c stencil.c teximage.c texobj.c      \
           texstate.c texture.c triangle.c varray.c vb.c vbfill.c vbrender.c  \
           vbxform.c winpos.c xform.c mesa4.c)

include ../../Makefile.spec2000
