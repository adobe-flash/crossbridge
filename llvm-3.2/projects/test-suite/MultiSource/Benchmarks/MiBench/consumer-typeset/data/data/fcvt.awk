# convert old fontdefs file to new font database
NR==1	{
print "###########################################################################"
print "#                                                                         #"

	  # first line must be either EXTRAS or NO_EXTRAS
	  if( $0 == "EXTRAS" )
	  {
print "#  Font definitions database (created by fcvt, with @ExtraMetrics)        #"
	    extra_metrics = 1
	  }
	  else if( $0 == "NO_EXTRAS" )
	  {
print "#  Font definitions database (created by fcvt, without @ExtraMetrics)     #"
	    extra_metrics = 0
	  }
	  else
	  {
print "#  Font definitions database ERRONEOUS FIRST LINE NEEDS INVESTIGATION     #"
	  }
print "#                                                                         #"
print "#  Each entry defines one font.  The fields and their meanings are:       #"
print "#                                                                         #"
print "#     Name          Compulsory   Meaning                                  #"
print "#     ---------------------------------------------------------------     #"
print "#     @Tag          Yes          Must equal @Family-@Face                 #"
print "#     @Family       Yes          Font family name                         #"
print "#     @Face         Yes          Font face name                           #"
print "#     @Name         Yes          PostScript name as in @Metrics file      #"
print "#     @Metrics      Yes          Adobe font metrics file of this font     #"
print "#     @ExtraMetrics No           Extra metrics                            #"
print "#     @Mapping      Yes          .LCM (Lout Character Mapping) file       #"
print "#     @Recode       No           Recode or not (Yes or No, default Yes)   #"
print "#                                                                         #"
print "#  Jeffrey H. Kingston                                                    #"
print "#  6 May 20000                                                            #"
print "#                                                                         #"
print "###########################################################################"
	}

$0~/^#/	{
	  # comment line, skip it
	  next;
	}

	{
	  # a real line
	  # can't get split to work, so using this
	  j = 1
	  for(i = 1;  i < NF; i++ )
	  {
	    if( $1 != "" )
	    {
	      field[j++] = $i
	    }
	  }

	  # now print the lot
	  if( j == 1 )
	  {
	    # empty line, echo it
	    print ""
	  }
	  else if( j != 9 )
	  {
	    print "# error, " j " fields: " $0
	  }
	  else
	  {
	    family = field[2]
	    face = field[3]
	    name = field[5]
	    afmfile = field[6]
	    lcmfile = field[7]
	    recode = field[8]
	    if( used[family "-" face] == "" )
	    {
	      used[family "-" face] = "used"
	      print "{ @FontDef"
	      print "    @Tag { " family "-" face " }"
	      print "    @Family { " family " }"
	      print "    @Face { " face " }"
	      print "    @Name { " name " }"
	      print "    @Metrics { " afmfile " }"
	      if( extra_metrics == 1 )
	        print "    @ExtraMetrics { " afmfile "+ }"
	      print "    @Mapping { " lcmfile " }"
	      if( recode == "NoRecode" )
	        print "    @Recode { No }"
	      print "}"
	      print ""
	    }
	  }
	}
