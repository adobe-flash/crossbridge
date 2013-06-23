##########################################################################
#                                                                        #
#  refstyles.ld                                                          #
#                                                                        #
#  Standard reference styles database.                                   #
#  Jeffrey H. Kingston                                                   #
#  August 1994 (@NumSep added in Version 3.13, February 1999).           #
#                                                                        #
##########################################################################


##########################################################################
#                                                                        #
#  Large works: Book, Proceedings (identical to Book), PhDThesis.        #
#                                                                        #
##########################################################################

{ Book @RefStyle @Style
  {  @Reference&&reftag @Open
     {
	{ @Author.  {}			} @If @Author
	{ @I @Title			} @If @Title
	{ @Word&&notitle		} @If @Not @Title
	{ , @Pinpoint			} @If @Pinpoint
	{ , @Word&&pages @NumSep @Pages } @If @Pages
	{ , @Word&&page @NumSep @Page	} @If @Page
	{ .  @TitleNote			} @If @TitleNote
	{ .  @HowPublished		} @If @HowPublished
	{ .  @Publisher			} @If @Publisher
	{ .  @Organization		} @If @Organization
	{ .  @Institution		} @If @Institution
	{ , @Address			} @If @Address
	{ .  @Edition			} @If @Edition
	{ , @Month @Year		} @If @Year @And @Month
	{ , @Year 			} @If @Year @And @Not @Month
	{ .				} @If @True
	{ {}  URL @I { @URL }.		} @If @URL
	{ {}  @Note			} @If @Note
     }
  }
}

{ Proceedings @RefStyle @Style
  {  @Reference&&reftag @Open
     {
	{ @Author.  {}			} @If @Author
	{ @I @Title			} @If @Title
	{ @Word&&notitle		} @If @Not @Title
	{ , @Pinpoint			} @If @Pinpoint
	{ , @Word&&pages @NumSep @Pages } @If @Pages
	{ , @Word&&page @NumSep @Page	} @If @Page
	{ .  @TitleNote			} @If @TitleNote
	{ .  @HowPublished		} @If @HowPublished
	{ .  @Publisher			} @If @Publisher
	{ .  @Organization		} @If @Organization
	{ .  @Institution		} @If @Institution
	{ , @Address			} @If @Address
	{ .  @Edition			} @If @Edition
	{ , @Month @Year		} @If @Year @And @Month
	{ , @Year 			} @If @Year @And @Not @Month
	{ .				} @If @True
	{ {}  URL @I { @URL }.		} @If @URL
	{ {}  @Note			} @If @Note
     }
  }
}

{ PhDThesis @RefStyle @Style
  {  @Reference&&reftag @Open
     {
	{ @Author.  {}			} @If @Author
	{ @I @Title			} @If @Title
	{ @Word&&notitle		} @If @Not @Title
	{ , @Pinpoint			} @If @Pinpoint
	{ , @Word&&pages @NumSep @Pages } @If @Pages
	{ , @Word&&page @NumSep @Page	} @If @Page
	{ .  @TitleNote			} @If @TitleNote
	{ .  @Word&&phdthesis		} @If @True
	{ , @HowPublished		} @If @HowPublished
	{ , @Publisher			} @If @Publisher
	{ , @Organization		} @If @Organization
	{ , @Institution		} @If @Institution
	{ , @Address			} @If @Address
	{ .  @Edition			} @If @Edition
	{ , @Month @Year		} @If @Year @And @Month
	{ , @Year 			} @If @Year @And @Not @Month
	{ .				} @If @True
	{ {}  URL @I { @URL }.		} @If @URL
	{ {}  @Note			} @If @Note
     }
  }
}


##########################################################################
#                                                                        #
#  Small works not appearing within anything else: TechReport, Manual,   #
#  MastersThesis, Seminar, Misc                                          #
#                                                                        #
##########################################################################

{ TechReport @RefStyle @Style
  {  @Reference&&reftag @Open
     {
	{ @Author.  {}			} @If @Author
	{ @Title			} @If @Title
	{ @Word&&notitle		} @If @Not @Title
	{ , @Pinpoint			} @If @Pinpoint
	{ , @Word&&pages @NumSep @Pages } @If @Pages
	{ , @Word&&page @NumSep @Page	} @If @Page
	{ .  @TitleNote			} @If @TitleNote
	{ .  @TRType			} @If @TRType
	{ .  @Word&&techrep		} @If @Not @TRType
	{ {} @Number			} @If @Number
	{ {} (@Month @Year)		} @If @Year @And @Month
	{ {} (@Year)			} @If @Year @And @Not @Month
	{ , @HowPublished		} @If @HowPublished
	{ , @Publisher			} @If @Publisher
	{ , @Organization		} @If @Organization
	{ , @Institution		} @If @Institution
	{ , @Address			} @If @Address
	{ .  @Edition			} @If @Edition
	{ .				} @If @True
	{ {}  URL @I { @URL }.		} @If @URL
	{ {}  @Note			} @If @Note
     }
  }
}

{ MastersThesis @RefStyle @Style
  {  @Reference&&reftag @Open
     {
	{ @Author.  {}			} @If @Author
	{ @Title			} @If @Title
	{ @Word&&notitle		} @If @Not @Title
	{ , @Pinpoint			} @If @Pinpoint
	{ , @Word&&pages @NumSep @Pages } @If @Pages
	{ , @Word&&page @NumSep @Page	} @If @Page
	{ .  @TitleNote			} @If @TitleNote
	{ .  @Word&&mastersthesis	} @If @True
	{ {} @Number			} @If @Number
	{ {} (@Month @Year)		} @If @Year @And @Month
	{ {} (@Year)			} @If @Year @And @Not @Month
	{ , @HowPublished		} @If @HowPublished
	{ , @Publisher			} @If @Publisher
	{ , @Organization		} @If @Organization
	{ , @Institution		} @If @Institution
	{ , @Address			} @If @Address
	{ .  @Edition			} @If @Edition
	{ .				} @If @True
	{ {}  URL @I { @URL }.		} @If @URL
	{ {}  @Note			} @If @Note
     }
  }
}

{ Seminar @RefStyle @Style
  {  @Reference&&reftag @Open
     {
	{ @Author.  {}			} @If @Author
	{ @I @Title			} @If @Title
	{ @Word&&notitle		} @If @Not @Title
	{ , @Pinpoint			} @If @Pinpoint
	{ , @Word&&pages @NumSep @Pages } @If @Pages
	{ , @Word&&page @NumSep @Page	} @If @Page
	{ .  @TitleNote			} @If @TitleNote
	{ .  @HowPublished		} @If @HowPublished
	{ .  @Publisher			} @If @Publisher
	{ .  @Organization		} @If @Organization
	{ .  @Institution		} @If @Institution
	{ , @Address			} @If @Address
	{ .  @Edition			} @If @Edition
	{ , @Day @Month @Year		} @If @Year @And @Month @And @Day
	{ , @Month @Year		} @If @Year @And @Month @And @Not @Day
	{ , @Year 			} @If @Year @And @Not @Month
	{ .				} @If @True
	{ {}  URL @I { @URL }.		} @If @URL
	{ {}  @Note			} @If @Note
     }
  }
}

{ Misc @RefStyle @Style
  {  @Reference&&reftag @Open
     {
	{ @Author.  {}			} @If @Author
	{ @Title			} @If @Title
	{ @Word&&notitle		} @If @Not @Title
	{ , @Pinpoint			} @If @Pinpoint
	{ , @Word&&pages @NumSep @Pages } @If @Pages
	{ , @Word&&page @NumSep @Page	} @If @Page
	{ .  @TitleNote			} @If @TitleNote
	{ .  @HowPublished		} @If @HowPublished
	{ , @Publisher			} @If @Publisher
	{ , @Organization		} @If @Organization
	{ , @Institution		} @If @Institution
	{ , @Address			} @If @Address
	{ .  @Edition			} @If @Edition
	{ , @Month @Year		} @If @Year @And @Month
	{ , @Year			} @If @Year @And @Not @Month
	{ .				} @If @True
	{ {}  URL @I { @URL }.		} @If @URL
	{ {}  @Note			} @If @Note
     }
  }
}


##########################################################################
#                                                                        #
#  Small works appearing within an ongoing forum: Article.               #
#                                                                        #
##########################################################################

{ Article @RefStyle @Style
  {  @Reference&&reftag @Open
     {
	{ @Author.  {}			} @If @Author
	{ @Title			} @If @Title
	{ @Word&&notitle		} @If @Not @Title
	{ , @Pinpoint			} @If @Pinpoint
	{ .				} @If @True
	{ {}  @TitleNote.		} @If @TitleNote
	{ {}  @I @Journal		} @If @Journal
	{ {}  @Word&&nopublisher	} @If @Not @Journal
	{ {} @B @Volume			} @If @Volume
	{ {} (@Number)			} @If @Number @And @Volume
	{ {} @Number			} @If @Number @And @Not @Volume
	{ , @Pages			} @If @Pages
	{ , @Word&&page @Page		} @If @Page
	{ , @Word&&page @NumSep @Page	} @If @Page
	{ {} (@Day @Month @Year)	} @If @Year @And @Month @And @Day
	{ {} (@Month @Year)		} @If @Year @And @Month @And @Not @Day
	{ {} (@Year)			} @If @Year @And @Not @Month
	{ .				} @If @True
	{ {}  URL @I { @URL }.		} @If @URL
	{ {}  @Note			} @If @Note
     }
  }
}


##########################################################################
#                                                                        #
#  Small works appearing within large works:  InBook, InProceedings      #
#  (identical to InBook).                                                #
#                                                                        #
##########################################################################

{ InBook @RefStyle @Style
  {  @Reference&&reftag @Open
     {
	{ @Author.  {}			} @If @Author
	{ @Title			} @If @Title
	{ @Word&&notitle		} @If @Not @Title
	{ .  @Word&&in {}		} @If @True
	{ @InAuthor, {}			} @If @InAuthor
	{ @I @InTitle			} @If @InTitle
	{ @Word&&notitle		} @If @Not @InTitle
	{ , @Pinpoint			} @If @Pinpoint
	{ , @Word&&pages @NumSep @Pages } @If @Pages
	{ , @Word&&page @NumSep @Page	} @If @Page
	{ .  @TitleNote			} @If @TitleNote
	{ .  @HowPublished		} @If @HowPublished
	{ .  @Publisher			} @If @Publisher
	{ .  @Organization		} @If @Organization
	{ .  @Institution		} @If @Institution
	{ , @Address			} @If @Address
	{ .  @Edition			} @If @Edition
	{ , @Month @Year		} @If @Year @And @Month
	{ , @Year 			} @If @Year @And @Not @Month
	{ .				} @If @True
	{ {}  URL @I { @URL }.		} @If @URL
	{ {}  @Note			} @If @Note
     }
  }
}

{ InProceedings @RefStyle @Style
  {  @Reference&&reftag @Open
     {
	{ @Author.  {}			} @If @Author
	{ @Title			} @If @Title
	{ @Word&&notitle		} @If @Not @Title
	{ .  @Word&&in {}		} @If @True
	{ @InAuthor, {}			} @If @InAuthor
	{ @I @InTitle			} @If @InTitle
	{ @Word&&notitle		} @If @Not @InTitle
	{ , @Pinpoint			} @If @Pinpoint
	{ , @Word&&pages @NumSep @Pages } @If @Pages
	{ , @Word&&page @NumSep @Page	} @If @Page
	{ .  @TitleNote			} @If @TitleNote
	{ .  @HowPublished		} @If @HowPublished
	{ .  @Publisher			} @If @Publisher
	{ .  @Organization		} @If @Organization
	{ .  @Institution		} @If @Institution
	{ , @Address			} @If @Address
	{ .  @Edition			} @If @Edition
	{ , @Month @Year		} @If @Year @And @Month
	{ , @Year 			} @If @Year @And @Not @Month
	{ .				} @If @True
	{ {}  URL @I { @URL }.		} @If @URL
	{ {}  @Note			} @If @Note
     }
  }
}
