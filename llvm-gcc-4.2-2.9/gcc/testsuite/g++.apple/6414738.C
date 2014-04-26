/* APPLE LOCAL file 6414738 */
/* { dg-do compile } */
/* { dg-options "-O3 -fomit-frame-pointer -gdwarf-2 -fno-exceptions" } */
struct TheStructure { void *memberVoidStar; };

int function(...) {
    TheStructure *theStructureStar = (TheStructure*)&&secondComputedLabel;
    goto *&&firstComputedLabel;

firstComputedLabel:
    ++theStructureStar;
    function() ? function() : function();
    goto *(++theStructureStar)->memberVoidStar;

secondComputedLabel:
    ++theStructureStar;
    TheStructure *theOtherStructureStar = *(TheStructure**)&&thirdComputedLabel;
    if (function() && theOtherStructureStar)
        goto *theStructureStar;

    void *voidStar = *(void**)&&lastComputedLabel;
    if (function())
        goto firstLabel;

    function(voidStar);

thirdComputedLabel:
    ++theStructureStar;

lastComputedLabel:
    goto *(++theStructureStar);

firstLabel:
    goto *theStructureStar->memberVoidStar;
}
