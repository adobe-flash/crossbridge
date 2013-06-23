#include "custom.h"
extern double chipaspect ;
extern double spot_control ;
extern int cost_only ;
extern int attpercell ;
extern int doPlacement ;
extern int doChannelGraph ;
extern int doGlobalRoute ;
extern int doCompaction ;
extern int pitch ;
int core_expansion_given ;
int wire_est_factor ;
double core_expansion ;
int defaultTracks ;
int routerMaxPaths ;
int routerExtraS   ;
int bareFlag ;
int wireEstimateOnly ;

void readpar(void)
{
FILE *fp;
  
int test ;
char input[1024] ;
char filename[1024] ;

wireEstimateOnly = 0 ;
attpercell = -1 ;
lapFactor  = 1.0;
cost_only  = 0  ;
maxWeight  = 2  ;
baseWeight = 1  ;
chipaspect = -1.0 ;
pinSpacing = -1 ;
trackspacing = -1 ;
doPlacement = 0 ;
doChannelGraph = 0 ;
doGlobalRoute = 0 ;
doCompaction = 0 ;
coreGiven = 0 ;
gOffsetX = -1000000 ;
gOffsetY = -1000000 ;
gridX = -1000000 ;
gridY = -1000000 ;
defaultTracks = -1 ;
routerMaxPaths = -1 ;
routerExtraS   = -1 ;
core_expansion_given = 0 ;
spot_control = -1.0 ;


sprintf( filename, "%s.par" , cktName ) ;
if( (fp = fopen( filename , "r")) == (FILE *) NULL ) {
    fprintf(fpo,"can't open %s\n", filename ) ;
    exit (0);
}

while( fscanf( fp , " %s " , input ) == 1 ) {
    if( strcmp( input , "initialT") == 0 ) {
	test = fscanf( fp , " %lf " , &T ) ;
	if( test != 1 ) {
	    fprintf(fpo,"The value of initialT was not properly");
	    fprintf(fpo,"entered in the .par file\n");
	    exit(0);
	}
    } else if( strcmp( input , "spot.control") == 0 ) {
	test = fscanf( fp , " %lf " , &spot_control ) ;
	if( test != 1 ) {
	    fprintf(fpo,"The value of spot.control was not properly");
	    fprintf(fpo,"entered in the .par file\n");
	    exit(0);
	}
    } else if( strcmp( input , "core.expansion") == 0 ) {
	test = fscanf( fp , " %lf " , &core_expansion ) ;
	if( test != 1 ) {
	    fprintf(fpo,"The value of core.expansion was not properly");
	    fprintf(fpo,"entered in the .par file\n");
	    exit(0);
	}
	core_expansion_given = 1 ;
    } else if( strcmp( input , "wire.estimation.factor") == 0 ) {
	test = fscanf( fp , " %d " , &wire_est_factor ) ;
	if( test != 1 ) {
	    fprintf(fpo,"The value of wire.estimation.factor was not ");
	    fprintf(fpo,"properly entered in the .par file\n");
	    exit(0);
	}
    } else if( strcmp( input , "cost_only") == 0 ) {
	cost_only = 1 ;
    } else if( strcmp( input , "random.seed") == 0 ) {
	test = fscanf( fp , " %d " , &randVar ) ;
	if( test != 1 ) {
	    fprintf(fpo,"The value of random.seed was not properly");
	    fprintf(fpo,"entered in the .par file\n");
	    exit(0);
	}
    } else if( strcmp( input , "att.per.cell") == 0 ) {
	test = fscanf( fp , " %d " , &attpercell ) ;
	if( test != 1 ) {
	    fprintf(fpo,"The value of att.per.cell was not properly");
	    fprintf(fpo,"entered in the .par file\n");
	    exit(0);
	}
    } else if( strcmp( input , "track.spacing") == 0 ) {
	test = fscanf( fp , " %d " , &trackspacing ) ;
	if( test != 1 ) {
	    fprintf(fpo,"The value for track.spacing was not properly");
	    fprintf(fpo,"entered in the .par file\n");
	    exit(0);
	}
	pitch = trackspacing ;
    } else if( strcmp( input , "pin.spacing") == 0 ) {
	test = fscanf( fp , " %d " , &pinSpacing ) ;
	if( test != 1 ) {
	    fprintf(fpo,"The value for pin.spacing was not properly");
	    fprintf(fpo,"entered in the .par file\n");
	    exit(0);
	}
    } else if( strcmp( input , "chip.aspect.ratio") == 0 ) {
	test = fscanf( fp , " %lf " , &chipaspect ) ;
	if( test != 1 ) {
	    fprintf(fpo,"The value for chip.aspect.ratio was not");
	    fprintf(fpo," properly entered in the .par file\n");
	    exit(0);
	}
    } else if( strcmp( input , "router.max.paths") == 0 ) {
	test = fscanf( fp , " %d " , &routerMaxPaths ) ;
	if( test != 1 ) {
	    fprintf(fpo,"The value for router.max.paths was");
	    fprintf(fpo," not properly entered in the .par file\n");
	    exit(0);
	}
    } else if( strcmp( input , "router.extra.sources") == 0 ) {
	test = fscanf( fp , " %d " , &routerExtraS ) ;
	if( test != 1 ) {
	    fprintf(fpo,"The value for router.extra.sources was");
	    fprintf(fpo," not properly entered in the .par file\n");
	    exit(0);
	}
    } else if( strcmp( input , "default.tracks.per.channel") == 0 ) {
	test = fscanf( fp , " %d " , &defaultTracks ) ;
	if( test != 1 ) {
	    fprintf(fpo,"The value for default.tracks.per.channel was");
	    fprintf(fpo," not properly entered in the .par file\n");
	    exit(0);
	}
    } else if( strcmp( input , "wire.estimation.only") == 0 ) {
	wireEstimateOnly = 1 ;
    } else if( strcmp( input , "do.placement") == 0 ) {
	doPlacement = 1 ;
    } else if( strcmp( input , "do.channel.graph") == 0 ) {
	doChannelGraph = 1 ;
    } else if( strcmp( input , "do.global.route") == 0 ) {
	doGlobalRoute = 1 ;
    } else if( strcmp( input , "do.compaction") == 0 ) {
	test = fscanf( fp , " %d " , &doCompaction ) ;
	if( test != 1 ) {
	    fprintf(fpo,"The value for do.compaction was not properly");
	    fprintf(fpo,"entered in the .par file\n");
	    exit(0);
	}
    } else if( strcmp( input , "core") == 0 ) {
	test = fscanf( fp , " %d %d %d %d " , &blockl , &blockr,
						&blockb , &blockt ) ;
	if( test != 4 ) {
	    fprintf(fpo,"The 4 values for core were not properly");
	    fprintf(fpo,"entered in the .par file\n");
	    exit(0);
	}
	coreGiven = 1 ;
    } else if( strcmp( input , "gridOffsetX") == 0 ) {
	test = fscanf( fp , " %d " , &gOffsetX ) ;
	if( test != 1 ) {
	    fprintf(fpo,"The value for gridOffsetX was not properly");
	    fprintf(fpo,"entered in the .par file\n");
	    exit(0);
	}
    } else if( strcmp( input , "gridOffsetY") == 0 ) {
	test = fscanf( fp , " %d " , &gOffsetY ) ;
	if( test != 1 ) {
	    fprintf(fpo,"The value for gridOffsetY was not properly");
	    fprintf(fpo,"entered in the .par file\n");
	    exit(0);
	}
    } else if( strcmp( input , "gridX") == 0 ) {
	test = fscanf( fp , " %d " , &gridX ) ;
	if( test != 1 ) {
	    fprintf(fpo,"The value for gridX was not properly");
	    fprintf(fpo,"entered in the .par file\n");
	    exit(0);
	}
    } else if( strcmp( input , "gridY") == 0 ) {
	test = fscanf( fp , " %d " , &gridY ) ;
	if( test != 1 ) {
	    fprintf(fpo,"The value for gridY was not properly");
	    fprintf(fpo,"entered in the .par file\n");
	    exit(0);
	}
    } else {
	fprintf(fpo,"Unexpected keyword in the .par file\n");
	exit(0);
    }
}

if( doPlacement ) {
    if( gridX >= 0 || gridY >= 0 || gOffsetX >= 0 || gOffsetY >= 0 ) {
	if( !(gridX >= 0 && gridY >= 0 && 
				gOffsetX >= 0 && gOffsetY >= 0 ) ) {
	    fprintf(fpo,"Error: It appears as though the grid is\n");
	    fprintf(fpo,"not fully specified\n");
	    exit(0);
	} else {
	    gridGiven = 1 ;
	}
    }
    if( attpercell == -1 ) {
	fprintf(fpo,"att.per.cell was not entered in the .par file\n");
	exit(0);
    } else {
	fprintf(fpo,"att.per.cell: %d\n" , attpercell ) ;
    }
    if( trackspacing < 0 ) {
	fprintf(fpo,"track.spacing was not entered in .par file\n");
	exit(0);
    } else {
	fprintf(fpo,"track.spacing: %d\n" , trackspacing ) ;
    }
    if( pinSpacing < 0 ) {
	fprintf(fpo,"pin.spacing was not entered in the .par file\n");
	exit(0);
    } else {
	fprintf(fpo,"pin.spacing: %d\n" , pinSpacing ) ;
    }
    if( chipaspect < 0.0 ) {
	fprintf(fpo,"chip.aspect.ratio was not entered ");
	fprintf(fpo,"in the .par file\n");
	exit(0);
    } else {
	fprintf(fpo,"chip.aspect.ratio: %g\n" , chipaspect ) ;
    }
    if( spot_control < 0.0 ) {
	fprintf(fpo,"Using default value of spot.control: 1.0\n");
	spot_control = 1.0 ;
    } else {
	fprintf(fpo,"spot.control set to: %f\n", spot_control );
    }
}
if( doPlacement ) {
    fprintf(fpo,"TimberWolf instructed to do ");
    fprintf(fpo,"placement of circuit:<%s>\n", cktName ) ;
}
if( doChannelGraph ) {
    fprintf(fpo,"TimberWolf instructed to generate ");
    fprintf(fpo,"channel graph for circuit:<%s>\n", cktName ) ;
    if( ! doPlacement ) {
	fprintf(fpo,"TimberWolf assumes input files: ");
	fprintf(fpo,"<%s.geo> and <%s.pin> are present in\n",
						cktName, cktName );
    }
}
if( doGlobalRoute ) {
    if( routerMaxPaths < 0 ) {
	fprintf(fpo,"router.max.paths was not entered in .par file\n");
	exit(0);
    } else {
	fprintf(fpo,"router.max.paths: %d\n" , routerMaxPaths ) ;
    }
    if( routerMaxPaths == 0 ) {
	fprintf(fpo,"router.max.paths set to BARE MINIMUM version\n");
	routerMaxPaths = 1 ;
	bareFlag = 1 ;
    } else {
	bareFlag = 0 ;
    }


    if( routerExtraS < 0 ) {
	fprintf(fpo,"router.extra.sources was not entered in .par file\n");
	exit(0);
    } else {
	fprintf(fpo,"router.extra.sources: %d\n" , routerExtraS ) ;
    }
    if( doChannelGraph ) {
	if( trackspacing < 0 ) {
	    fprintf(fpo,"track.spacing was not entered in .par file\n");
	    exit(0);
	} else {
	    fprintf(fpo,"track.spacing: %d\n" , trackspacing ) ;
	}
	if( defaultTracks < 0 ) {
	    fprintf(fpo,"default.tracks.per.channel \n");
	    fprintf(fpo,"was not entered in .par file\n");
	    exit(0);
	} else {
	    fprintf(fpo,"default.tracks.per.channel: %d\n",defaultTracks);
	}
    }
    fprintf(fpo,"TimberWolf instructed to do ");
    fprintf(fpo,"global route for circuit:<%s>\n", cktName ) ;
    if( ! doChannelGraph ) {
	fprintf(fpo,"TimberWolf assumes input files: ");
	fprintf(fpo,"<%s.gph> and <%s.twf> are present in ",
						cktName, cktName );
	fprintf(fpo,"the working directory\n") ;
    }
}
if( doCompaction ) {
    fprintf(fpo,"TimberWolf instructed to do ");
    fprintf(fpo,"post-placement compaction for circuit:<%s>\n",cktName);
    if( !(doPlacement && doChannelGraph && doGlobalRoute) ) {
	fprintf(fpo,"Error: TimberWolf cannot do compaction\n");
	fprintf(fpo,"without request to also do: doPlacement,\n");
	fprintf(fpo,"doChannelGraph and doGlobalRoute\n");
	fprintf(fpo,"Hence, request is cancelled\n");
	doCompaction = 0 ;
    }
}

fflush(fpo);
return ;
}
