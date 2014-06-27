#include "custom.h"
extern int iwire, iwirex , iwirey , icost ;
extern int fwire, fwirex , fwirey , fcost ;

void twstats(void)
{

fprintf( fpo,"\nInitial Wiring Cost: %d   Final Wiring Cost: %d\n",
						icost , fcost ) ;
if( icost != 0 ) {
    fprintf(fpo,"############ Percent Wire Cost Reduction: %d\n\n",
	100 - (int)( (double)fcost / (double)icost * 100.0 ) ) ;
}

fprintf( fpo,"\nInitial Wire Length: %d   Final Wire Length: %d\n",
					    iwire, fwire ) ;
if( icost != 0 ) {
    fprintf(fpo,"*********** Percent Wire Length Reduction: %d\n\n",
	100 - (int)( (double) fwire / (double) iwire * 100.0 ) );
}

fprintf( fpo,"\nInitial Horiz. Wire: %d   Final Horiz. Wire: %d\n",
					    iwirex , fwirex ) ;
if( iwirex != 0 ) {
    fprintf(fpo,"$$$$$$$$$ Percent H-Wire Length Reduction: %d\n\n",
	100 - (int)( (double)fwirex / (double)iwirex * 100.0 ) ) ;
}
fprintf( fpo,"\nInitial Vert. Wire: %d   Final Vert. Wire: %d\n",
					    iwirey , fwirey ) ;
if( iwirey != 0 ) {
    fprintf(fpo,"@@@@@@@@@ Percent V-Wire Length Reduction: %d\n\n",
	100 - (int)( (double)fwirey / (double)iwirey * 100.0 ) ) ;
}

fprintf( fpo , "\nStatistics:\n");
fprintf( fpo , "Number of Cells: %d\n", numcells );
fprintf( fpo , "Number of Pads: %d\n", numpads );
fprintf( fpo , "Number of Nets: %d \n", numnets ) ;
fprintf( fpo , "Number of Pins: %d \n", maxterm ) ;

return ;
}
