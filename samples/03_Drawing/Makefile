T03: check
	@echo "-------- Sample 3 --------"
	# We use SWF 17 here to avoid the default behavior of running in a background
	# worker when targeting SWF 18. See the samples documentation for more info.
	"$(FLASCC)/usr/bin/g++" $(BASE_CFLAGS) voronoi.cpp VoronoiDiagramGenerator.cpp -emit-swf -swf-version=17 -swf-size=640x480 -o voronoi.swf

include ../Makefile.common


clean:
	rm -f *.swf *.bc
