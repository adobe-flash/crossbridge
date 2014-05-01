FLASCC=/path/to/FLASCC/sdk
GLS3D=/path/to/GLS3D
# http://ioquake3.org/data/quake3-latest-pk3s.zip
BASEQ3DIR=baseq3

clean:
	echo "Cleaning..."

all:
	cd Quake3 && PATH=$(FLASCC)/usr/bin:$(PATH) make FLASCC=$(FLASCC) GLS3D=$(GLS3D) BASEQ3DIR=$(BASEQ3DIR) -j8

.PHONY: quake3

include ../Makefile.common