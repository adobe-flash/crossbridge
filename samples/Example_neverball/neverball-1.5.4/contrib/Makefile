CFLAGS := -Wall -g -ansi -pedantic

CURVE_TARG := curve$(EXT)
CURVE_SRCS := curve.c
CURVE_OBJS := $(CURVE_SRCS:%.c=%.o)

all: $(CURVE_TARG)

$(CURVE_TARG): $(CURVE_OBJS)
	$(CC) $(CFLAGS) -o $@ $(CURVE_OBJS) -lm

clean:
	$(RM) $(CURVE_TARG) $(CURVE_OBJS)
