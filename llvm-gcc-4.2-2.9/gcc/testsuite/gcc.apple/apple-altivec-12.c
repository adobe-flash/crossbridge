/* APPLE LOCAL file AltiVec */
/* Check for presence of AltiVec PIM "specific operations".  */

/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-options "-faltivec -Wall" } */

vector signed int
specific_operations( signed int *p)
{
    vector unsigned int vui1, vui2;
    vector unsigned short vus1, vus2;
    vector unsigned char vuc1, vuc2;
    vector signed int vsi1, vsi2;
    vector signed short vss1, vss2;
    vector signed char vsc1, vsc2;
    vector bool int vbi1, vbi2;
    vector bool short vbs1, vbs2;
    vector bool char vbc1, vbc2;
    vector float vf1, vf2;
    vector pixel vp1, vp2;
    
    vuc1 = vec_lvebx (8, (unsigned char *)p);
    vss1 = vec_lvehx (4, (short *)p);
    vf1 = vec_lvewx (2, (float *)p);
    
    vsi1 = vec_lvx (1, (vector signed int *)p);
    vp1 = vec_lvxl (16, (vector pixel *)p);
    
    vec_stvebx (vuc1, 2, (char *)p);
    vec_stvehx (vp1, 4, (unsigned short *)p);
    vec_stvewx (vf1, 1, (float *)p);

    vec_stvx (vbi1, 12, (vector int bool *)p);
    vec_stvxl (vp1, 11, (vector pixel *)p);

    vui1 = vec_vaddcuw (vui1, vui2);
    vf1 = vec_vaddfp (vf1, vf2);
    
    vsc1 = vec_vaddsbs (vsc1, vsc2);
    vss1 = vec_vaddshs (vss1, vss2);
    vsi1 = vec_vaddsws (vsi1, vsi2);
    vuc1 = vec_vaddubs (vuc1, vuc2);
    vus1 = vec_vadduhs (vus1, vus2);
    vui1 = vec_vadduws (vui1, vui2);
    vuc1 = vec_vaddubm (vuc1, vuc2);
    vus1 = vec_vadduhm (vus1, vus2);
    vui1 = vec_vadduwm (vui1, vui2);

    vsc1 = vec_vand (vsc1, vsc2);
    vss1 = vec_vandc (vss1, vss2);

    vsc1 = vec_vavgsb (vsc1, vsc2);
    vss1 = vec_vavgsh (vss1, vss2);
    vsi1 = vec_vavgsw (vsi1, vsi2);
    vuc1 = vec_vavgub (vuc1, vuc2);
    vus1 = vec_vavguh (vus1, vus2);
    vui1 = vec_vavguw (vui1, vui2);
    
    vf1 = vec_vcfsx (vsi1, 4);
    vf2 = vec_vcfux (vui1, 3);
    
    vsi1 = vec_vcmpbfp (vf1, vf2);
    vbi1 = vec_vcmpeqfp (vf1, vf2);
    
    vbc1 = vec_vcmpequb (vuc1, vuc2);
    vbs2 = vec_vcmpequh (vus1, vus2);
    vbi1 = vec_vcmpequw (vui1, vui2);

    vbi1 = vec_vcmpgefp (vf1, vf2);
    vbi2 = vec_vcmpgtfp (vf1, vf2);

    vbc1 = vec_vcmpgtsb (vsc1, vsc2);
    vbs1 = vec_vcmpgtsh (vss1, vss2);
    vbi1 = vec_vcmpgtsw (vsi1, vsi2);

    vbc1 = vec_vcmpgtub (vuc1, vuc2);
    vbs1 = vec_vcmpgtuh (vus1, vus2);
    vbi1 = vec_vcmpgtuw (vui1, vui2);
    
    vsi1 = vec_vctsxs (vf1, 22);
    vui1 = vec_vctuxs (vf2, 30);
    
    vf2 = vec_vexptefp (vf1);
    vf1 = vec_vlogefp (vf2);
    vf2 = vec_vmaddfp (vf1, vf2, vf2);
    
    vf1 = vec_vmaxfp (vf1, vf2);
    vsc1 = vec_vmaxsb (vsc1, vsc2);
    vss1 = vec_vmaxsh (vss1, vss2);
    vsi1 = vec_vmaxsw (vsi1, vsi2);
    vuc1 = vec_vmaxub (vuc1, vuc2);
    vus1 = vec_vmaxuh (vus1, vus2);
    vui1 = vec_vmaxuw (vui1, vui2);

    vbc1 = vec_vmrghb (vbc1, vbc2);
    vp1 = vec_vmrghh (vp1, vp2);
    vf2 = vec_vmrghw (vf2, vf1);
    
    vbc1 = vec_vmrglb (vbc1, vbc2);
    vp1 = vec_vmrglh (vp1, vp2);
    vf2 = vec_vmrglw (vf2, vf1);
    
    vf1 = vec_vminfp (vf1, vf2);
    vsc1 = vec_vminsb (vsc1, vsc2);
    vss1 = vec_vminsh (vss1, vss2);
    vsi1 = vec_vminsw (vsi1, vsi2);
    vuc1 = vec_vminub (vuc1, vuc2);
    vus1 = vec_vminuh (vus1, vus2);
    vui1 = vec_vminuw (vui1, vui2);
    
    vss1 = vec_vmhaddshs (vss1, vss2, vss2);
    vss2 = vec_vmhraddshs (vss1, vss2, vss2);
    vus1 = vec_vmladduhm (vus1, vus1, vus2);
    vui1 = vec_vmsumubm (vuc1, vuc2, vui1);
    vsi1 = vec_vmsummbm (vsc1, vuc2, vsi1);
    vsi2 = vec_vmsumshm (vss1, vss2, vsi1);
    vui1 = vec_vmsumuhm (vus1, vus2, vui2);
    vui2 = vec_vmsumuhs (vus1, vus2, vui1);
    vsi2 = vec_vmsumshs (vss1, vss1, vsi2);
 
    vus1 = vec_vmuleub (vuc1, vuc2);
    vss1 = vec_vmulesb (vsc1, vsc2);
    vui1 = vec_vmuleuh (vus1, vus2);
    vsi1 = vec_vmulesh (vss1, vss2);

    vus2 = vec_vmuloub (vuc1, vuc2);
    vss2 = vec_vmulosb (vsc1, vsc2);
    vui2 = vec_vmulouh (vus1, vus2);
    vsi2 = vec_vmulosh (vss1, vss2);
    
    vf1 = vec_vnmsubfp (vf1, vf2, vf1);
    
    vp1 = vec_vnor (vp1, vp2);
    vf2 = vec_vor (vf2, vbi1);
    vf2 = vec_vxor (vf2, vbi1);
    
    vsc1 = vec_vpkuhum (vss1, vss2);
    vbs1 = vec_vpkuwum (vbi1, vbi2);
    vp1 = vec_vpkpx (vui1, vui2);
    vuc1 = vec_vpkuhus (vus1, vus1);
    vsc1 = vec_vpkshss (vss1, vss2);
    vus1 = vec_vpkuwus (vui1, vui2);
    vss2 = vec_vpkswss (vsi1, vsi1);
    vuc1 = vec_vpkshus (vss1, vss2);
    vus1 = vec_vpkswus (vsi1, vsi1);
    
    vp1 = vec_vperm (vp1, vp2, vuc1);

    vf1 = vec_vrefp (vf2);
    vf2 = vec_vrfin (vf1);
    vf1 = vec_vrsqrtefp (vf2);
    
    vuc1 = vec_vrlb (vuc1, vuc2);
    vss1 = vec_vrlh (vss2, vss1);
    vui2 = vec_vrlw (vui2, vui1);
    
    vf1 = vec_vsel (vf2, vf1, vbi1);
    
    vuc1 = vec_vslb (vuc1, vuc2);
    vss1 = vec_vslh (vss2, vss1);
    vui2 = vec_vslw (vui2, vui1);
    
    vp1 = vec_vsldoi (vp2, vp1, 14);
    vp2 = vec_vsl (vp1, vuc1);
    vp1 = vec_vslo (vp2, vuc1);
    
    vbc1 = vec_vspltb (vbc1, 9);
    vp2 = vec_vsplth (vp1, 7);
    vf1 = vec_vspltw (vf2, 31);
    vsc2 = vec_vspltisb (15);
    vss1 = vec_vspltish (14);
    vsi1 = vec_vspltisw (13);

    vuc1 = vec_vsrb (vuc1, vuc2);
    vss1 = vec_vsrh (vss2, vss1);
    vui2 = vec_vsrw (vui2, vui1);
    vuc2 = vec_vsrab (vuc1, vuc2);
    vss2 = vec_vsrah (vss2, vss1);
    vui1 = vec_vsraw (vui2, vui1);
    vp2 = vec_vsr (vp1, vuc1);
    vp1 = vec_vsro (vp2, vuc1);

    vui1 = vec_vsubcuw (vui1, vui2);
    vf1 = vec_vsubfp (vf1, vf2);
    vsc1 = vec_vsubsbs (vsc1, vsc2);
    vss1 = vec_vsubshs (vss1, vss2);
    vsi1 = vec_vsubsws (vsi1, vsi2);
    vuc1 = vec_vsububs (vuc1, vuc2);
    vus1 = vec_vsubuhs (vus1, vus2);
    vui1 = vec_vsubuws (vui1, vui2);
    vuc1 = vec_vsububm (vuc1, vuc2);
    vus1 = vec_vsubuhm (vus1, vus2);
    vui1 = vec_vsubuwm (vui1, vui2);

    vui2 = vec_vsum4ubs (vuc1, vui1);
    vsi1 = vec_vsum4sbs (vsc2, vsi1);
    vsi1 = vec_vsum2sws (vsi1, vsi1);
    vsi2 = vec_vsumsws (vsi2, vsi1);
    
    vf2 = vec_vrfiz (vf1);
    
    vbs1 = vec_vupkhsb (vbc1);
    vui1 = vec_vupkhpx (vp1);
    vbi1 = vec_vupkhsh (vbs1);
    
    vss1 = vec_vupklsb (vsc1);
    vui1 = vec_vupklpx (vp2);
    vbi1 = vec_vupklsh (vbs1);
    
    return vsi1;
}

/* { dg-final { scan-assembler "\tlvebx "      } } */
/* { dg-final { scan-assembler "\tlvehx "      } } */
/* { dg-final { scan-assembler "\tlvewx "      } } */
/* { dg-final { scan-assembler "\tlvx "        } } */
/* { dg-final { scan-assembler "\tlvxl "       } } */
/* { dg-final { scan-assembler "\tstvebx "     } } */
/* { dg-final { scan-assembler "\tstvehx "     } } */
/* { dg-final { scan-assembler "\tstvewx "     } } */
/* { dg-final { scan-assembler "\tstvx "       } } */
/* { dg-final { scan-assembler "\tstvxl "      } } */

/* { dg-final { scan-assembler "\tvaddcuw "    } } */
/* { dg-final { scan-assembler "\tvaddfp "     } } */
/* { dg-final { scan-assembler "\tvaddsbs "    } } */
/* { dg-final { scan-assembler "\tvaddshs "    } } */
/* { dg-final { scan-assembler "\tvaddsws "    } } */
/* { dg-final { scan-assembler "\tvaddubs "    } } */
/* { dg-final { scan-assembler "\tvadduhs "    } } */
/* { dg-final { scan-assembler "\tvadduws "    } } */
/* { dg-final { scan-assembler "\tvaddubm "    } } */
/* { dg-final { scan-assembler "\tvadduhm "    } } */
/* { dg-final { scan-assembler "\tvadduwm "    } } */

/* { dg-final { scan-assembler "\tvand "       } } */
/* { dg-final { scan-assembler "\tvandc "      } } */

/* { dg-final { scan-assembler "\tvavgsb "     } } */
/* { dg-final { scan-assembler "\tvavgsh "     } } */
/* { dg-final { scan-assembler "\tvavgsw "     } } */
/* { dg-final { scan-assembler "\tvavgub "     } } */
/* { dg-final { scan-assembler "\tvavguh "     } } */
/* { dg-final { scan-assembler "\tvavguw "     } } */

/* { dg-final { scan-assembler "\tvcfsx "      } } */
/* { dg-final { scan-assembler "\tvcfux "      } } */

/* { dg-final { scan-assembler "\tvcmpbfp "    } } */
/* { dg-final { scan-assembler "\tvcmpeqfp "   } } */

/* { dg-final { scan-assembler "\tvcmpequb "   } } */
/* { dg-final { scan-assembler "\tvcmpequh "   } } */
/* { dg-final { scan-assembler "\tvcmpequw "   } } */

/* { dg-final { scan-assembler "\tvcmpgefp "   } } */
/* { dg-final { scan-assembler "\tvcmpgtfp "   } } */

/* { dg-final { scan-assembler "\tvcmpgtsb "   } } */
/* { dg-final { scan-assembler "\tvcmpgtsh "   } } */
/* { dg-final { scan-assembler "\tvcmpgtsw "   } } */

/* { dg-final { scan-assembler "\tvcmpgtub "   } } */
/* { dg-final { scan-assembler "\tvcmpgtuh "   } } */
/* { dg-final { scan-assembler "\tvcmpgtuw "   } } */

/* { dg-final { scan-assembler "\tvctsxs "     } } */
/* { dg-final { scan-assembler "\tvctuxs "     } } */

/* { dg-final { scan-assembler "\tvexptefp "   } } */
/* { dg-final { scan-assembler "\tvlogefp "    } } */
/* { dg-final { scan-assembler "\tvmaddfp "    } } */

/* { dg-final { scan-assembler "\tvmaxfp "     } } */
/* { dg-final { scan-assembler "\tvmaxsb "     } } */
/* { dg-final { scan-assembler "\tvmaxsh "     } } */
/* { dg-final { scan-assembler "\tvmaxsw "     } } */
/* { dg-final { scan-assembler "\tvmaxub "     } } */
/* { dg-final { scan-assembler "\tvmaxuh "     } } */
/* { dg-final { scan-assembler "\tvmaxuw "     } } */

/* { dg-final { scan-assembler "\tvmrghb "     } } */
/* { dg-final { scan-assembler "\tvmrghh "     } } */
/* { dg-final { scan-assembler "\tvmrghw "     } } */

/* { dg-final { scan-assembler "\tvmrglb "     } } */
/* { dg-final { scan-assembler "\tvmrglh "     } } */
/* { dg-final { scan-assembler "\tvmrglw "     } } */

/* { dg-final { scan-assembler "\tvminfp "     } } */
/* { dg-final { scan-assembler "\tvminsb "     } } */
/* { dg-final { scan-assembler "\tvminsh "     } } */
/* { dg-final { scan-assembler "\tvminsw "     } } */
/* { dg-final { scan-assembler "\tvminub "     } } */
/* { dg-final { scan-assembler "\tvminuh "     } } */
/* { dg-final { scan-assembler "\tvminuw "     } } */

/* { dg-final { scan-assembler "\tvmhaddshs "  } } */
/* { dg-final { scan-assembler "\tvmhraddshs " } } */
/* { dg-final { scan-assembler "\tvmladduhm "  } } */
/* { dg-final { scan-assembler "\tvmsumubm "   } } */
/* { dg-final { scan-assembler "\tvmsummbm "   } } */
/* { dg-final { scan-assembler "\tvmsumshm "   } } */
/* { dg-final { scan-assembler "\tvmsumuhm "   } } */
/* { dg-final { scan-assembler "\tvmsumuhs "   } } */
/* { dg-final { scan-assembler "\tvmsumshs "   } } */

/* { dg-final { scan-assembler "\tvmuleub "    } } */
/* { dg-final { scan-assembler "\tvmulesb "    } } */
/* { dg-final { scan-assembler "\tvmuleuh "    } } */
/* { dg-final { scan-assembler "\tvmulesh "    } } */

/* { dg-final { scan-assembler "\tvmuloub "    } } */
/* { dg-final { scan-assembler "\tvmulosb "    } } */
/* { dg-final { scan-assembler "\tvmulouh "    } } */
/* { dg-final { scan-assembler "\tvmulosh "    } } */

/* { dg-final { scan-assembler "\tvnmsubfp "   } } */

/* { dg-final { scan-assembler "\tvnor "       } } */
/* { dg-final { scan-assembler "\tvor "        } } */
/* { dg-final { scan-assembler "\tvxor "       } } */

/* { dg-final { scan-assembler "\tvpkuhum "    } } */
/* { dg-final { scan-assembler "\tvpkuwum "    } } */
/* { dg-final { scan-assembler "\tvpkpx "      } } */
/* { dg-final { scan-assembler "\tvpkuhus "    } } */
/* { dg-final { scan-assembler "\tvpkshss "    } } */
/* { dg-final { scan-assembler "\tvpkuwus "    } } */
/* { dg-final { scan-assembler "\tvpkswss "    } } */
/* { dg-final { scan-assembler "\tvpkshus "    } } */
/* { dg-final { scan-assembler "\tvpkswus "    } } */

/* { dg-final { scan-assembler "\tvperm "      } } */

/* { dg-final { scan-assembler "\tvrefp "      } } */
/* { dg-final { scan-assembler "\tvrfin "      } } */
/* { dg-final { scan-assembler "\tvrsqrtefp "  } } */

/* { dg-final { scan-assembler "\tvrlb "       } } */
/* { dg-final { scan-assembler "\tvrlh "       } } */
/* { dg-final { scan-assembler "\tvrlw "       } } */

/* { dg-final { scan-assembler "\tvsel "       } } */

/* { dg-final { scan-assembler "\tvslb "       } } */
/* { dg-final { scan-assembler "\tvslh "       } } */
/* { dg-final { scan-assembler "\tvslw "       } } */

/* { dg-final { scan-assembler "\tvsldoi "     } } */
/* { dg-final { scan-assembler "\tvsl "        } } */
/* { dg-final { scan-assembler "\tvslo "       } } */

/* { dg-final { scan-assembler "\tvspltb "     } } */
/* { dg-final { scan-assembler "\tvsplth "     } } */
/* { dg-final { scan-assembler "\tvspltw "     } } */
/* { dg-final { scan-assembler "\tvspltisb "   } } */
/* { dg-final { scan-assembler "\tvspltish "   } } */
/* { dg-final { scan-assembler "\tvspltisw "   } } */

/* { dg-final { scan-assembler "\tvsrb "       } } */
/* { dg-final { scan-assembler "\tvsrh "       } } */
/* { dg-final { scan-assembler "\tvsrw "       } } */
/* { dg-final { scan-assembler "\tvsrab "      } } */
/* { dg-final { scan-assembler "\tvsrah "      } } */
/* { dg-final { scan-assembler "\tvsraw "      } } */
/* { dg-final { scan-assembler "\tvsr "        } } */
/* { dg-final { scan-assembler "\tvsro "       } } */

/* { dg-final { scan-assembler "\tvsubcuw "    } } */
/* { dg-final { scan-assembler "\tvsubfp "     } } */
/* { dg-final { scan-assembler "\tvsubsbs "    } } */
/* { dg-final { scan-assembler "\tvsubshs "    } } */
/* { dg-final { scan-assembler "\tvsubsws "    } } */
/* { dg-final { scan-assembler "\tvsububs "    } } */
/* { dg-final { scan-assembler "\tvsubuhs "    } } */
/* { dg-final { scan-assembler "\tvsubuws "    } } */
/* { dg-final { scan-assembler "\tvsububm "    } } */
/* { dg-final { scan-assembler "\tvsubuhm "    } } */
/* { dg-final { scan-assembler "\tvsubuwm "    } } */

/* { dg-final { scan-assembler "\tvsum4ubs "   } } */
/* { dg-final { scan-assembler "\tvsum4sbs "   } } */
/* { dg-final { scan-assembler "\tvsum2sws "   } } */
/* { dg-final { scan-assembler "\tvsumsws "    } } */

/* { dg-final { scan-assembler "\tvrfiz "      } } */

/* { dg-final { scan-assembler "\tvupkhsb "    } } */
/* { dg-final { scan-assembler "\tvupkhpx "    } } */
/* { dg-final { scan-assembler "\tvupkhsh "    } } */

/* { dg-final { scan-assembler "\tvupklsb "    } } */
/* { dg-final { scan-assembler "\tvupklpx "    } } */
/* { dg-final { scan-assembler "\tvupklsh "    } } */
