/* curve.c
   Original code by rlk
   Additional code by Dave
   code cleanup by paxed
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

/*

  To compile this on unixen, do

    cc -lm curve.c -o curve

 */

#define MAX_HELP_TXTS 17

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int thickness;
int usesteps;
int totalstep;
int innerdrop, outerdrop, hill;
int ct;
int r2,r3;
int manual;
int seed;
int lots;
int iter;

char toptex[256];
char intex [256];
char outtex[256];
char bottex[256];
char blanktex[256];

const char *helptxt[MAX_HELP_TXTS] = {
    /* 0: nothing */ "",

    /* 1: inner radius */
    "   -The inner radius can be any number from zero on up.\n"
    "   -You might choose a value of 0, if you wanted to do a disc-shaped\n"
    "    platform, or filled circle (you could join all of the lumps\n"
    "    together in gtkradiant to keep the lump count low...)",

    /* 2: outer radius */
    "   -The outer radius can be any number as long as it is larger than\n"
    "    the inner radius.",

    /* 3: choice on radii */
    "   -You may specify an entirely different set of radii for the ending\n"
    "    portion of your curve. If you do so, it will be smoothly transitioned\n"
    "    between the beginning and the end.\n"
    "   -By using this feature, it is easy to create swirls, and spirals.",

    /* 4: ending inner radius */
    "   -The ending inner radius can be any number from zero on up.\n"
    "   -It can smaller than, larger than, or exactly equal to the\n"
    "    beginning inner radius.",

    /* 5: ending outer radius */
    "   -The ending outer radius can be any number as long as it is\n"
    "    larger than the ending inner radius.\n"
    "   -It can smaller than, larger than, or exactly equal to the\n"
    "    beginning outer radius.",

    /* 6: number of lumps */
    "   -The number of lumps will determine the \"coarseness\" of the curve.\n"
    "    Less lumps = coarser    -    More lumps = smoother\n"
    "    Be careful not to use too many lumps, though, as this can actually\n"
    "    lead to certain sloped curves being even more coarse due to the alignment\n"
    "    on the 1-unit grid.\n"
    "   -If you go into the \"advanced\" curve settings, the number of lumps you\n"
    "    specify in this step will be doubled, as each lump will be split into\n"
    "    two triangular sections. This can be desirable even when you are not going\n"
    "    to create a sloped/angled/hilled curve, if you plan on rotating your curve\n"
    "    in gtkradiant in some other fashion than the typical 90-degree angles.",

    /* 7: beginning angle */
    "   -The beginning angle is given in degrees and can be any value(+ or -).",

    /* 8: ending radius */
    "   -The ending angle is given in degrees and can be any value(+ or -).\n"
    "   -You can specify greater than 360 degrees, most useful for creating spirals-\n"
    "    simple \"snail-shell\"-style spirals, or sloped \"cork screw\"-style spirals.",

    /* 9: thickness */
    "   -Thickness can be any value greater than zero.\n"
    "   -If you plan on doing an angled curve(with inner or outer drop), or hill,\n"
    "    you should take into account how much drop or hill value you desire, as\n"
    "    that figure will be subtracted from the thickness you specify in this step.",

    /* 10: advanced or not? */
    "   -In the advanced settings, you can specify slope, inner an outer drop,\n"
    "    hill value, and whether or not to use constant thickness.\n"
    "   -Also, if you go into the advanced settings section of the questions, the\n"
    "    number of lumps will automatically double, as each lump will be split in\n"
    "    two triangular pieces.\n"
    "   -You do not have to use any of the advanced settings available if you\n"
    "    go into that section - you might just want the lumps split in two.\n"
    "    This is useful if you plan on rotating your curve in some other fashion\n"
    "    than the typical 90-degree angles.",

    /* 11: slope */
    "   -The slope of the curve specifies the number of vertical units total,\n"
    "    from the start to the end, that you wish the curve to ascend or descend.\n"
    "   -This number can be positive(ascend), negative(descend), or zero.\n"
    "   -Best results are achieved when the number of units specified in this step\n"
    "    is a multiple of the number of lumps (e.g. lumps = 8, slope = 64),\n"
    "    although it will still produce a very even slope if the numbers don't\n"
    "    have this relationship.\n"
    "   -To create a loop de loop, simply create a sloped, 360 degree curve where\n"
    "    the slope value is higher than the thickness of the curve.\n"
    "    Then rotate it on the x or y axis in gtkradiant.",

    /* 12: inner drop */
    "   -Inner drop must be at least one unit smaller than the thickness value.\n"
    "    It can also be zero.\n"
    "   -This determines how much to \"drop\" the inner, top portion of the curve.\n"
    "    This results in a curve which is angled inwards.\n"
    "   -You can combine this option with the \"hill\" option to get banked curves,\n"
    "    similar to those you might find at a race-track.",

    /* 13: outer drop */
    "   -Outer drop must be at least one unit smaller than the thickness value.\n"
    "    It can also be zero.\n"
    "   -This determines how much to \"drop\" the outer, top portion of the curve.\n"
    "    This results in a curve which is angled outwards.",

    /* 14: hill */
    "   -Hill must be at least one unit smaller than the thickness value.\n"
    "    It can also be zero.\n"
    "   -This figure determines how much to lower the ends of the curve. They will\n"
    "    rise from the ends toward the middle, in a smooth sine-wave shaped hill.\n"
    "   -This can be combined with the inner or outer drop option, creating a \"hill\"\n"
    "    which rises on only one side. In this case, it is best to use the same\n"
    "    value for \"hill\" as you did for inner or outer drop.",

    /* 15: constant thickness */
    "   -By default, the curve generator will attempt to give a constant thickness\n"
    "    in the lumps, by raising or lowering opposite corners by similar amounts.\n"
    "   -By deselecting this feature, the bottom portions of the lump will not be\n"
    "    adjusted, which can be desirable for variety of reasons, which is why\n"
    "    this option is available.\n"
    "   -The best way to understand how this feature works is to create either\n"
    "    an angled(inner or outer drop) or hill-shaped curve, and do one with\n"
    "    constant thickness on, and then another one with it turned off.",

    /* 16: filename */
    "   -It is recommended to end your filename with \".map\", so that you can\n"
    "    import immediately into gtkradiant. You can, of course, rename the file\n"
    "    manually after it is created.\n"
    "   -Do not include spaces when entering the filename, or your filename will\n"
    "    only be that which occurs before the first space.\n"
    "   -If you specify a filename which already exists, it will be overwritten.\n"
    "    Be aware that if you have already imported that file into gtkradiant,\n"
    "    you will either need to rename the file, or close and re-open your map,\n"
    "    in order to avoid the \"cached\" version of the file used by gtkradiant."
};

void showhelp(int which)
{
    if (which > 0 && which < MAX_HELP_TXTS)
	printf("\n%s\n\n", helptxt[which]);
}

void showusage(FILE *fp, char *fn)
{
    fprintf(fp, "Usage: %s <r0> <r1> <n> <a0> <a1> <t> <r2> <r3> <s> <id> <od> <h> <ct> \n"
                        "r0 - inner radius\n"
                        "r1 - outer radius\n"
                        "n  - number of lumps (will be doubled if sloped,angled,or hill-shaped)\n"
                        "a0 - beginning angle\n"
                        "a1 - ending angle\n"
                        "t  - thickness of curve\n"
                        "r2 - ending inner radius(can be >,<, or = r0)\n"
                        "r3 - ending outer radius(can be >,<, or = r1)\n"
                        "s  - vertical slope from beginning to end\n"
                        "id - vertical drop of curve's inner radius(inward angled)\n"
                        "od - vertical drop of curve's outer radius(outward angled)\n"
                        "h  - vertical drop for both ends of curve (hill)\n"
                        "ct - zero will override default of constant thickness for angle/hill curves\n",fn );

    return ;
}

int
get_input_num(char *name, int defaultval, int helpnum, int minval, int maxval, int *retval)
{
    char str[80];
    int i;

    for (;;) {
	printf("\nPlease enter %s (default = %d) -> ", name, defaultval);
	scanf("%s", str);

	if (str[0] == 'x') return 1;

	if (str[0] == '?') showhelp(helpnum);
	else if (isdigit(str[0]) || (str[0] == '-') || str[0] == 'd') {
	    if (str[0] == 'd') i = defaultval;
	    else i = atoi(str);
	    if (i < minval || i > maxval) printf("Please enter \"?\", \"x\", \"d\" or an integer value between %d and %d.", minval, maxval);
	    else {
		*retval = i;
		return 0;
	    }
	} else printf("Please enter \"?\", \"x\", \"d\" or an integer value between %d and %d.", minval, maxval);
    }
}

char
get_input_yn(char *question, char defval, int helpnum)
{
    char str[80];

    for (;;) {
	printf("\n%s? (y/n/?) -> ", question);
	scanf("%s", str);

	if (str[0] == '?') showhelp(helpnum);
	else if (str[0] == 'y' || str[0] == 'n') return str[0];
	else return defval;
    }
}

static int rndnum(int low, int high)
{
    if (seed == 0)
    {
        srand((unsigned int) time(NULL));
        seed = 1;
    }

    return (rand() % ((high-low) +1)) + low;
}

static void side(double x0, double y0, double z0,
                 double x1, double y1, double z1,
                 double x2, double y2, double z2, char *tex)
{
    double modx, mody, modz; /*for doing a grid of curves*/
    modx = 0;
    mody = 0;
    modz = 0;

    if (lots > 1)
    {
        int row = iter / 4;
        int col = iter % 4;
        modz = ((row + col) % 2) * 256.0;
        modx = row * 768.0;
        mody = col * 768.0;
    }
    printf("( %f %f %f ) ( %f %f %f ) ( %f %f %f ) "
           "%s 0 0 0 0.500000 0.500000 0 0 0\n",
           x0+modx, y0+mody, z0+modz,
           x1+modx, y1+mody, z1+modz,
           x2+modx, y2+mody, z2+modz, tex);
}

static void lump(int r0, int r1, double a0, double a1, int i, int n)
{
    double x00 = (r0+((double)(r2-r0)*((i)/(double)n))) * cos(M_PI * a0 / 180.0);
    double y00 = (r0+((double)(r2-r0)*((i)/(double)n))) * sin(M_PI * a0 / 180.0);

    double x10 = (r1+((double)(r3-r1)*((i)/(double)n))) * cos(M_PI * a0 / 180.0);
    double y10 = (r1+((double)(r3-r1)*((i)/(double)n))) * sin(M_PI * a0 / 180.0);

    double x01 = (r0+((double)(r2-r0)*((i+1.0f)/(double)n))) * cos(M_PI * a1 / 180.0);
    double y01 = (r0+((double)(r2-r0)*((i+1.0f)/(double)n))) * sin(M_PI * a1 / 180.0);

    double x11 = (r1+((double)(r3-r1)*((i+1.0f)/(double)n))) * cos(M_PI * a1 / 180.0);
    double y11 = (r1+((double)(r3-r1)*((i+1.0f)/(double)n))) * sin(M_PI * a1 / 180.0);

    double z0  = 0;
    double z1  = thickness;

    printf("{\n");

    if (!usesteps)
    {
        /*just use rlk's code    */
        side(x00, y00, z0, x01, y01, z0, x00, y00, z1, intex);
        side(x10, y10, z1, x11, y11, z1, x10, y10, z0, outtex);
        side(x00, y00, z1, x10, y10, z1, x00, y00, z0, blanktex);
        side(x01, y01, z0, x11, y11, z0, x01, y01, z1, blanktex);

        side(0, 0, z0, 1, 0, z0, 0, 1, z0, bottex);
        side(0, 0, z1, 0, 1, z1, 1, 0, z1, toptex);
    }
    else
    {
        /*use Dave's code! */
        double stepsize = totalstep/(double)n;
        double zmod0=(double)i * stepsize;
        double zmod1=(double)(i+1) * stepsize; /*this goes up!  */
        double hillmodinside=0,hillmodoutside=0;
        double hmi2=0,hmo2=0;
        double cthi1=0,cthi2=0,ctho1=0,ctho2=0;

        if (hill != 0)
        {
            /*do one based on sinewave. */
            double mult1=((sin((((360.0f/(double)n)*(double)i)-90.0) * M_PI / 180.0)+1.0)/2.0);
            double mult2=((sin((((360.0f/(double)n)*(double)(i+1))-90.0) * M_PI / 180.0)+1.0)/2.0);
            if (innerdrop>=outerdrop)
            {
                /*then it's the outside needs altered   */
                hillmodoutside=hill-(mult1*(double)hill) ;
                hmo2=hill-(mult2*(double)hill) ;
                ctho1=hill-hillmodoutside;
                ctho2=hill-hmo2;
            }
            if (outerdrop>=innerdrop)
            {
                hillmodinside=hill-(mult1*(double)hill) ;
                hmi2=hill-(mult2*(double)hill) ;
                cthi1=hill-hillmodinside;
                cthi2=hill-hmi2;
            }
               /*end of sinewave hill code*/
            if (innerdrop>outerdrop)
            {
                cthi1-=hillmodoutside; cthi2-=hmo2;
                ctho1=0; ctho2=0;
            }
             if (outerdrop>innerdrop )
                         {ctho1-=hillmodinside; ctho2-=hmi2;
                          cthi1=0; cthi2=0; }
        } /*end of "hill" code */

        /*
            00 is bottom left
            10 is bottom right
            11 is top right
            01 is top left
        */

        side(x11, y11, ((z1+zmod1)-outerdrop)-hmo2,             x10, y10, ((z1+zmod0)-outerdrop)-hillmodoutside, x00, y00, ((z1+zmod0)-innerdrop)-hillmodinside, toptex);
        side(x00, y00, z0+zmod0+((outerdrop+ctho1)*ct),         x10, y10, z0+zmod0+((innerdrop+cthi1)*ct),      x11, y11, z0+zmod1+((innerdrop+cthi2)*ct),       bottex);
        side(x11, y11, ((z1+zmod1)-outerdrop)-hmo2,             x11, y11, z0+zmod1+((innerdrop+cthi2)*ct),      x10, y10, z0+zmod0+((innerdrop+cthi1)*ct),       outtex);
        side(x10, y10, ((z1+zmod0)-outerdrop)-hillmodoutside,   x10, y10, z0+zmod0+((innerdrop+cthi1)*ct),      x00, y00, z0+zmod0+((outerdrop+ctho1)*ct),       blanktex);
        side(x11, y11, ((z1+zmod1)-outerdrop)-hmo2,             x00, y00, z0+zmod0+((outerdrop+ctho1)*ct),      x11, y11, z0+zmod1+((innerdrop+cthi2)*ct),       blanktex);

        printf("}\n{\n"); /*next one...   */

        side(x00, y00, ((z1+zmod0)-innerdrop)-hillmodinside,    x01, y01, ((z1+zmod1)-innerdrop)-hmi2,          x11, y11, ((z1+zmod1)-outerdrop)-hmo2,           toptex);
        side(x11, y11, z0+zmod1+((innerdrop+cthi2)*ct),         x01, y01, z0+zmod1+((outerdrop+ctho2)*ct),      x00, y00, z0+zmod0+((outerdrop+ctho1)*ct),       bottex);
        side(x00, y00, ((z1+zmod0)-innerdrop)-hillmodinside,    x00, y00, z0+zmod0+((outerdrop+ctho1)*ct),      x01, y01, z0+zmod1+((outerdrop+ctho2)*ct),       intex);
        side(x01, y01, ((z1+zmod1)-innerdrop)-hmi2,             x01, y01, z0+zmod1+((outerdrop+ctho2)*ct),      x11, y11, z0+zmod1+((innerdrop+cthi2)*ct),       blanktex);
        side(x00, y00, ((z1+zmod0)-innerdrop)-hillmodinside,    x11, y11, z0+zmod1+((innerdrop+cthi2)*ct),      x00, y00, z0+zmod0+((outerdrop+ctho1)*ct),       blanktex);

         /*end of Dave's code! */
    }

    printf("}\n");
}

void gettextures(void)
{
    /*default = mtrl/invisible*/
    FILE *fp;
    char c1[256],c2[256];

    strcpy(toptex,"mtrl/invisible");
    strcpy(bottex,"mtrl/invisible");
    strcpy(intex, "mtrl/invisible");
    strcpy(outtex,"mtrl/invisible");
    strcpy(blanktex,"mtrl/invisible");

    fp = fopen("textures.txt","r");
    if (fp)
    {
        int i;
        int huh;
        for (i = 0; i<4; i++)
        {
            huh = fscanf(fp,"%s %s",c1,c2);
            if (huh == 2)
            {
                if (strcmp(c1,"TOP") == 0) sprintf(toptex,"mtrl/%s",c2);
                if (strcmp(c1,"BOT") == 0) sprintf(bottex,"mtrl/%s",c2);
                if (strcmp(c1,"IN") == 0)  sprintf(intex, "mtrl/%s",c2);
                if (strcmp(c1,"OUT") == 0) sprintf(outtex,"mtrl/%s",c2);
            }
            else
                break;
        }
        fclose(fp);
    }
}

int main(int argc, char *argv[])
{
    int i     = 0 ;
    int r0    = 128;
    int r1    = 256;
    int n     = 8;
    int a0    = 0;
    int a1    = 90;
    thickness = 16;
    usesteps  = 0;
    totalstep = 0;
    innerdrop = 0;
    outerdrop = 0;
    hill      = 0;
    ct        = 1;
    r2        = 128;
    r3        = 256;
	manual    = 0;

    lots      = 0;
    seed      = 0;
    iter      = 0;

    if (argc > 1 && strcmp(argv[1], "-?") == 0)
    {
        showusage(stderr, argv[0]);
        fprintf(stderr, "\nOther ways to use this program :\n"
                        "%s -? : This help screen.\n"
                        "%s -readme   : creates a readme.txt file with detailed help information.\n"
                        "%s -textures : creates a textures.txt, editable for auto-texture placement.\n"
                        "%s -random   : creates a random curve; add -lots for a grid of 16 curves.\n"
                        "%s : (no parameters) runs a basic user-interface to aid your curve creation.\n",
                        argv[0],argv[0],argv[0],argv[0],argv[0]);
    }
    else if (argc > 1 && strcmp(argv[1], "-readme") == 0)
    {
        fprintf(stderr, "Detailed information about each setting has been placed in readme.txt\n");
        freopen("readme.txt", "w" ,stdout);
        showusage(stdout,argv[0]);
        for ( i=0; i<MAX_HELP_TXTS; i++) showhelp(i);
    }
    else if (argc > 1 && strcmp(argv[1], "-textures") == 0)
    {
        FILE *f;
        fprintf(stderr, "textures.txt can be edited to specify the textures that you wish to apply to\n"
                        "the top, bottom, inside, and outside of the subsequently generated curves.");
        f = fopen("textures.txt", "w");
        fprintf(f,"TOP invisible\n"
                  "BOT invisible\n"
                  "IN invisible\n"
                  "OUT invisible");
        fclose(f);
    }
    else
    {
        if (argc == 1)
        {
            /*Engage the so called user-interface... */
            char str[80];
            int okay = 0;
	    int retval = 0;
	    char ynchar;
            manual = 1;

            printf("During the input process, typing a lowercase \"x\" and pressing Enter will\n");
	    printf("  accept the default for that question and all of the remaining questions,\n"
		   "  and proceed to the file output step.\n");
	    printf("Typing \"d\" and Enter will accept the default for that particular question.\n");
	    printf("Typing a \"?\" and pressing Enter will give you a description of that variable.\n\n");

	    okay = get_input_num("INNER RADIUS", r0, 1, 0, 65536, &retval);
	    if (okay) goto getfilename;
	    r0 = retval; r2 = r0;

	    okay = get_input_num("OUTER RADIUS", r1, 2, r0+1, 65535, &retval);
	    if (okay) goto getfilename;
	    r1 = retval; r3 = r1;

	    ynchar = get_input_yn("Do you wish to have the same radii from beginning to end", 'y', 3);
	    if (ynchar == 'n') {
		okay = get_input_num("ENDING INNER RADIUS", r2, 4, 0, 65535, &retval);
		if (okay) goto getfilename;
		r2 = retval;

		okay = get_input_num("ENDING OUTER RADIUS", r3, 5, r2+1, 65535, &retval);
		if (okay) goto getfilename;
		r3 = retval;
	    }

	    okay = get_input_num("NUMBER OF LUMPS", n, 6, 1, 65535, &retval);
	    if (okay) goto getfilename;
	    n = retval;

	    okay = get_input_num("BEGINNING ANGLE", a0, 7, -65535, 65535, &retval);
	    if (okay) goto getfilename;
	    a0 = retval;

	    okay = get_input_num("ENDING ANGLE", a1, 8, -65535, 65535, &retval);
	    if (okay) goto getfilename;
	    a1 = retval;

	    okay = get_input_num("THICKNESS", thickness, 9, 1, 65536, &retval);
	    if (okay) goto getfilename;
	    thickness = retval;

	    ynchar = get_input_yn("Do you wish to use any advanced curve settings", 'n', 10);
	    if (ynchar == 'n') goto getfilename;

	    okay = get_input_num("SLOPE", totalstep, 11, -65535, 65535, &retval);
	    usesteps = 1;
	    if (okay) goto getfilename;
	    totalstep = retval;

	    okay = get_input_num("INNER DROP", innerdrop, 12, 0, thickness - 1, &retval);
	    if (okay) goto getfilename;
	    innerdrop = retval;

	    okay = get_input_num("OUTER DROP", outerdrop, 13, 0, thickness - 1, &retval);
	    if (okay) goto getfilename;
	    outerdrop = retval;

	    okay = get_input_num("HILL", hill, 14, 0, thickness - 1, &retval);
	    if (okay) goto getfilename;
	    hill = retval;

	    ynchar = get_input_yn("Would you like to use the CONSTANT THICKNESS feature", 'y', 15);
	    ct = (ynchar == 'y') ? 1 : 0;

getfilename:

	    do {
		okay=0;
		printf("\nPlease enter filename for your curve -> ");
		scanf("%s",str);
		if (str[0]=='?')
		    showhelp(16);
		else
		    okay=1; /*accept what they typed...  */
	    } while (okay!=1);

	    freopen(str, "w", stdout); /*setup stdout redirect */

        } /* end of interactive user input */
        else
        {
            if (strcmp(argv[1],"-random") != 0)
            { /* use the command line inputs...  */
                if (argc > 1)
                {
                    r0 = atoi(argv[1]); r2=r0;
                }
                if (argc > 2)
                {
                    r1 = atoi(argv[2]); r3=r1;
                }
                if (argc > 3)
                    n  = atoi(argv[3]);
                if (argc > 4)
                    a0 = atoi(argv[4]);
                if (argc > 5)
                    a1 = atoi(argv[5]);
                if (argc > 6)
                    thickness = atoi(argv[6]);
                if (argc > 7)
                    r2=atoi(argv[7]);
                if (argc > 8)
                    r3=atoi(argv[8]);
                if (argc > 9)
                {   /*can specify 0, for segmentation, but no slope.  */
                    totalstep = atoi(argv[9]); usesteps=1;
                }
                if (argc >10)
                    innerdrop= atoi(argv[10]);
                if (argc >11)
                    outerdrop= atoi(argv[11]);
                if (argc >12)
                    hill= atoi(argv[12]);
                if (argc >13)
                    ct=atoi(argv[13]); /*any non-zero number keeps curves constant thickness.. */
                if (ct!=0)
                    ct=1;
            }
            else
            {
                /*it's the random curve generator */
                lots = 1; /*just an indicator that we need a random number*/
                if ((argc > 2) && (strcmp(argv[2],"-lots")==0))
                    lots = 16;
            }

        } /* end of command line input  */

        do
        {
            if (lots > 0)
            {
                r0 = rndnum(0, 256/8) * 8; r2=r0;

                r1 = r0 + rndnum(8/8, 256/8) * 8; r3=r1;

                if (rndnum(1,100)<=40)
                    r2 = rndnum(0,256/8) * 8;
                if (rndnum(1,100)<=40)
                    r3 = r2 + rndnum(8/8,256/8) * 8;

                a0 = rndnum(0,7) * 45;
                a1 = a0 + (rndnum(2,16) * 45);

                n = (a1-a0) / 12;

                if (rndnum(1,100)<=50)
                    thickness = rndnum(8/8,64/8) * 8;
                else
                    thickness = rndnum(1,2) * 128;

                if (rndnum(1,100)<=40)
                    totalstep = rndnum(1,16) * 32;
                else
                    totalstep = 0;

                innerdrop = 0;
                outerdrop = 0;
                if (rndnum(1,100)<=40)
                {
                    if (rndnum(1,100)<=50)
                    {
                        if (thickness>8)
                        {
                            innerdrop = rndnum(1,(thickness - 8)/8) * 8;
                            usesteps = 1;
                        }
                    }
                    else
                    {
                        if (thickness>8)
                        {
                            outerdrop = rndnum(1,(thickness - 8)/8) * 8;
                            usesteps = 1;
                        }
                    }
                }

                hill = 0;
                if (rndnum(1,100)<=40)
                {
                    if (thickness>16)
                    {
                        hill = rndnum(2,(thickness - 16)/8) * 8;
                        usesteps = 1;
                    }
                }

                ct = 1;
                if (rndnum(1,100)<=25)
                    ct=0;
            }

            printf("{\n");
            printf("\"classname\" \"worldspawn\"\n");
            printf("// This curve was created with the following parameters :\n"
                   "// curve %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
                   r0,r1,n,a0,a1,thickness,r2,r3,totalstep,innerdrop,outerdrop,hill,ct);
            gettextures();
            for (i = 0; i < n; i++)
            {
                double ai0 = (double) (i    ) * (a1 - a0) / n + a0;
                double ai1 = (double) (i + 1) * (a1 - a0) / n + a0;

                lump(r0, r1, ai0, ai1, i, n);
            }
            printf("}\n");

            iter += 1;
        } while (iter < lots);
    }
    return 0;
}
