#ifdef LAMEPARSE

#include "util.h"
#include "id3tag.h"
#include "get_audio.h"
#include "brhist.h"
#include "version.h"



#define         MAX_NAME_SIZE           300
  char    inPath[MAX_NAME_SIZE];
  char    outPath[MAX_NAME_SIZE];


/************************************************************************
*
* usage
*
* PURPOSE:  Writes command line syntax to the file specified by #stderr#
*
************************************************************************/

void lame_usage(lame_global_flags *gfp,char *name)  /* print syntax & exit */
{
  lame_print_version(stderr);
  fprintf(stderr,"\n");
  fprintf(stderr,"USAGE   :  %s [options] <infile> [outfile]\n",name);
  fprintf(stderr,"\n<infile> and/or <outfile> can be \"-\", which means stdin/stdout.\n");
  fprintf(stderr,"\n");
  fprintf(stderr,"Try \"%s --help\" for more information\n",name);
  exit(1);
}



/************************************************************************
*
* usage
*
* PURPOSE:  Writes command line syntax to the file specified by #stdout#
*
************************************************************************/

void lame_help(lame_global_flags *gfp,char *name)  /* print syntax & exit */
{
  lame_print_version(stdout);
  fprintf(stdout,"\n");
  fprintf(stdout,"USAGE   :  %s [options] <infile> [outfile]\n",name);
  fprintf(stdout,"\n<infile> and/or <outfile> can be \"-\", which means stdin/stdout.\n");
  fprintf(stdout,"\n");
  fprintf(stdout,"OPTIONS :\n");
  fprintf(stdout,"  Input options:\n");
  fprintf(stdout,"    -r              input is raw pcm\n");
  fprintf(stdout,"    -x              force byte-swapping of input\n");
  fprintf(stdout,"    -s sfreq        sampling frequency of input file(kHz) - default 44.1kHz\n");
  fprintf(stdout,"    --mp3input      input file is a MP3 file\n");
  fprintf(stdout,"\n");
  fprintf(stdout,"  Filter options:\n");
  fprintf(stdout,"    -k              keep ALL frequencies (disables all filters)\n");
  fprintf(stdout,"  --lowpass freq         frequency(kHz), lowpass filter cutoff above freq\n");
  fprintf(stdout,"  --lowpass-width freq   frequency(kHz) - default 15%% of lowpass freq\n");
  fprintf(stdout,"  --highpass freq        frequency(kHz), highpass filter cutoff below freq\n");
  fprintf(stdout,"  --highpass-width freq  frequency(kHz) - default 15%% of highpass freq\n");
  fprintf(stdout,"  --resample sfreq  sampling frequency of output file(kHz)- default=input sfreq\n");
  fprintf(stdout,"  --cwlimit freq    compute tonality up to freq (in kHz) default 8.8717\n");
  fprintf(stdout,"\n");
  fprintf(stdout,"  Operational options:\n");
  fprintf(stdout,"    -m mode         (s)tereo, (j)oint, (f)orce or (m)ono  (default j)\n");
  fprintf(stdout,"                    force = force ms_stereo on all frames. Faster\n");
  fprintf(stdout,"    -a              downmix from stereo to mono file for mono encoding\n");
  fprintf(stdout,"    -d              allow channels to have different blocktypes\n");
  fprintf(stdout,"    -S              don't print progress report, VBR histograms\n");
  fprintf(stdout,"    --athonly       only use the ATH for masking\n");
  fprintf(stdout,"    --noath         disable the ATH for masking\n");
  fprintf(stdout,"    --noshort       do not use short blocks\n");
  fprintf(stdout,"    --voice         experimental voice mode\n");
  fprintf(stdout,"    --preset type   type must be phone, voice, fm, tape, hifi, cd or studio\n");
  fprintf(stdout,"                    help gives some more infos on these\n");
  fprintf(stdout,"\n");
  fprintf(stdout,"  CBR (constant bitrate, the default) options:\n");
  fprintf(stdout,"    -h              higher quality, but a little slower.  Recommended.\n");
  fprintf(stdout,"    -f              fast mode (very low quality)\n");
  fprintf(stdout,"    -b bitrate      set the bitrate, default 128kbps\n");
  fprintf(stdout,"\n");
  fprintf(stdout,"  VBR options:\n");
  fprintf(stdout,"    -v              use variable bitrate (VBR)\n");
  fprintf(stdout,"    -V n            quality setting for VBR.  default n=%i\n",gfp->VBR_q);
  fprintf(stdout,"                    0=high quality,bigger files. 9=smaller files\n");
  fprintf(stdout,"    -b bitrate      specify minimum allowed bitrate, default 32kbs\n");
  fprintf(stdout,"    -B bitrate      specify maximum allowed bitrate, default 256kbs\n");
  fprintf(stdout,"    -t              disable Xing VBR informational tag\n");
  fprintf(stdout,"    --nohist        disable VBR histogram display\n");
  fprintf(stdout,"\n");
  fprintf(stdout,"  MP3 header/stream options:\n");
  fprintf(stdout,"    -e emp          de-emphasis n/5/c  (obsolete)\n");
  fprintf(stdout,"    -c              mark as copyright\n");
  fprintf(stdout,"    -o              mark as non-original\n");
  fprintf(stdout,"    -p              error protection.  adds 16bit checksum to every frame\n");
  fprintf(stdout,"                    (the checksum is computed correctly)\n");
  fprintf(stdout,"    --nores         disable the bit reservoir\n");
  fprintf(stdout,"\n");
  fprintf(stdout,"  Specifying any of the following options will add an ID3 tag:\n");
  fprintf(stdout,"     --tt \"title\"     title of song (max 30 chars)\n");
  fprintf(stdout,"     --ta \"artist\"    artist who did the song (max 30 chars)\n");
  fprintf(stdout,"     --tl \"album\"     album where it came from (max 30 chars)\n");
  fprintf(stdout,"     --ty \"year\"      year in which the song/album was made (max 4 chars)\n");
  fprintf(stdout,"     --tc \"comment\"   additional info (max 30 chars)\n");
  fprintf(stdout,"                      (or max 28 chars if using the \"track\" option)\n");
  fprintf(stdout,"     --tn \"track\"     track number of the song on the CD (1 to 99)\n");
  fprintf(stdout,"                      (using this option will add an ID3v1.1 tag)\n");
  fprintf(stdout,"     --tg \"genre\"     genre of song (name or number)\n");
  fprintf(stdout,"\n");
#ifdef HAVEGTK
  fprintf(stdout,"    -g              run graphical analysis on <infile>\n");
#endif
  display_bitrates(stdout);
  exit(0);
}



/************************************************************************
*
* usage
*
* PURPOSE:  Writes presetting info to #stdout#
*
************************************************************************/

void lame_presets_info(lame_global_flags *gfp,char *name)  /* print syntax & exit */
{
  lame_print_version(stdout);
  fprintf(stdout,"\n");
  fprintf(stdout,"Presets are some shortcuts for common settings.\n");
  fprintf(stdout,"They can be combined with -v if you want VBR MP3s.\n");
  fprintf(stdout,"\n");
  fprintf(stdout,"  --preset phone    =>  --resample      16\n");
  fprintf(stdout,"                        --highpass       0.260\n");
  fprintf(stdout,"                        --highpasswidth  0.040\n");
  fprintf(stdout,"                        --lowpass        3.700\n");
  fprintf(stdout,"                        --lowpasswidth   0.300\n");
  fprintf(stdout,"                        --noshort\n");
  fprintf(stdout,"                        -m   m\n");
  fprintf(stdout,"                        -b  16\n");
  fprintf(stdout,"                  plus  -b   8  \\\n");
  fprintf(stdout,"                        -B  56   > in combination with -v\n");
  fprintf(stdout,"                        -V   5  /\n");
  fprintf(stdout,"\n");
  fprintf(stdout,"  --preset voice:   =>  --resample      24\n");
  fprintf(stdout,"                        --highpass       0.100\n");
  fprintf(stdout,"                        --highpasswidth  0.020\n");
  fprintf(stdout,"                        --lowpass       11\n");
  fprintf(stdout,"                        --lowpasswidth   2\n");
  fprintf(stdout,"                        --noshort\n");
  fprintf(stdout,"                        -m   m\n");
  fprintf(stdout,"                        -b  32\n");
  fprintf(stdout,"                  plus  -b   8  \\\n");
  fprintf(stdout,"                        -B  96   > in combination with -v\n");
  fprintf(stdout,"                        -V   4  /\n");
  fprintf(stdout,"\n");
  fprintf(stdout,"  --preset fm:      =>  --resample      32\n");
  fprintf(stdout,"                        --highpass       0.030\n");
  fprintf(stdout,"                        --highpasswidth  0\n");
  fprintf(stdout,"                        --lowpass       11.4\n");
  fprintf(stdout,"                        --lowpasswidth   0\n");
  fprintf(stdout,"                        -m   j\n");
  fprintf(stdout,"                        -b  96\n");
  fprintf(stdout,"                  plus  -b  32  \\\n");
  fprintf(stdout,"                        -B 192   > in combination with -v\n");
  fprintf(stdout,"                        -V   4  /\n");
  fprintf(stdout,"\n");
  fprintf(stdout,"  --preset tape:    =>  --lowpass       17\n");
  fprintf(stdout,"                        --lowpasswidth   2\n");
  fprintf(stdout,"                        --highpass       0.015\n");
  fprintf(stdout,"                        --highpasswidth  0.015\n");
  fprintf(stdout,"                        -m   j\n");
  fprintf(stdout,"                        -b 128\n");
  fprintf(stdout,"                  plus  -b  32  \\\n");
  fprintf(stdout,"                        -B 192   > in combination with -v\n");
  fprintf(stdout,"                        -V   4  /\n");
  fprintf(stdout,"\n");
  fprintf(stdout,"  --preset hifi:    =>  --lowpass       20\n");
  fprintf(stdout,"                        --lowpasswidth   3\n");
  fprintf(stdout,"                        --highpass       0.015\n");
  fprintf(stdout,"                        --highpasswidth  0.015\n");
  fprintf(stdout,"                        -h\n");
  fprintf(stdout,"                        -m   j\n");
  fprintf(stdout,"                        -b 160\n");
  fprintf(stdout,"                  plus  -b  32  \\\n");
  fprintf(stdout,"                        -B 224   > in combination with -v\n");
  fprintf(stdout,"                        -V   3  /\n");
  fprintf(stdout,"\n");
  fprintf(stdout,"  --preset cd:      =>  -k\n");
  fprintf(stdout,"                        -h\n");
  fprintf(stdout,"                        -m   s\n");
  fprintf(stdout,"                        -b 192\n");
  fprintf(stdout,"                  plus  -b  80  \\\n");
  fprintf(stdout,"                        -B 256   > in combination with -v\n");
  fprintf(stdout,"                        -V   2  /\n");
  fprintf(stdout,"\n");
  fprintf(stdout,"  --preset studio:  =>  -k\n");
  fprintf(stdout,"                        -h\n");
  fprintf(stdout,"                        -m   s\n");
  fprintf(stdout,"                        -b 256\n");
  fprintf(stdout,"                  plus  -b 112  \\\n");
  fprintf(stdout,"                        -B 320   > in combination with -v\n");
  fprintf(stdout,"                        -V   0  /\n");
  fprintf(stdout,"\n");

  exit(0);
}



/************************************************************************
*
* parse_args
*
* PURPOSE:  Sets encoding parameters to the specifications of the
* command line.  Default settings are used for parameters
* not specified in the command line.
*
* If the input file is in WAVE or AIFF format, the sampling frequency is read
* from the AIFF header.
*
* The input and output filenames are read into #inpath# and #outpath#.
*
************************************************************************/
void lame_parse_args(lame_global_flags *gfp,int argc, char **argv)
{
  FLOAT srate;
  int   err = 0, i = 0;
  int autoconvert=0;
  int user_quality=0;

  char *programName = argv[0]; 
  int track = 0;

  inPath[0] = '\0';   
  outPath[0] = '\0';
  gfp->inPath=inPath;
  gfp->outPath=outPath;

  id3_inittag(&id3tag);
  id3tag.used = 0;

  /* process args */
  while(++i<argc && err == 0) {
    char c, *token, *arg, *nextArg;
    int  argUsed;
    
    token = argv[i];
    if(*token++ == '-') {
      if(i+1 < argc) nextArg = argv[i+1];
      else           nextArg = "";
      argUsed = 0;
      if (! *token) {
	/* The user wants to use stdin and/or stdout. */
	if(inPath[0] == '\0')       strncpy(inPath, argv[i],MAX_NAME_SIZE);
	else if(outPath[0] == '\0') strncpy(outPath, argv[i],MAX_NAME_SIZE);
      } 
      if (*token == '-') {
	/* GNU style */
	token++;

	if (strcmp(token, "resample")==0) {
	  argUsed=1;
	  srate = atof( nextArg );
	  /* samplerate = rint( 1000.0 * srate ); $A  */
	  gfp->out_samplerate =  (( 1000.0 * srate ) + 0.5);
	  if (srate  < 1) {
	    fprintf(stderr,"Must specify a samplerate with --resample\n");
	    exit(1);
	  }
	}
	else if (strcmp(token, "mp3input")==0) {
	  gfp->input_format=sf_mp3;
	}
	else if (strcmp(token, "voice")==0) {
	  gfp->lowpassfreq=12000;
	  gfp->VBR_max_bitrate_kbps=160;
	  gfp->no_short_blocks=1;
	}
	else if (strcmp(token, "noshort")==0) {
	  gfp->no_short_blocks=1;
	}
	else if (strcmp(token, "noath")==0) {
	  gfp->noATH=1;
	}
	else if (strcmp(token, "nores")==0) {
	  gfp->disable_reservoir=1;
	  gfp->padding=0;
	}
	else if (strcmp(token, "athonly")==0) {
	  gfp->ATHonly=1;
	}
	else if (strcmp(token, "nohist")==0) {
#ifdef BRHIST
	  disp_brhist = 0;
#endif
	}
	/* options for ID3 tag */
 	else if (strcmp(token, "tt")==0) {
 		id3tag.used=1;      argUsed = 1;
  		strncpy(id3tag.title, nextArg, 30);
 		}
 	else if (strcmp(token, "ta")==0) {
 		id3tag.used=1; argUsed = 1;
  		strncpy(id3tag.artist, nextArg, 30);
 		}
 	else if (strcmp(token, "tl")==0) {
 		id3tag.used=1; argUsed = 1;
  		strncpy(id3tag.album, nextArg, 30);
 		}
 	else if (strcmp(token, "ty")==0) {
 		id3tag.used=1; argUsed = 1;
  		strncpy(id3tag.year, nextArg, 4);
 		}
 	else if (strcmp(token, "tc")==0) {
 		id3tag.used=1; argUsed = 1;
  		strncpy(id3tag.comment, nextArg, 30);
 		}
 	else if (strcmp(token, "tn")==0) {
 		id3tag.used=1; argUsed = 1;
  		track = atoi(nextArg);
  		if (track < 1) { track = 1; }
  		if (track > 99) { track = 99; }
  		id3tag.track = track;
 		}
 	else if (strcmp(token, "tg")==0) {
		argUsed = strtol (nextArg, &token, 10);
		if (nextArg==token) {
		  /* Genere was given as a string, so it's number*/
		  for (argUsed=0; argUsed<=genre_last; argUsed++) {
		    if (!strcmp (genre_list[argUsed], nextArg)) { break; }
		  }
 		}
		if (argUsed>genre_last) { 
		  argUsed=255; 
		  fprintf(stderr,"Unknown genre: %s.  Specifiy genre number \n", nextArg);
		}
	        argUsed &= 255; c=(char)(argUsed);

 		id3tag.used=1; argUsed = 1;
  		strncpy(id3tag.genre, &c, 1);
	       }
	else if (strcmp(token, "lowpass")==0) {
	  argUsed=1;
	  gfp->lowpassfreq =  (( 1000.0 * atof( nextArg ) ) + 0.5);
	  if (gfp->lowpassfreq  < 1) {
	    fprintf(stderr,"Must specify lowpass with --lowpass freq, freq >= 0.001 kHz\n");
	    exit(1);
	  }
	}
	else if (strcmp(token, "lowpass-width")==0) {
	  argUsed=1;
	  gfp->lowpasswidth =  (( 1000.0 * atof( nextArg ) ) + 0.5);
	  if (gfp->lowpasswidth  < 0) {
	    fprintf(stderr,"Must specify lowpass width with --lowpass-width freq, freq >= 0 kHz\n");
	    exit(1);
	  }
	}
	else if (strcmp(token, "highpass")==0) {
	  argUsed=1;
	  gfp->highpassfreq =  (( 1000.0 * atof( nextArg ) ) + 0.5);
	  if (gfp->highpassfreq  < 1) {
	    fprintf(stderr,"Must specify highpass with --highpass freq, freq >= 0.001 kHz\n");
	    exit(1);
	  }
	}
	else if (strcmp(token, "highpass-width")==0) {
	  argUsed=1;
	  gfp->highpasswidth =  (( 1000.0 * atof( nextArg ) ) + 0.5);
	  if (gfp->highpasswidth  < 0) {
	    fprintf(stderr,"Must specify highpass width with --highpass-width freq, freq >= 0 kHz\n");
	    exit(1);
	  }
	}
	else if (strcmp(token, "cwlimit")==0) {
	  argUsed=1;
	  gfp->cwlimit =  atof( nextArg );
	  if (gfp->cwlimit <= 0 ) {
	    fprintf(stderr,"Must specify cwlimit in kHz\n");
	    exit(1);
	  }
	} /* some more GNU-ish options could be added
	   * version       => complete name, version and license info (normal exit)  
	   * quiet/silent  => no messages on screen
	   * brief         => few messages on screen (name, status report)
	   * verbose       => all infos to screen (brhist, internal flags/filters)
	   * o/output file => specifies output filename
	   * O             => stdout
	   * i/input file  => specifies input filename
	   * I             => stdin
	   */
	else if (strcmp(token, "help") ==0
	       ||strcmp(token, "usage")==0){
	  lame_help(gfp,programName);  /* doesn't return */
	}
	else if (strcmp(token, "preset")==0) {
	  argUsed=1;
	  if (strcmp(nextArg,"phone")==0)
	  { /* when making changes, please update help text too */
	    gfp->brate = 16; 
	    gfp->highpassfreq=260;
            gfp->highpasswidth=40; 
	    gfp->lowpassfreq=3700;
	    gfp->lowpasswidth=300;
	    gfp->VBR_q=5;
	    gfp->VBR_min_bitrate_kbps=8;
	    gfp->VBR_max_bitrate_kbps=56;
	    gfp->no_short_blocks=1;
	    gfp->out_samplerate =  16000;
	    gfp->mode = MPG_MD_MONO; 
	    gfp->mode_fixed = 1; 
	    gfp->quality = 5;
	  }
	  else if (strcmp(nextArg,"voice")==0)
	  { /* when making changes, please update help text too */
	    gfp->brate = 56; 
	    gfp->highpassfreq=100;  
	    gfp->highpasswidth=20;
	    gfp->lowpasswidth=2000;
	    gfp->lowpassfreq=11000;
	    gfp->VBR_q=4;
	    gfp->VBR_min_bitrate_kbps=8;
	    gfp->VBR_max_bitrate_kbps=96;
	    gfp->no_short_blocks=1;
	    gfp->mode = MPG_MD_MONO; 
	    gfp->mode_fixed = 1; 
	    gfp->out_samplerate =  24000; 
	    gfp->quality = 5;
	  }
	  else if (strcmp(nextArg,"fm")==0)
	  { /* when making changes, please update help text too */
	    gfp->brate = 96; 
            gfp->highpassfreq=30;
            gfp->highpasswidth=0;
            gfp->lowpassfreq=15000;
            gfp->lowpasswidth=0;
	    gfp->VBR_q=4;
	    gfp->VBR_min_bitrate_kbps=32;
	    gfp->VBR_max_bitrate_kbps=192;
	    gfp->mode = MPG_MD_JOINT_STEREO; 
	    gfp->mode_fixed = 1; 
	    /*gfp->out_samplerate =  32000; */ /* determined automatically based on bitrate & sample freq. */
	    gfp->quality = 5;
	  }
	  else if (strcmp(nextArg,"tape")==0)
	  { /* when making changes, please update help text too */
	    gfp->brate = 128; 
            gfp->highpassfreq=15;
            gfp->highpasswidth=15;
            gfp->lowpassfreq=17000;
            gfp->lowpasswidth=2000;
	    gfp->VBR_q=4;
	    gfp->VBR_min_bitrate_kbps=32;
	    gfp->VBR_max_bitrate_kbps=192;
	    gfp->mode = MPG_MD_JOINT_STEREO; 
	    gfp->mode_fixed = 1; 
	    gfp->quality = 5;
	  }
	  else if (strcmp(nextArg,"hifi")==0)
	  { /* when making changes, please update help text too */
	    gfp->brate = 160;            
	    gfp->highpassfreq=15;
            gfp->highpasswidth=15;
            gfp->lowpassfreq=20000;
            gfp->lowpasswidth=3000;
	    gfp->VBR_q=3;
	    gfp->VBR_min_bitrate_kbps=32;
	    gfp->VBR_max_bitrate_kbps=224;
	    gfp->mode = MPG_MD_JOINT_STEREO; 
	    gfp->mode_fixed = 1; 
	    gfp->quality = 2;
	  }
	  else if (strcmp(nextArg,"cd")==0)
	  { /* when making changes, please update help text too */
	    gfp->brate = 192;  
	    gfp->lowpassfreq=-1;
            gfp->highpassfreq=-1;
	    gfp->VBR_q=2;
	    gfp->VBR_min_bitrate_kbps=80;
	    gfp->VBR_max_bitrate_kbps=256;
	    gfp->mode = MPG_MD_STEREO; 
	    gfp->mode_fixed = 1; 
	    gfp->quality = 2;
	  }
	  else if (strcmp(nextArg,"studio")==0)
	  { /* when making changes, please update help text too */
	    gfp->brate = 256; 
	    gfp->lowpassfreq=-1;
            gfp->highpassfreq=-1;
	    gfp->VBR_q=0;
	    gfp->VBR_min_bitrate_kbps=112;
	    gfp->VBR_max_bitrate_kbps=320;
	    gfp->mode = MPG_MD_STEREO; 
	    gfp->mode_fixed = 1; 
	    gfp->quality = 2; /* should be 0, but does not work now */
	  }
	  else if (strcmp(nextArg,"help")==0)
	  {
	    lame_presets_info(gfp,programName);  /* doesn't return */
	  }
	  else
	    {
	      fprintf(stderr,"%s: --preset type, type must be phone, voice, fm, tape, hifi, cd or studio, not %s\n",
		      programName, nextArg);
	      exit(1);
	    }
	} /* --preset */
	else
	  {
	    fprintf(stderr,"%s: unrec option --%s\n",
		    programName, token);
	  }
	i += argUsed;
	
      } else  while( (c = *token++) ) {
	if(*token ) arg = token;
	else                             arg = nextArg;
	switch(c) {
	case 'm':        argUsed = 1;   gfp->mode_fixed = 1;
	  if (*arg == 's')
	    { gfp->mode = MPG_MD_STEREO; }
	  else if (*arg == 'd')
	    { gfp->mode = MPG_MD_DUAL_CHANNEL; }
	  else if (*arg == 'j')
	    { gfp->mode = MPG_MD_JOINT_STEREO; }
	  else if (*arg == 'f')
	    { gfp->mode = MPG_MD_JOINT_STEREO; gfp->force_ms=1; }
	  else if (*arg == 'm')
	    { gfp->mode = MPG_MD_MONO; }
	  else {
	    fprintf(stderr,"%s: -m mode must be s/d/j/f/m not %s\n",
		    programName, arg);
	    err = 1;
	  }
	  break;
	case 'V':        argUsed = 1;   gfp->VBR = 1;  
	  gfp->VBR_q = atoi(arg);
	  if (gfp->VBR_q <0) gfp->VBR_q=0;
	  if (gfp->VBR_q >9) gfp->VBR_q=9;
	  break;
	case 'q':        argUsed = 1; 
	  user_quality = atoi(arg);
	  if (user_quality<0) user_quality=0;
	  if (user_quality>9) user_quality=9;
	  break;
	case 's':
	  argUsed = 1;
	  srate = atof( arg );
	  /* samplerate = rint( 1000.0 * srate ); $A  */
	  gfp->in_samplerate =  (( 1000.0 * srate ) + 0.5);
	  break;
	case 'b':        
	  argUsed = 1;
	  gfp->brate = atoi(arg); 
	  gfp->VBR_min_bitrate_kbps=gfp->brate;
	  break;
	case 'B':        
	  argUsed = 1;
	  gfp->VBR_max_bitrate_kbps=atoi(arg); 
	  break;	
	case 't':  /* dont write VBR tag */
	  gfp->bWriteVbrTag=0;
	  break;
	case 'r':  /* force raw pcm input file */
#ifdef LIBSNDFILE
	  fprintf(stderr,"WARNING: libsndfile may ignore -r and perform fseek's on the input.\n");
	  fprintf(stderr,"Compile without libsndfile if this is a problem.\n");
#endif
	  gfp->input_format=sf_raw;
	  break;
	case 'x':  /* force byte swapping */
	  gfp->swapbytes=TRUE;
	  break;
	case 'p': /* (jo) error_protection: add crc16 information to stream */
	  gfp->error_protection = 1; 
	  break;
	case 'a': /* autoconvert input file from stereo to mono - for mono mp3 encoding */
	  autoconvert=1;
	  gfp->mode=MPG_MD_MONO;
	  gfp->mode_fixed=1;
	  break;
	case 'h': 
	  gfp->quality = 2;
	  break;
	case 'k': 
	  gfp->lowpassfreq=-1;
	  gfp->highpassfreq=-1;
	  break;
	case 'd': 
	  gfp->allow_diff_short = 1;
	  break;
	case 'v': 
	  gfp->VBR = 1; 
	  break;
	case 'S': 
	  gfp->silent = TRUE;
	  break;
	case 'X':        argUsed = 1;   gfp->experimentalX = 0;
	  if (*arg == '0')
	    { gfp->experimentalX=0; }
	  else if (*arg == '1')
	    { gfp->experimentalX=1; }
	  else if (*arg == '2')
	    { gfp->experimentalX=2; }
	  else if (*arg == '3')
	    { gfp->experimentalX=3; }
	  else if (*arg == '4')
	    { gfp->experimentalX=4; }
	  else if (*arg == '5')
	    { gfp->experimentalX=5; }
	  else if (*arg == '6')
	    { gfp->experimentalX=6; }
	  else {
	    fprintf(stderr,"%s: -X n must be 0-6, not %s\n",
		    programName, arg);
	    err = 1;
	  }
	  break;


	case 'Y': 
	  gfp->experimentalY = TRUE;
	  break;
	case 'Z': 
	  gfp->experimentalZ = TRUE;
	  break;
	case 'f': 
	  gfp->quality= 9;
	  break;
	case 'g': /* turn on gtk analysis */
#ifdef HAVEGTK
	  gfp->gtkflag = TRUE;
#else
	    fprintf(stderr,"LAME not compiled with GTK support, -g not supported.\n",
		    programName, arg);
#endif
	  break;

	case 'e':        argUsed = 1;
	  if (*arg == 'n')                    gfp->emphasis = 0;
	  else if (*arg == '5')               gfp->emphasis = 1;
	  else if (*arg == 'c')               gfp->emphasis = 3;
	  else {
	    fprintf(stderr,"%s: -e emp must be n/5/c not %s\n",
		    programName, arg);
	    err = 1;
	  }
	  break;
	case 'c':   gfp->copyright = 1; break;
	case 'o':   gfp->original  = 0; break;
	
	case '?':   lame_help(gfp,programName);  /* doesn't return */
	default:    fprintf(stderr,"%s: unrec option %c\n",
				programName, c);
	err = 1; break;
	}
	if(argUsed) {
	  if(arg == token)    token = "";   /* no more from token */
	  else                ++i;          /* skip arg we used */
	  arg = ""; argUsed = 0;
	}
      }
    } else {
      if(inPath[0] == '\0')       strncpy(inPath, argv[i], MAX_NAME_SIZE);
      else if(outPath[0] == '\0') strncpy(outPath, argv[i], MAX_NAME_SIZE);
      else {
	fprintf(stderr,"%s: excess arg %s\n", programName, argv[i]);
	err = 1;
      }
    }
  }  /* loop over args */



  if(err || inPath[0] == '\0') lame_usage(gfp,programName);  /* never returns */
  if (inPath[0]=='-') gfp->silent=1;  /* turn off status - it's broken for stdin */
  if(outPath[0] == '\0') {
    if (inPath[0]=='-') {
      /* if input is stdin, default output is stdout */
      strcpy(outPath,"-");
    }else {
      strncpy(outPath, inPath, MAX_NAME_SIZE - 4);
      strncat(outPath, ".mp3", 4 );
    }
  }
  /* some file options not allowed with stdout */
  if (outPath[0]=='-') {
    gfp->bWriteVbrTag=0; /* turn off VBR tag */
    if (id3tag.used) {
      id3tag.used=0;         /* turn of id3 tagging */
      fprintf(stderr,"id3tag ignored: id3 tagging not supported for stdout.\n");
    }
  }


  /* if user did not explicitly specify input is mp3, check file name */
  if (gfp->input_format != sf_mp3)
    if (!(strcmp((char *) &inPath[strlen(inPath)-4],".mp3")))
      gfp->input_format = sf_mp3;

#if !(defined HAVEMPGLIB || defined AMIGA_MPEGA)
  if (gfp->input_format == sf_mp3) {
    fprintf(stderr,"Error: libmp3lame not compiled with mp3 *decoding* support \n");
    exit(1);
  }
#endif
  /* default guess for number of channels */
  if (autoconvert) gfp->num_channels=2; 
  else if (gfp->mode == MPG_MD_MONO) gfp->num_channels=1;
  else gfp->num_channels=2;

  /* user specified a quality value.  override any defaults set above */
  if (user_quality)   gfp->quality=user_quality;

}



#endif
