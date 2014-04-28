#!/bin/sh

LNG=lv

Ball="Bumba"
Goal="Mērķis"
Coin="Monēta"
Switch="Slēdzis"
Texture="Tekstūra"
FallOut="Kritiens"

FONT=FreeMono-Negreta
PTSIZE=56
DENSITY=70

convert \
    \( -size 256x64 xc:transparent -font $FONT -pointsize $PTSIZE \
        -density $DENSITY -channel A -gravity Center \) \
    \( -clone 0 -annotate +0+0 "$Ball"    \) \
    \( -clone 0 -annotate +0+0 "$Goal"    \) \
    \( -clone 0 -annotate +0+0 "$Coin"    \) \
    \( -clone 0 -annotate +0+0 "$Switch"  \) \
    \( -clone 0 -annotate +0+0 "$Texture" \) \
    \( -clone 0 -annotate +0+0 "$FallOut" \) \
    -delete 0 -append \
    -gravity North -background transparent -extent 256x512 \
    -depth 8 words-$LNG.png

