#!/usr/bin/perl -w -I ./
#####!/usr/bin/perl -T -w -I ./
use IO::File;

# stdin: html
# stdout: html with toc
# replace a line with "<TOC>" with table of contents
# generated from header elements: <h1> <h2> etc.
# surrounds each head element with <a name=> element

##open (TOC, "+>DOC"); 
##open (DOC, "+>TOC"); 
$toc = IO::File->new_tmpfile();
$doc = IO::File->new_tmpfile();

print ($toc "<center><b><font size=+2>Table of Contents</font></b></center>\n");

$level = 0;
#@paranums;

while ( <STDIN> )
{
	###if( /\s*<[hH](\d+)>([^<]*)<\/[hH]\1>/ )
	if( /\s*<([hHlL])(\d+)>([^<]*)<\/\1\2>/ )
	{
		my $hdr = $1;
		my $lvl = $2;
		my $txt = $3;
		if( $lvl < 1 ) { $lvl = 1; }
		if( $lvl > $level )
		{
			$paranums[$lvl-1] = 0;
			print ($toc "<dl>\n");
		}elsif( $lvl < $level )
		{
			my $ndx;
			for( $ndx=0; $lvl+$ndx < $level; $ndx++ )
			{
				print ($toc "</dl>\n");
			}
		}
		$level = $lvl;
		$paranums[$level-1] += 1;

		my $paraname = "";
		my $dot = "";
		for ( $ndx=0; $ndx<$level; $ndx++ )
		{
			$paraname = $paraname . $dot . $paranums[$ndx];
			$dot = ".";
		}
		if( "h" eq $hdr || "H" eq $hdr )
		{
			print ($doc "<a name=\"P$paraname\"><H$lvl>$paraname $txt</H$lvl></a>\n");
			print ($toc "<dt><a href=\"#P$paraname\">$paraname $txt</a></dt>\n");
		}else{
			my @txtprt = split(/;/,$txt,2);
			if( @txtprt == 2 )
			{
				print ($toc "<dt><a href=\"$txtprt[0]\">$paraname $txtprt[1]</a></dt>\n");
			}else{
				print ($toc "<dt>$paraname $txtprt[0]</H$lvl></dt>\n");
			}
		}
	}else{
		s/<header>([^<]*)<\/header>/<table class="header"><tr><td class="header">$1<\/td><\/tr><\/table>/g;
		s/<cat>([^<]*)<\/cat>/<span class="cat">$1<\/span>/g;
		s/<func>([^<]*)<\/func>/<span class="func">$1<\/span>/g;
		s/<map>([^<]*)<\/map>/<span class="map">$1<\/span>/g;
		s/<set>([^<]*)<\/set>/<span class="set">$1<\/span>/g;
		s/<obj>([^<]*)<\/obj>/<span class="object">$1<\/span>/g;
		s/<CAT>([^<]*)<\/CAT>/<span class="category">$1<\/span>/g;
		s/<FAM>([^<]*)<\/FAM>/<span class="family">$1<\/span>/g;
		s/<FUNC>([^<]*)<\/FUNC>/<span class="functor">$1<\/span>/g;

		s/&arrow;/&#x2192;/g;
		s/&rarrow;/&#x2190;/g; #reverse arrow
		s/&monic;/&#x21A6;/g; #monic arrow
		s/&equalize;/&#x21A3;/g; #equalizer arrow
		#s/&eqlz;/&#x2225;/g; #equalizer op double bar
		s/&eqlz;/&#x2251;/g; #equalizer op geometric equality

		#s/&cover;/&#x21A0;/g; #cover arrow
		s/&cover;/&#x2212;&#x22B3;/g; #cover arrow
		#s/&cover;/&#x21FE;/g; #cover arrow
		s/&isoarrow;/&#x2972;/g; #isomorphism arrow

		s/&darrow;/&#x21D2;/g; #double arrow
		s/&implies;/&#x21D2;/g; #double arrow

		s/&rdarrow;/&#x21D0;/g; #reverse double arrow
		s/&impliedby;/&#x21D0;/g; #reverse double arrow

		s/&iff;/&#x21D4;/g; #if and only if

		s/&box;/&#x25A1;/g; #source or target operator
		s/&opbox;/&#x25A3;/g; #opposite source or target op
		s/&circle;/&#x25CB;/g; #circle
		s/&cring;/&#x229A;/g; #circle ring
		s/&bullseye;/&#x25CE;/g; #circle ring

		s/&role;/&#x25CB;/g; #role
		s/&roleop;/&#x25CE;/g; #opposite role

		s/&forall;/&#x2200;/g;
		s/&exist;/&#x2203;/g;
		s/&notexist;/&#x2204;/g;
		s/&notequal;/&#x2260;/g;
		s/&isomorphic;/&#x2243;/g;

		s/&mul;/<font face="sans-serif">X<\/font>/g;
		s/&add;/+/g;

		s/&prod;/&#x220F;/g; # n-ary
		s/&coprod;/&#x2210;/g; # n-ary
		s/&sum;/&#x2211;/g; # n-ary

		s/&empty;/&#x2205;/g;
		s/&incr;/&#x2206;/g;
		s/&nable;/&#x2207;/g;
		s/&isin;/&#x2208;/g;
		s/&notin;/&#x2209;/g;
		s/&smallisin;/&#x220A;/g;
		s/&contains;/&#x220B;/g;
		s/&notcontains;/&#x220C;/g;
		s/&smallcontains;/&#x220D;/g;
		s/&QED;/&#x220E;/g;

		s/&slash;/&#x2215;/g;
		s/&setminus;/&#x2216;/g;
		s/&asterisk;/&#x2217;/g;
		s/&compose;/&#x2218;/g;
		s/&dcomp;/&nbsp;/g;
		s/&bullet;/&#x2219;/g;
		s/&infinity;/&#x221E;/g;

		s/&logicaland;/&#x2227;/g; s/&land;/&#x2227;/g;
		s/&logicalor;/&#x2228;/g;  s/&lor;/&#x2228;/g;
		s/&cap;/&#x2229;/g; s/&intersection;/&#x2229;/g;
		s/&cup;/&#x222A;/g; s/&union;/&#x222A;/g;

		s/&bar;/&#x2223;/g;
		s/&sub;/&#x2282;/g;
		s/&sup;/&#x2283;/g;
		s/&direq;/&#x21C9;/g; #directed equality, venturi tube
		#s/&direq;/&#x2254;/g; # :=, directed equality, venturi tube
		s/&colonequal;/&#x2255;/g; # :=
		s/&equalcolon;/&#x2255;/g; # =;

		s/&degree;/&#x00B0;/g;
		s/&middot;/&#x00B7;/g;
		s/&inverse;/&#x207B;&#x00B9;/g;
		s/&nland;/&#x22C0;/g;
		s/&nlor;/&#x22C1;/g;
		s/&nintersection;/&#x22C2;/g;
		s/&ncap;/&#x22C2;/g;
		s/&nunion;/&#x22C3;/g;
		s/&ncup;/&#x22C3;/g;
		s/&diamond;/&#x22C4;/g;
		s/&dot;/&#x22C5;/g;

		s/&sharp;/<sup>#<\/sup>/g;
		s/&wbullet;/&#x25E6;/g;
		s/&whitebullet;/&#x25E6;/g;

		s/&Alpha;/&#x0391;/g;
		s/&Beta;/&#x0392;/g;
		s/&Gamma;/&#x0393;/g;
		s/&Delta;/&#x0394;/g;
		s/&Epsilon;/&#x0395;/g;
		s/&Zeta;/&#x0396;/g;
		s/&Eta;/&#x0397;/g;
		s/&Theta;/&#x0398;/g;
		s/&Iota;/&#x0399;/g;
		s/&Kappa;/&#x039A;/g;
		s/&Lambda;/&#x039B;/g;
		s/&Mu;/&#x039C;/g;
		s/&Nu;/&#x039D;/g;
		s/&Xi;/&#x039E;/g;
		s/&Omicron;/&#x039F;/g;
		s/&Pi;/&#x03A0;/g;
		s/&Rho;/&#x03A1;/g;
		s/&Stigma;/&#x03A2;/g;
		s/&Sigma;/&#x03A3;/g;
		s/&Tau;/&#x03A4;/g;
		s/&Upsilon;/&#x03A5;/g;
		s/&Phi;/&#x03A6;/g;
		s/&Chi;/&#x03A7;/g;
		s/&Psi;/&#x03A8;/g;
		s/&Omega;/&#x03A9;/g;

		s/&alpha;/&#x03B1;/g;
		s/&beta;/&#x03B2;/g;
		s/&gamma;/&#x03B3;/g;
		s/&delta;/&#x03B4;/g;
		s/&epsilon;/&#x03B5;/g;
		s/&zeta;/&#x03B6;/g;
		s/&eta;/&#x03B7;/g;
		s/&theta;/&#x03B8;/g;
		s/&iota;/&#x03B9;/g;
		s/&kappa;/&#x03BA;/g;
		s/&lambda;/&#x03BB;/g;
		s/&mu;/&#x03BC;/g;
		s/&nu;/&#x03BD;/g;
		s/&xi;/&#x03BE;/g;
		s/&omicron;/&#x03BF;/g;
		s/&pi;/&#x03C0;/g;
		s/&rho;/&#x03C1;/g;
		s/&stigma;/&#x03C2;/g;
		s/&sigma;/&#x03C3;/g;
		s/&tau;/&#x03C4;/g;
		s/&upsilon;/&#x03C5;/g;
		s/&phi;/&#x03C6;/g;
		s/&chi;/&#x03C7;/g;
		s/&psi;/&#x03C8;/g;
		s/&omega;/&#x03C9;/g;
		print $doc $_;
	}
}
print ($toc "</dl>\n");

seek ($toc,0,0);
seek ($doc,0,0);

while (<$doc>)
{
	if( /<[Tt][Oo][Cc]\/?>/ )
	{
		while ( <$toc> )
		{
			print;
		}
	}else{
		print;
	}
}
close $toc;
close $doc;
#unlink TOC, DOC;
