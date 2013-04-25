T06: check
	@echo "-------- Sample 6 --------"

	# Build the PassingData example
	cd PassingData && make FLASCC="$(FLASCC)" FLEX="$(FLEX)"

	# Build the MurmurHash example
	cd MurmurHash && make FLASCC="$(FLASCC)" FLEX="$(FLEX)"

	# Build the LodePNG example
	cd LodePNG && make FLASCC="$(FLASCC)" FLEX="$(FLEX)"

	# Build the Typemaps example
	cd Typemaps && make FLASCC="$(FLASCC)" FLEX="$(FLEX)"

include ../Makefile.common

clean:
	cd PassingData && make clean
	cd MurmurHash && make clean
	cd LodePNG && make clean
	cd Typemaps && make clean
