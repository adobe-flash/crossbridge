LEVEL = ../../../..
STDOUT_FILENAME := test.out
include ../../Makefile.spec95

ifeq ($(RUN_TYPE),ref)
STDIN_FILENAME = bigtest.in
else
STDIN_FILENAME = test.in
endif

CFLAGS += -Wno-return-type
