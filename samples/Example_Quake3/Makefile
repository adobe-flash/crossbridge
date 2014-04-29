FLASCC=/path/to/FLASCC/sdk
ALCEXTRA=/path/to/alcextra
GLS3D=/path/to/GLS3D

clean:
	echo "Cleaning..."

all:
	cd Quake3 && PATH=$(FLASCC)/usr/bin:$(ALCEXTRA)/usr/bin:$(PATH) make FLASCC=$(FLASCC) GLS3D=$(GLS3D) ALCEXTRA=$(ALCEXTRA) BASEQ3DIR=$(BASEQ3DIR) -j8

.PHONY: quake3

include ../Makefile.common