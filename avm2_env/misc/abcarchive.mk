LLCOPTS:=

%.abc: %.o
	$(SDK)/usr/bin/llc $(LLCOPTS) -gendbgsymtable -filetype=obj $< -o $@

OBJS := $(wildcard *.o)

all: $(OBJS:.o=.abc)
	$(SDK)/usr/bin/ar r test.a *.abc
