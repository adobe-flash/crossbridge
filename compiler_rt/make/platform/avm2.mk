Description := Static runtime libraries for AVM2.

Configs :=
UniversalArchs :=

Configs += avm2
UniversalArchs.avm2 := avm2

###

CC := foo

CFLAGS.avm2 := -Wall -Werror -O3 -fomit-frame-pointer

FUNCTIONS.avm2 := \
    absvdi2 \
    absvsi2 \
    absvti2 \
    adddf3 \
    addsf3 \
    addvdi3 \
    addvsi3 \
    addvti3 \
    ashldi3 \
    ashlti3 \
    ashrdi3 \
    ashrti3 \
    clzdi2 \
    clzsi2 \
    clzti2 \
    cmpdi2 \
    cmpti2 \
    comparedf2 \
    comparesf2 \
    ctzdi2 \
    ctzsi2 \
    ctzti2 \
    divdc3 \
    divdf3 \
    divdi3 \
    divmoddi4 \
    divmodsi4 \
    divsc3 \
    divsf3 \
    divsi3 \
    divti3 \
    divxc3 \
    extendsfdf2 \
    ffsdi2 \
    ffsti2 \
    fixdfdi \
    fixdfsi \
    fixdfti \
    fixsfdi \
    fixsfsi \
    fixsfti \
    fixunsdfdi \
    fixunsdfsi \
    fixunsdfti \
    fixunssfdi \
    fixunssfsi \
    fixunssfti \
    fixunsxfdi \
    fixunsxfsi \
    fixunsxfti \
    fixxfdi \
    fixxfti \
    floatdidf \
    floatdisf \
    floatdixf \
    floatsidf \
    floatsisf \
    floattidf \
    floattisf \
    floattixf \
    floatundidf \
    floatundisf \
    floatundixf \
    floatunsidf \
    floatunsisf \
    floatuntidf \
    floatuntisf \
    floatuntixf \
    lshrdi3 \
    lshrti3 \
    moddi3 \
    modsi3 \
    modti3 \
    muldc3 \
    muldf3 \
    muldi3 \
    mulodi4 \
    mulosi4 \
    muloti4 \
    mulsc3 \
    mulsf3 \
    multi3 \
    mulvdi3 \
    mulvsi3 \
    mulvti3 \
    mulxc3 \
    negdf2 \
    negdi2 \
    negsf2 \
    negti2 \
    negvdi2 \
    negvsi2 \
    negvti2 \
    paritydi2 \
    paritysi2 \
    parityti2 \
    popcountdi2 \
    popcountsi2 \
    popcountti2 \
    powidf2 \
    powisf2 \
    powitf2 \
    powixf2 \
    subdf3 \
    subsf3 \
    subvdi3 \
    subvsi3 \
    subvti3 \
    truncdfsf2 \
    ucmpdi2 \
    ucmpti2 \
    udivdi3 \
    udivmoddi4 \
    udivmodsi4 \
    udivmodti4 \
    udivsi3 \
    udivti3 \
    umoddi3 \
    umodsi3 \
    umodti3

VISIBILITY_HIDDEN := 0
