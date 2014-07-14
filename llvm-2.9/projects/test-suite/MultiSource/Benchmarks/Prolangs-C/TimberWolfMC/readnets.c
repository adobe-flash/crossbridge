#include "custom.h"
double Hdefault ;
double Vdefault ;

extern int hashfind( char hname[] );

void readnets( FILE *fp )
{

int net , test , status ;
double Hweight , Vweight ;
char input[1024] ;


fscanf( fp , " %s ", input ) ;
if( strcmp( input , "allnets") != 0 ) {
    fprintf( fpo, "The keyword allnets was ");
    fprintf( fpo, "not properly entered in the .net file\n");
    exit(0);
}
fscanf( fp , " %s ", input ) ;
if( strcmp( input , "HVweights") != 0 ) {
    fprintf( fpo, "The keyword HVweights was ");
    fprintf( fpo, "not properly entered in the .net file\n");
    exit(0);
}
test = fscanf( fp , "%lf %lf" , &Hweight , &Vweight ) ;
if( test != 2 ) {
    fprintf( fpo, "The net weights for allnets were ");
    fprintf( fpo, "not properly entered in the .net file\n");
    exit(0);
}

for( net = 1 ; net <= numnets ; net++ ) {
    netarray[net]->Hweight = Hweight ;
    netarray[net]->Vweight = Vweight ;
}
Hdefault = Hweight ;
Vdefault = Vweight ;

while( fscanf( fp , " %s " , input ) == 1 ) {
    if( strcmp( input , "net") == 0 ) {
	fscanf( fp , " %s ", input ) ;
	status = hashfind( input ) ;
	if( status == 0 ) {
	    fprintf( fpo, "The net named: %s  in the .net file\n",
						    input );
	    fprintf( fpo, "was not encountered while reading\n");
	    fprintf( fpo, "the .cel file --- FATAL error\n");
	    exit(0);
	}
	fscanf( fp , " %s ", input ) ;
	if( strcmp( input , "HVweights") == 0 ) {
	    test = fscanf( fp , "%lf %lf" , &Hweight , &Vweight ) ;
	    if( test != 2 ) {
		fprintf( fpo, "The critical net weights were ");
		fprintf( fpo, "not properly entered\n");
		fprintf(fpo,"in the .net file after net: %s\n",
							    input);
		exit(0);
	    }
	    netarray[status]->Hweight = Hweight ;
	    netarray[status]->Vweight = Vweight ;
	} else {
	    fprintf(fpo,"Neither the keyword HVweights nor ");
	    fprintf(fpo,"the keyword nofeeds was entered\n");
	    fprintf(fpo,"in the .net file after net: %s\n", input);
	    exit(0);
	}
    } else {
	fprintf( fpo, "unexpected keyword in the .net file\n");
	exit(0);
    }
}

return ;
}
