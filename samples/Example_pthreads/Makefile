$?FLASCC=$(PWD)/../sdk
SWFS=atomic.swf flash++flavors.swf flash++ui.swf openmp.swf pthreads.swf sema.swf workers.swf

all: $(SWFS)

atomic.swf: atomic.cpp
	$(FLASCC)/usr/bin/g++ -emit-swf -O4 -flto-api=exports.txt -pthread atomic.cpp -o atomic.swf

pthreads.swf: pthreads.cpp
	$(FLASCC)/usr/bin/g++ -emit-swf -O4 -flto-api=exports.txt -pthread pthreads.cpp -o pthreads.swf

sema.swf: sema.cpp
	$(FLASCC)/usr/bin/g++ -emit-swf -O4 -flto-api=exports.txt -pthread sema.cpp -o sema.swf

workers.swf: workers.cpp
	$(FLASCC)/usr/bin/g++ -emit-swf -O4 -flto-api=exports.txt -pthread workers.cpp -o workers.swf

openmp.swf: openmp.cpp
	$(FLASCC)/usr/bin/g++ -emit-swf -O4 -flto-api=exports.txt -pthread -fopenmp openmp.cpp -o openmp.swf

flash++flavors.swf: flash++flavors.cpp
	$(FLASCC)/usr/bin/g++ -emit-swf -O4 -flto-api=exports.txt -pthread flash++flavors.cpp -lFlash++ -lAS3++ -o flash++flavors.swf

flash++ui.swf: flash++ui.cpp
	$(FLASCC)/usr/bin/g++ -emit-swf -O4 -flto-api=exports.txt -pthread flash++ui.cpp -lFlash++ -lAS3++ -o flash++ui.swf

include ../Makefile.common

clean:
	rm -f $(SWFS)