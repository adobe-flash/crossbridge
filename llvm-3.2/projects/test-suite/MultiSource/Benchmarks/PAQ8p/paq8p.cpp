/* paq8p file compressor/archiver.  Release by Andreas Morphis, Aug. 22, 2008

    Copyright (C) 2008 Matt Mahoney, Serge Osnach, Alexander Ratushnyak,
    Bill Pettis, Przemyslaw Skibinski, Matthew Fite, wowtiger, Andrew Paterson,
    Jan Ondrus, Andreas Morphis, Pavel L. Holoborodko, KZ.

    LICENSE

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details at
    Visit <http://www.gnu.org/copyleft/gpl.html>.

To install and use in Windows:

- To install, put paq8p.exe or a shortcut to it on your desktop.
- To compress a file or folder, drop it on the paq8p icon.
- To decompress, drop a .paq8p file on the icon.

A .paq8p extension is added for compression, removed for decompression.
The output will go in the same folder as the input.

While paq8p is working, a command window will appear and report
progress.  When it is done you can close the window by pressing
ENTER or clicking [X]. 


COMMAND LINE INTERFACE

- To install, put paq8p.exe somewhere in your PATH.
- To compress:      paq8p [-N] file1 [file2...]
- To decompress:    paq8p [-d] file1.paq8p [dir2]
- To view contents: more < file1.paq8p

The compressed output file is named by adding ".paq8p" extension to
the first named file (file1.paq8p).  Each file that exists will be
added to the archive and its name will be stored without a path.
The option -N specifies a compression level ranging from -0
(fastest) to -9 (smallest).  The default is -5.  If there is
no option and only one file, then the program will pause when
finished until you press the ENTER key (to support drag and drop).
If file1.paq8p exists then it is overwritten.

If the first named file ends in ".paq8p" then it is assumed to be
an archive and the files within are extracted to the same directory
as the archive unless a different directory (dir2) is specified.
The -d option forces extraction even if there is not a ".paq8p"
extension.  If any output file already exists, then it is compared
with the archive content and the first byte that differs is reported.
No files are overwritten or deleted.  If there is only one argument
(no -d or dir2) then the program will pause when finished until
you press ENTER.

For compression, if any named file is actually a directory, then all
files and subdirectories are compressed, preserving the directory
structure, except that empty directories are not stored, and file
attributes (timestamps, permissions, etc.) are not preserved.
During extraction, directories are created as needed.  For example:

  paq8p -4 c:\tmp\foo bar

compresses foo and bar (if they exist) to c:\tmp\foo.paq8p at level 4.

  paq8p -d c:\tmp\foo.paq8p .

extracts foo and compares bar in the current directory.  If foo and bar
are directories then their contents are extracted/compared.

There are no commands to update an existing archive or to extract
part of an archive.  Files and archives larger than 2GB are not
supported (but might work on 64-bit machines, not tested).
File names with nonprintable characters are not supported (spaces
are OK).


TO COMPILE

There are 2 files: paq8p.cpp (C++) and paq7asm.asm (NASM/YASM).
paq7asm.asm is the same as in paq7 and paq8x.  paq8p.cpp recognizes the
following compiler options:

  -DWINDOWS           (to compile in Windows)
  -DUNIX              (to compile in Unix, Linux, Solairs, MacOS/Darwin, etc)
  -DNOASM             (to replace paq7asm.asm with equivalent C++)
  -DDEFAULT_OPTION=N  (to change the default compression level from 5 to N).

If you compile without -DWINDOWS or -DUNIX, you can still compress files,
but you cannot compress directories or create them during extraction.
You can extract directories if you manually create the empty directories
first.

Use -DEFAULT_OPTION=N to change the default compression level to support
drag and drop on machines with less than 256 MB of memory.  Use
-DDEFAULT_OPTION=4 for 128 MB, 3 for 64 MB, 2 for 32 MB, etc.

Use -DNOASM for non x86-32 machines, or older than a Pentium-MMX (about
1997), or if you don't have NASM or YASM to assemble paq7asm.asm.  The
program will still work but it will be slower.  For NASM in Windows,
use the options "--prefix _" and either "-f win32" or "-f obj" depending
on your C++ compiler.  In Linux, use "-f elf".

Recommended compiler commands and optimizations:

  MINGW g++:
    nasm paq7asm.asm -f win32 --prefix _
    g++ paq8p.cpp -DWINDOWS -O2 -Os -s -march=pentiumpro -fomit-frame-pointer -o paq8p.exe paq7asm.obj

  Borland:
    nasm paq7asm.asm -f obj --prefix _
    bcc32 -DWINDOWS -O -w-8027 paq8p.cpp paq7asm.obj

  Mars:
    nasm paq7asm.asm -f obj --prefix _
    dmc -DWINDOWS -Ae -O paq8p.cpp paq7asm.obj

  UNIX/Linux (PC):
    nasm -f elf paq7asm.asm
    g++ paq8p.cpp -DUNIX -O2 -Os -s -march=pentiumpro -fomit-frame-pointer -o paq8p paq7asm.o

  Non PC (e.g. PowerPC under MacOS X)
    g++ paq8p.cpp -O2 -DUNIX -DNOASM -s -o paq8p

MinGW produces faster executables than Borland or Mars, but Intel 9
is about 4% faster than MinGW).


ARCHIVE FILE FORMAT

An archive has the following format.  It is intended to be both
human and machine readable.  The header ends with CTRL-Z (Windows EOF)
so that the binary compressed data is not displayed on the screen.

  paq8p -N CR LF
  size TAB filename CR LF
  size TAB filename CR LF
  ...
  CTRL-Z
  compressed binary data

-N is the option (-0 to -9), even if a default was used.
Plain file names are stored without a path.  Files in compressed
directories are stored with path relative to the compressed directory
(using UNIX style forward slashes "/").  For example, given these files:

  123 C:\dir1\file1.txt
  456 C:\dir2\file2.txt

Then

  paq8p archive \dir1\file1.txt \dir2

will create archive.paq8p with the header:

  paq8p -5
  123     file1.txt
  456     dir2/file2.txt

The command:

  paq8p archive.paq8p C:\dir3

will create the files:

  C:\dir3\file1.txt
  C:\dir3\dir2\file2.txt

Decompression will fail if the first 7 bytes are not "paq8p -".  Sizes
are stored as decimal numbers.  CR, LF, TAB, CTRL-Z are ASCII codes
13, 10, 9, 26 respectively.


ARITHMETIC CODING

The binary data is arithmetic coded as the shortest base 256 fixed point
number x = SUM_i x_i 256^-1-i such that p(<y) <= x < p(<=y), where y is the
input string, x_i is the i'th coded byte, p(<y) (and p(<=y)) means the
probability that a string is lexicographcally less than (less than
or equal to) y according to the model, _ denotes subscript, and ^ denotes
exponentiation.

The model p(y) for y is a conditional bit stream,
p(y) = PROD_j p(y_j | y_0..j-1) where y_0..j-1 denotes the first j
bits of y, and y_j is the next bit.  Compression depends almost entirely
on the ability to predict the next bit accurately.


MODEL MIXING

paq8p uses a neural network to combine a large number of models.  The
i'th model independently predicts
p1_i = p(y_j = 1 | y_0..j-1), p0_i = 1 - p1_i.
The network computes the next bit probabilty

  p1 = squash(SUM_i w_i t_i), p0 = 1 - p1                        (1)

where t_i = stretch(p1_i) is the i'th input, p1_i is the prediction of
the i'th model, p1 is the output prediction, stretch(p) = ln(p/(1-p)),
and squash(s) = 1/(1+exp(-s)).  Note that squash() and stretch() are
inverses of each other.

After bit y_j (0 or 1) is received, the network is trained:

  w_i := w_i + eta t_i (y_j - p1)                                (2)

where eta is an ad-hoc learning rate, t_i is the i'th input, (y_j - p1)
is the prediction error for the j'th input but, and w_i is the i'th
weight.  Note that this differs from back propagation:

  w_i := w_i + eta t_i (y_j - p1) p0 p1                          (3)

which is a gradient descent in weight space to minimize root mean square
error.  Rather, the goal in compression is to minimize coding cost,
which is -log(p0) if y = 1 or -log(p1) if y = 0.  Taking
the partial derivative of cost with respect to w_i yields (2).


MODELS

Most models are context models.  A function of the context (last few
bytes) is mapped by a lookup table or hash table to a state which depends
on the bit history (prior sequence of 0 and 1 bits seen in this context).
The bit history is then mapped to p1_i by a fixed or adaptive function.
There are several types of bit history states:

- Run Map. The state is (b,n) where b is the last bit seen (0 or 1) and
  n is the number of consecutive times this value was seen.  The initial
  state is (0,0).  The output is computed directly:

    t_i = (2b - 1)K log(n + 1).

  where K is ad-hoc, around 4 to 10.  When bit y_j is seen, the state
  is updated:

    (b,n) := (b,n+1) if y_j = b, else (y_j,1).

- Stationary Map.  The state is p, initially 1/2.  The output is
  t_i = stretch(p).  The state is updated at ad-hoc rate K (around 0.01):

    p := p + K(y_j - p)

- Nonstationary Map.  This is a compromise between a stationary map, which
  assumes uniform statistics, and a run map, which adapts quickly by
  discarding old statistics.  An 8 bit state represents (n0,n1,h), initially
  (0,0,0) where:

    n0 is the number of 0 bits seen "recently".
    n1 is the number of 1 bits seen "recently".
    n = n0 + n1.
    h is the full bit history for 0 <= n <= 4,
      the last bit seen (0 or 1) if 5 <= n <= 15,
      0 for n >= 16.

  The primaty output is t_i := stretch(sm(n0,n1,h)), where sm(.) is
  a stationary map with K = 1/256, initiaized to 
  sm(n0,n1,h) = (n1+(1/64))/(n+2/64).  Four additional inputs are also 
  be computed to improve compression slightly:

    p1_i = sm(n0,n1,h)
    p0_i = 1 - p1_i
    t_i   := stretch(p_1)
    t_i+1 := K1 (p1_i - p0_i)
    t_i+2 := K2 stretch(p1) if n0 = 0, -K2 stretch(p1) if n1 = 0, else 0
    t_i+3 := K3 (-p0_i if n1 = 0, p1_i if n0 = 0, else 0)
    t_i+4 := K3 (-p0_i if n0 = 0, p1_i if n1 = 0, else 0)

  where K1..K4 are ad-hoc constants.

  h is updated as follows:
    If n < 4, append y_j to h.
    Else if n <= 16, set h := y_j.
    Else h = 0.

  The update rule is biased toward newer data in a way that allows
  n0 or n1, but not both, to grow large by discarding counts of the
  opposite bit.  Large counts are incremented probabilistically.
  Specifically, when y_j = 0 then the update rule is:

    n0 := n0 + 1, n < 29
          n0 + 1 with probability 2^(27-n0)/2 else n0, 29 <= n0 < 41
          n0, n = 41.
    n1 := n1, n1 <= 5
          round(8/3 lg n1), if n1 > 5

  swapping (n0,n1) when y_j = 1.

  Furthermore, to allow an 8 bit representation for (n0,n1,h), states
  exceeding the following values of n0 or n1 are replaced with the
  state with the closest ratio n0:n1 obtained by decrementing the
  smaller count: (41,0,h), (40,1,h), (12,2,h), (5,3,h), (4,4,h),
  (3,5,h), (2,12,h), (1,40,h), (0,41,h).  For example:
  (12,2,1) 0-> (7,1,0) because there is no state (13,2,0).

- Match Model.  The state is (c,b), initially (0,0), where c is 1 if
  the context was previously seen, else 0, and b is the next bit in
  this context.  The prediction is:

    t_i := (2b - 1)Kc log(m + 1)

  where m is the length of the context.  The update rule is c := 1,
  b := y_j.  A match model can be implemented efficiently by storing
  input in a buffer and storing pointers into the buffer into a hash
  table indexed by context.  Then c is indicated by a hash table entry
  and b can be retrieved from the buffer.


CONTEXTS

High compression is achieved by combining a large number of contexts.
Most (not all) contexts start on a byte boundary and end on the bit
immediately preceding the predicted bit.  The contexts below are
modeled with both a run map and a nonstationary map unless indicated.

- Order n.  The last n bytes, up to about 16.  For general purpose data.
  Most of the compression occurs here for orders up to about 6.
  An order 0 context includes only the 0-7 bits of the partially coded
  byte and the number of these bits (255 possible values).

- Sparse.  Usually 1 or 2 of the last 8 bytes preceding the byte containing
  the predicted bit, e.g (2), (3),..., (8), (1,3), (1,4), (1,5), (1,6),
  (2,3), (2,4), (3,6), (4,8).  The ordinary order 1 and 2 context, (1)
  or (1,2) are included above.  Useful for binary data.

- Text.  Contexts consists of whole words (a-z, converted to lower case
  and skipping other values).  Contexts may be sparse, e.g (0,2) meaning
  the current (partially coded) word and the second word preceding the
  current one.  Useful contexts are (0), (0,1), (0,1,2), (0,2), (0,3),
  (0,4).  The preceding byte may or may not be included as context in the
  current word.

- Formatted text.  The column number (determined by the position of
  the last linefeed) is combined with other contexts: the charater to
  the left and the character above it.

- Fixed record length.  The record length is determined by searching for
  byte sequences with a uniform stride length.  Once this is found, then
  the record length is combined with the context of the bytes immediately
  preceding it and the corresponding byte locations in the previous
  one or two records (as with formatted text).

- Context gap.  The distance to the previous occurrence of the order 1
  or order 2 context is combined with other low order (1-2) contexts.

- FAX.  For 2-level bitmapped images.  Contexts are the surrounding
  pixels already seen.  Image width is assumed to be 1728 bits (as
  in calgary/pic).

- Image.  For uncompressed 24-bit color BMP and TIFF images.  Contexts
  are the high order bits of the surrounding pixels and linear
  combinations of those pixels, including other color planes.  The
  image width is detected from the file header.  When an image is
  detected, other models are turned off to improve speed.

- JPEG.  Files are further compressed by partially uncompressing back
  to the DCT coefficients to provide context for the next Huffman code.
  Only baseline DCT-Huffman coded files are modeled.  (This ia about
  90% of images, the others are usually progresssive coded).  JPEG images
  embedded in other files (quite common) are detected by headers.  The
  baseline JPEG coding process is:
  - Convert to grayscale and 2 chroma colorspace.
  - Sometimes downsample the chroma images 2:1 or 4:1 in X and/or Y.
  - Divide each of the 3 images into 8x8 blocks.
  - Convert using 2-D discrete cosine transform (DCT) to 64 12-bit signed
    coefficients.
  - Quantize the coefficients by integer division (lossy).
  - Split the image into horizontal slices coded independently, separated
    by restart codes.
  - Scan each block starting with the DC (0,0) coefficient in zigzag order
    to the (7,7) coefficient, interleaving the 3 color components in
    order to scan the whole image left to right starting at the top.
  - Subtract the previous DC component from the current in each color.
  - Code the coefficients using RS codes, where R is a run of R zeros (0-15)
    and S indicates 0-11 bits of a signed value to follow.  (There is a
    special RS code (EOB) to indicate the rest of the 64 coefficients are 0).
  - Huffman code the RS symbol, followed by S literal bits.
  The most useful contexts are the current partially coded Huffman code
  (including S following bits) combined with the coefficient position
  (0-63), color (0-2), and last few RS codes.

- Match.  When a context match of 400 bytes or longer is detected,
  the next bit of the match is predicted and other models are turned
  off to improve speed.

- Exe.  When a x86 file (.exe, .obj, .dll) is detected, sparse contexts
  with gaps of 1-12 selecting only the prefix, opcode, and the bits
  of the modR/M byte that are relevant to parsing are selected.
  This model is turned off otherwise.

- Indirect.  The history of the last 1-3 bytes in the context of the
  last 1-2 bytes is combined with this 1-2 byte context.

- DMC. A bitwise n-th order context is built from a state machine using
  DMC, described in http://plg.uwaterloo.ca/~ftp/dmc/dmc.c
  The effect is to extend a single context, one bit at a time and predict
  the next bit based on the history in this context.  The model here differs
  in that two predictors are used.  One is a pair of counts as in the original
  DMC.  The second predictor is a bit history state mapped adaptively to
  a probability as as in a Nonstationary Map.

ARCHITECTURE

The context models are mixed by several of several hundred neural networks
selected by a low-order context.  The outputs of these networks are
combined using a second neural network, then fed through several stages of 
adaptive probability maps (APM) before arithmetic coding.

For images, only one neural network is used and its context is fixed.

An APM is a stationary map combining a context and an input probability.
The input probability is stretched and divided into 32 segments to
combine with other contexts.  The output is interpolated between two
adjacent quantized values of stretch(p1).  There are 2 APM stages in series:

  p1 := (p1 + 3 APM(order 0, p1)) / 4.
  p1 := (APM(order 1, p1) + 2 APM(order 2, p1) + APM(order 3, p1)) / 4.

PREPROCESSING

paq8p uses preprocessing transforms on certain data types to improve
compression.  To improve reliability, the decoding transform is
tested during compression to ensure that the input file can be
restored.  If the decoder output is not identical to the input file
due to a bug, then the transform is abandoned and the data is compressed
without a transform so that it will still decompress correctly.

The input is split into blocks with the format <type> <decoded size> <data>
where <type> is 1 byte (0 = no transform), <decoded size> is the size
of the data after decoding, which may be different than the size of <data>.
Blocks do not span file boundaries, and have a maximum size of 4MB to
2GB depending on compression level.  Large files are split into blocks
of this size.  The preprocessor has 3 parts:

- Detector.  Splits the input into smaller blocks depending on data type.

- Coder.  Input is a block to be compressed.  Output is a temporary
  file.  The coder determines whether a transform is to be applied
  based on file type, and if so, which one.  A coder may use lots
  of resources (memory, time) and make multiple passes through the
  input file.  The file type is stored (as one byte) during compression.

- Decoder.  Performs the inverse transform of the coder.  It uses few
  resorces (fast, low memory) and runs in a single pass (stream oriented).
  It takes input either from a file or the arithmetic decoder.  Each call
  to the decoder returns a single decoded byte.

The following transforms are used:

- EXE:  CALL (0xE8) and JMP (0xE9) address operands are converted from
  relative to absolute address.  The transform is to replace the sequence
  E8/E9 xx xx xx 00/FF by adding file offset modulo 2^25 (signed range,
  little-endian format).  Data to transform is identified by trying the
  transform and applying a crude compression test: testing whether the
  byte following the E8/E8 (LSB of the address) occurred more recently
  in the transformed data than the original and within 4KB 4 times in
  a row.  The block ends when this does not happen for 4KB.

- JPEG: detected by SOI and SOF and ending with EOI or any nondecodable
  data.  No transform is applied.  The purpose is to separate images
  embedded in execuables to block the EXE transform, and for a future
  place to insert a transform.


IMPLEMENTATION

Hash tables are designed to minimize cache misses, which consume most
of the CPU time.

Most of the memory is used by the nonstationary context models.
Contexts are represented by 32 bits, possibly a hash.  These are
mapped to a bit history, represented by 1 byte.  The hash table is
organized into 64-byte buckets on cache line boundaries.  Each bucket
contains 7 x 7 bit histories, 7 16-bit checksums, and a 2 element LRU
queue packed into one byte.  Each 7 byte element represents 7 histories
for a context ending on a 3-bit boundary plus 0-2 more bits.  One
element (for bits 0-1, which have 4 unused bytes) also contains a run model 
consisting of the last byte seen and a count (as 1 byte each).

Run models use 4 byte hash elements consisting of a 2 byte checksum, a
repeat count (0-255) and the byte value.  The count also serves as
a priority.

Stationary models are most appropriate for small contexts, so the
context is used as a direct table lookup without hashing.

The match model maintains a pointer to the last match until a mismatching
bit is found.  At the start of the next byte, the hash table is referenced
to find another match.  The hash table of pointers is updated after each
whole byte.  There is no checksum.  Collisions are detected by comparing
the current and matched context in a rotating buffer.

The inner loops of the neural network prediction (1) and training (2)
algorithms are implemented in MMX assembler, which computes 4 elements
at a time.  Using assembler is 8 times faster than C++ for this code
and 1/3 faster overall.  (However I found that SSE2 code on an AMD-64,
which computes 8 elements at a time, is not any faster).


DIFFERENCES FROM PAQ7

An .exe model and filter are added.  Context maps are improved using 16-bit
checksums to reduce collisions.  The state table uses probabilistic updates
for large counts, more states that remember the last bit, and decreased
discounting of the opposite count.  It is implemented as a fixed table.
There are also many minor changes.

DIFFERENCES FROM PAQ8A

The user interface supports directory compression and drag and drop.
The preprocessor segments the input into blocks and uses more robust
EXE detection.  An indirect context model was added.  There is no
dictionary preprocesor like PAQ8B/C/D/E.

DIFFERENCES FROM PAQ8F

Different models, usually from paq8hp*. Also changed rate from 8 to 7. A bug
in Array was fixed that caused the program to silently crash upon exit.

DIFFERENCES FROM PAQ8J

1) Slightly improved sparse model. 
2) Added new family of sparse contexts. Each byte mapped to 3-bit value, where
different values corresponds to different byte classes. For example, input
byte 0x00 transformed into 0, all bytes that less then 16 -- into 5, all 
punctuation marks (ispunct(c)!=0) -- into 2 etc. Then this flags from 11 
previous bytes combined into 32-bit pseudo-context.

All this improvements gives only 62 byte on BOOK1, but on binaries archive size
reduced on 1-2%.

DIFFERENCES FROM PAQ8JA

Introduced distance model. Distance model uses distance to last occurence
of some anchor char ( 0x00, space, newline, 0xff ), combined with previous
charactes as context. This slightly improves compression of files with
variable-width record data.

DIFFERENCES FROM PAQ8JB

Restored recordModel(), broken in paq8hp*. Slightly tuned indirectModel(). 

DIFFERENCES FROM PAQ8JC

Changed the APMs in the Predictor. Up to a 0.2% improvement for some files.

DIFFERENCES FROM PAQ8JD

Added DMCModel.  Removed some redundant models from SparseModel and other
minor tuneups.  Changes introduced in PAQ8K were not carried over.

PAQ8L v.2

Changed Mixer::p() to p() to fix a compiler error in Linux
(patched by Indrek Kruusa, Apr. 15, 2007).

DIFFERENCES FROM PAQ8L, PAQ8M

Modified JPEG model by Jan Ondrus (paq8fthis2).  The new model improves
compression by using decoded pixel values of current and adjacent blocks
as context.  PAQ8M was an earlier version of the new JPEG model
(from paq8fthis).

DIFFERENCES FROM PAQ8N

Improved bmp model. Slightly faster.

DIFFERENCES FROM PAQ8O

Modified JPEG model by Jan Ondrus (paq8fthis4).
Added PGM (grayscale image) model form PAQ8I.
Added grayscale BMP model to PGM model.
Ver. 2 can be compiled using either old or new "for" loop scoping rules.
Added APM and StateMap from LPAQ1
Code optimizations from Enrico Zeidler 
Detection of BMP 4,8,24 bit and PGM 8 bit images before compress
On non BMP,PGM,JPEG data mem is lower
Fixed bug in BMP 8-bit detection in other files like .exe
15. oct 2007
Updates JPEG model by Jan Ondrus
PGM detection bug fix
22. oct 2007
improved JPEG model by Jan Ondrus
16. feb 2008
fixed bmp detection bug
added .rgb file support (uncompressed grayscale)

DIFFERENCES FROM PAQ8O9

Added wav Model. Slightly improved bmp model.
*/

#define PROGNAME "paq8p"  // Please change this if you change the program.

#ifdef LLVM
#include <unistd.h>
#include <sys/wait.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#ifndef NDEBUG
#define NDEBUG  // remove for debugging (turns on Array bound checks)
#endif
#include <assert.h>

#ifdef UNIX
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#endif

#ifdef WINDOWS
#include <windows.h>
#endif

#ifndef DEFAULT_OPTION
#define DEFAULT_OPTION 5
#endif

// 8, 16, 32 bit unsigned types (adjust as appropriate)
typedef unsigned char  U8;
typedef unsigned short U16;
typedef unsigned int   U32;

// min, max functions
#ifndef WINDOWS
inline int min(int a, int b) {return a<b?a:b;}
inline int max(int a, int b) {return a<b?b:a;}
#endif

static const char *mybasename(const char *str) {
  const char *base = strrchr(str, '/');
  return base ? base+1 : str;
}

// Error handler: print message if any, and exit
void quit(const char* message=0) {
  throw message;
}

// strings are equal ignoring case?
int equals(const char* a, const char* b) {
  assert(a && b);
  while (*a && *b) {
    int c1=*a;
    if (c1>='A'&&c1<='Z') c1+='a'-'A';
    int c2=*b;
    if (c2>='A'&&c2<='Z') c2+='a'-'A';
    if (c1!=c2) return 0;
    ++a;
    ++b;
  }
  return *a==*b;
}

//////////////////////// Program Checker /////////////////////

// Track time and memory used
class ProgramChecker {
  int memused;  // bytes allocated by Array<T> now
  int maxmem;   // most bytes allocated ever
  clock_t start_time;  // in ticks
public:
  void alloc(int n) {  // report memory allocated, may be negative
    memused+=n;
    if (memused>maxmem) maxmem=memused;
  }
  ProgramChecker(): memused(0), maxmem(0) {
    start_time=clock();
    assert(sizeof(U8)==1);
    assert(sizeof(U16)==2);
    assert(sizeof(U32)==4);
    assert(sizeof(short)==2);
    assert(sizeof(int)==4);
  }
  void print() const {  // print time and memory used
#ifndef LLVM
    printf("Time %1.2f sec, used %d bytes of memory\n",
      double(clock()-start_time)/CLOCKS_PER_SEC, maxmem);
#endif
  }
} programChecker;

//////////////////////////// Array ////////////////////////////

// Array<T, ALIGN> a(n); creates n elements of T initialized to 0 bits.
// Constructors for T are not called.
// Indexing is bounds checked if assertions are on.
// a.size() returns n.
// a.resize(n) changes size to n, padding with 0 bits or truncating.
// a.push_back(x) appends x and increases size by 1, reserving up to size*2.
// a.pop_back() decreases size by 1, does not free memory.
// Copy and assignment are not supported.
// Memory is aligned on a ALIGN byte boundary (power of 2), default is none.

template <class T, int ALIGN=0> class Array {
private:
  int n;     // user size
  int reserved;  // actual size
  char *ptr; // allocated memory, zeroed
  T* data;   // start of n elements of aligned data
  void create(int i);  // create with size i
public:
  explicit Array(int i=0) {create(i);}
  ~Array();
  T& operator[](int i) {
#ifndef NDEBUG
    if (i<0 || i>=n) fprintf(stderr, "%d out of bounds %d\n", i, n), quit();
#endif
    return data[i];
  }
  const T& operator[](int i) const {
#ifndef NDEBUG
    if (i<0 || i>=n) fprintf(stderr, "%d out of bounds %d\n", i, n), quit();
#endif
    return data[i];
  }
  int size() const {return n;}
  void resize(int i);  // change size to i
  void pop_back() {if (n>0) --n;}  // decrement size
  void push_back(const T& x);  // increment size, append x
private:
  Array(const Array&);  // no copy or assignment
  Array& operator=(const Array&);
};

template<class T, int ALIGN> void Array<T, ALIGN>::resize(int i) {
  if (i<=reserved) {
    n=i;
    return;
  }
  char *saveptr=ptr;
  T *savedata=data;
  int saven=n;
  create(i);
  if (saveptr) {
    if (savedata) {
      memcpy(data, savedata, sizeof(T)*min(i, saven));
      programChecker.alloc(-ALIGN-n*sizeof(T));
    }
    free(saveptr);
  }
}

template<class T, int ALIGN> void Array<T, ALIGN>::create(int i) {
  n=reserved=i;
  if (i<=0) {
    data=0;
    ptr=0;
    return;
  }
  const int sz=ALIGN+n*sizeof(T);
  programChecker.alloc(sz);
  ptr = (char*)calloc(sz, 1);
  if (!ptr) quit("Out of memory");
  data = (ALIGN ? (T*)(ptr+ALIGN-(((long)ptr)&(ALIGN-1))) : (T*)ptr);
  assert((char*)data>=ptr && (char*)data<=ptr+ALIGN);
}

template<class T, int ALIGN> Array<T, ALIGN>::~Array() {
  programChecker.alloc(-ALIGN-n*sizeof(T));
  free(ptr);
}

template<class T, int ALIGN> void Array<T, ALIGN>::push_back(const T& x) {
  if (n==reserved) {
    int saven=n;
    resize(max(1, n*2));
    n=saven;
  }
  data[n++]=x;
}

/////////////////////////// String /////////////////////////////

// A tiny subset of std::string
// size() includes NUL terminator.

class String: public Array<char> {
public:
  const char* c_str() const {return &(*this)[0];}
  void operator=(const char* s) {
    resize(strlen(s)+1);
    strcpy(&(*this)[0], s);
  }
  void operator+=(const char* s) {
    assert(s);
    pop_back();
    while (*s) push_back(*s++);
    push_back(0);
  }
  String(const char* s=""): Array<char>(1) {
    (*this)+=s;
  }
};


//////////////////////////// rnd ///////////////////////////////

// 32-bit pseudo random number generator
class Random{
  Array<U32> table;
  int i;
public:
  Random(): table(64) {
    table[0]=123456789;
    table[1]=987654321;
    for(int j=0; j<62; j++) table[j+2]=table[j+1]*11+table[j]*23/16;
    i=0;
  }
  U32 operator()() {
    return ++i, table[i&63]=table[i-24&63]^table[i-55&63];
  }
} rnd;

////////////////////////////// Buf /////////////////////////////

// Buf(n) buf; creates an array of n bytes (must be a power of 2).
// buf[i] returns a reference to the i'th byte with wrap (no out of bounds).
// buf(i) returns i'th byte back from pos (i > 0) 
// buf.size() returns n.

int pos;  // Number of input bytes in buf (not wrapped)

class Buf {
  Array<U8> b;
public:
  Buf(int i=0): b(i) {}
  void setsize(int i) {
    if (!i) return;
    assert(i>0 && (i&(i-1))==0);
    b.resize(i);
  }
  U8& operator[](int i) {
    return b[i&b.size()-1];
  }
  int operator()(int i) const {
    assert(i>0);
    return b[pos-i&b.size()-1];
  }
  int size() const {
    return b.size();
  }
};

// IntBuf(n) is a buffer of n int (must be a power of 2).
// intBuf[i] returns a reference to i'th element with wrap.

class IntBuf {
  Array<int> b;
public:
  IntBuf(int i=0): b(i) {}
  int& operator[](int i) {
    return b[i&b.size()-1];
  }
};

/////////////////////// Global context /////////////////////////

int level=DEFAULT_OPTION;  // Compression level 0 to 9
#define MEM (0x10000<<level)
int y=0;  // Last bit, 0 or 1, set by encoder

// Global context set by Predictor and available to all models.
int c0=1; // Last 0-7 bits of the partial byte with a leading 1 bit (1-255)
U32 c4=0; // Last 4 whole bytes, packed.  Last byte is bits 0-7.
int bpos=0; // bits in c0 (0 to 7)
Buf buf;  // Rotating input queue set by Predictor

///////////////////////////// ilog //////////////////////////////

// ilog(x) = round(log2(x) * 16), 0 <= x < 64K
class Ilog {
  Array<U8> t;
public:
  int operator()(U16 x) const {return t[x];}
  Ilog();
} ilog;

// Compute lookup table by numerical integration of 1/x
Ilog::Ilog(): t(65536) {
  U32 x=14155776;
  for (int i=2; i<65536; ++i) {
    x+=774541002/(i*2-1);  // numerator is 2^29/ln 2
    t[i]=x>>24;
  }
}

// llog(x) accepts 32 bits
inline int llog(U32 x) {
  if (x>=0x1000000)
    return 256+ilog(x>>16);
  else if (x>=0x10000)
    return 128+ilog(x>>8);
  else
    return ilog(x);
}

///////////////////////// state table ////////////////////////

// State table:
//   nex(state, 0) = next state if bit y is 0, 0 <= state < 256
//   nex(state, 1) = next state if bit y is 1
//   nex(state, 2) = number of zeros in bit history represented by state
//   nex(state, 3) = number of ones represented
//
// States represent a bit history within some context.
// State 0 is the starting state (no bits seen).
// States 1-30 represent all possible sequences of 1-4 bits.
// States 31-252 represent a pair of counts, (n0,n1), the number
//   of 0 and 1 bits respectively.  If n0+n1 < 16 then there are
//   two states for each pair, depending on if a 0 or 1 was the last
//   bit seen.
// If n0 and n1 are too large, then there is no state to represent this
// pair, so another state with about the same ratio of n0/n1 is substituted.
// Also, when a bit is observed and the count of the opposite bit is large,
// then part of this count is discarded to favor newer data over old.

#if 1 // change to #if 0 to generate this table at run time (4% slower)
static const U8 State_table[256][4]={
  {  1,  2, 0, 0},{  3,  5, 1, 0},{  4,  6, 0, 1},{  7, 10, 2, 0}, // 0-3
  {  8, 12, 1, 1},{  9, 13, 1, 1},{ 11, 14, 0, 2},{ 15, 19, 3, 0}, // 4-7
  { 16, 23, 2, 1},{ 17, 24, 2, 1},{ 18, 25, 2, 1},{ 20, 27, 1, 2}, // 8-11
  { 21, 28, 1, 2},{ 22, 29, 1, 2},{ 26, 30, 0, 3},{ 31, 33, 4, 0}, // 12-15
  { 32, 35, 3, 1},{ 32, 35, 3, 1},{ 32, 35, 3, 1},{ 32, 35, 3, 1}, // 16-19
  { 34, 37, 2, 2},{ 34, 37, 2, 2},{ 34, 37, 2, 2},{ 34, 37, 2, 2}, // 20-23
  { 34, 37, 2, 2},{ 34, 37, 2, 2},{ 36, 39, 1, 3},{ 36, 39, 1, 3}, // 24-27
  { 36, 39, 1, 3},{ 36, 39, 1, 3},{ 38, 40, 0, 4},{ 41, 43, 5, 0}, // 28-31
  { 42, 45, 4, 1},{ 42, 45, 4, 1},{ 44, 47, 3, 2},{ 44, 47, 3, 2}, // 32-35
  { 46, 49, 2, 3},{ 46, 49, 2, 3},{ 48, 51, 1, 4},{ 48, 51, 1, 4}, // 36-39
  { 50, 52, 0, 5},{ 53, 43, 6, 0},{ 54, 57, 5, 1},{ 54, 57, 5, 1}, // 40-43
  { 56, 59, 4, 2},{ 56, 59, 4, 2},{ 58, 61, 3, 3},{ 58, 61, 3, 3}, // 44-47
  { 60, 63, 2, 4},{ 60, 63, 2, 4},{ 62, 65, 1, 5},{ 62, 65, 1, 5}, // 48-51
  { 50, 66, 0, 6},{ 67, 55, 7, 0},{ 68, 57, 6, 1},{ 68, 57, 6, 1}, // 52-55
  { 70, 73, 5, 2},{ 70, 73, 5, 2},{ 72, 75, 4, 3},{ 72, 75, 4, 3}, // 56-59
  { 74, 77, 3, 4},{ 74, 77, 3, 4},{ 76, 79, 2, 5},{ 76, 79, 2, 5}, // 60-63
  { 62, 81, 1, 6},{ 62, 81, 1, 6},{ 64, 82, 0, 7},{ 83, 69, 8, 0}, // 64-67
  { 84, 71, 7, 1},{ 84, 71, 7, 1},{ 86, 73, 6, 2},{ 86, 73, 6, 2}, // 68-71
  { 44, 59, 5, 3},{ 44, 59, 5, 3},{ 58, 61, 4, 4},{ 58, 61, 4, 4}, // 72-75
  { 60, 49, 3, 5},{ 60, 49, 3, 5},{ 76, 89, 2, 6},{ 76, 89, 2, 6}, // 76-79
  { 78, 91, 1, 7},{ 78, 91, 1, 7},{ 80, 92, 0, 8},{ 93, 69, 9, 0}, // 80-83
  { 94, 87, 8, 1},{ 94, 87, 8, 1},{ 96, 45, 7, 2},{ 96, 45, 7, 2}, // 84-87
  { 48, 99, 2, 7},{ 48, 99, 2, 7},{ 88,101, 1, 8},{ 88,101, 1, 8}, // 88-91
  { 80,102, 0, 9},{103, 69,10, 0},{104, 87, 9, 1},{104, 87, 9, 1}, // 92-95
  {106, 57, 8, 2},{106, 57, 8, 2},{ 62,109, 2, 8},{ 62,109, 2, 8}, // 96-99
  { 88,111, 1, 9},{ 88,111, 1, 9},{ 80,112, 0,10},{113, 85,11, 0}, // 100-103
  {114, 87,10, 1},{114, 87,10, 1},{116, 57, 9, 2},{116, 57, 9, 2}, // 104-107
  { 62,119, 2, 9},{ 62,119, 2, 9},{ 88,121, 1,10},{ 88,121, 1,10}, // 108-111
  { 90,122, 0,11},{123, 85,12, 0},{124, 97,11, 1},{124, 97,11, 1}, // 112-115
  {126, 57,10, 2},{126, 57,10, 2},{ 62,129, 2,10},{ 62,129, 2,10}, // 116-119
  { 98,131, 1,11},{ 98,131, 1,11},{ 90,132, 0,12},{133, 85,13, 0}, // 120-123
  {134, 97,12, 1},{134, 97,12, 1},{136, 57,11, 2},{136, 57,11, 2}, // 124-127
  { 62,139, 2,11},{ 62,139, 2,11},{ 98,141, 1,12},{ 98,141, 1,12}, // 128-131
  { 90,142, 0,13},{143, 95,14, 0},{144, 97,13, 1},{144, 97,13, 1}, // 132-135
  { 68, 57,12, 2},{ 68, 57,12, 2},{ 62, 81, 2,12},{ 62, 81, 2,12}, // 136-139
  { 98,147, 1,13},{ 98,147, 1,13},{100,148, 0,14},{149, 95,15, 0}, // 140-143
  {150,107,14, 1},{150,107,14, 1},{108,151, 1,14},{108,151, 1,14}, // 144-147
  {100,152, 0,15},{153, 95,16, 0},{154,107,15, 1},{108,155, 1,15}, // 148-151
  {100,156, 0,16},{157, 95,17, 0},{158,107,16, 1},{108,159, 1,16}, // 152-155
  {100,160, 0,17},{161,105,18, 0},{162,107,17, 1},{108,163, 1,17}, // 156-159
  {110,164, 0,18},{165,105,19, 0},{166,117,18, 1},{118,167, 1,18}, // 160-163
  {110,168, 0,19},{169,105,20, 0},{170,117,19, 1},{118,171, 1,19}, // 164-167
  {110,172, 0,20},{173,105,21, 0},{174,117,20, 1},{118,175, 1,20}, // 168-171
  {110,176, 0,21},{177,105,22, 0},{178,117,21, 1},{118,179, 1,21}, // 172-175
  {110,180, 0,22},{181,115,23, 0},{182,117,22, 1},{118,183, 1,22}, // 176-179
  {120,184, 0,23},{185,115,24, 0},{186,127,23, 1},{128,187, 1,23}, // 180-183
  {120,188, 0,24},{189,115,25, 0},{190,127,24, 1},{128,191, 1,24}, // 184-187
  {120,192, 0,25},{193,115,26, 0},{194,127,25, 1},{128,195, 1,25}, // 188-191
  {120,196, 0,26},{197,115,27, 0},{198,127,26, 1},{128,199, 1,26}, // 192-195
  {120,200, 0,27},{201,115,28, 0},{202,127,27, 1},{128,203, 1,27}, // 196-199
  {120,204, 0,28},{205,115,29, 0},{206,127,28, 1},{128,207, 1,28}, // 200-203
  {120,208, 0,29},{209,125,30, 0},{210,127,29, 1},{128,211, 1,29}, // 204-207
  {130,212, 0,30},{213,125,31, 0},{214,137,30, 1},{138,215, 1,30}, // 208-211
  {130,216, 0,31},{217,125,32, 0},{218,137,31, 1},{138,219, 1,31}, // 212-215
  {130,220, 0,32},{221,125,33, 0},{222,137,32, 1},{138,223, 1,32}, // 216-219
  {130,224, 0,33},{225,125,34, 0},{226,137,33, 1},{138,227, 1,33}, // 220-223
  {130,228, 0,34},{229,125,35, 0},{230,137,34, 1},{138,231, 1,34}, // 224-227
  {130,232, 0,35},{233,125,36, 0},{234,137,35, 1},{138,235, 1,35}, // 228-231
  {130,236, 0,36},{237,125,37, 0},{238,137,36, 1},{138,239, 1,36}, // 232-235
  {130,240, 0,37},{241,125,38, 0},{242,137,37, 1},{138,243, 1,37}, // 236-239
  {130,244, 0,38},{245,135,39, 0},{246,137,38, 1},{138,247, 1,38}, // 240-243
  {140,248, 0,39},{249,135,40, 0},{250, 69,39, 1},{ 80,251, 1,39}, // 244-247
  {140,252, 0,40},{249,135,41, 0},{250, 69,40, 1},{ 80,251, 1,40}, // 248-251
  {140,252, 0,41}};  // 252, 253-255 are reserved

#define nex(state,sel) State_table[state][sel]

// The code used to generate the above table at run time (4% slower).
// To print the table, uncomment the 4 lines of print statements below.
// In this code x,y = n0,n1 is the number of 0,1 bits represented by a state.
#else

class StateTable {
  Array<U8> ns;  // state*4 -> next state if 0, if 1, n0, n1
  enum {B=5, N=64}; // sizes of b, t
  static const int b[B];  // x -> max y, y -> max x
  static U8 t[N][N][2];  // x,y -> state number, number of states
  int num_states(int x, int y);  // compute t[x][y][1]
  void discount(int& x);  // set new value of x after 1 or y after 0
  void next_state(int& x, int& y, int b);  // new (x,y) after bit b
public:
  int operator()(int state, int sel) {return ns[state*4+sel];}
  StateTable();
} nex;

const int StateTable::b[B]={42,41,13,6,5};  // x -> max y, y -> max x
U8 StateTable::t[N][N][2];

int StateTable::num_states(int x, int y) {
  if (x<y) return num_states(y, x);
  if (x<0 || y<0 || x>=N || y>=N || y>=B || x>=b[y]) return 0;

  // States 0-30 are a history of the last 0-4 bits
  if (x+y<=4) {  // x+y choose x = (x+y)!/x!y!
    int r=1;
    for (int i=x+1; i<=x+y; ++i) r*=i;
    for (int i=2; i<=y; ++i) r/=i;
    return r;
  }

  // States 31-255 represent a 0,1 count and possibly the last bit
  // if the state is reachable by either a 0 or 1.
  else
    return 1+(y>0 && x+y<16);
}

// New value of count x if the opposite bit is observed
void StateTable::discount(int& x) {
  if (x>2) x=ilog(x)/6-1;
}

// compute next x,y (0 to N) given input b (0 or 1)
void StateTable::next_state(int& x, int& y, int b) {
  if (x<y)
    next_state(y, x, 1-b);
  else {
    if (b) {
      ++y;
      discount(x);
    }
    else {
      ++x;
      discount(y);
    }
    while (!t[x][y][1]) {
      if (y<2) --x;
      else {
        x=(x*(y-1)+(y/2))/y;
        --y;
      }
    }
  }
}

// Initialize next state table ns[state*4] -> next if 0, next if 1, x, y
StateTable::StateTable(): ns(1024) {

  // Assign states
  int state=0;
  for (int i=0; i<256; ++i) {
    for (int y=0; y<=i; ++y) {
      int x=i-y;
      int n=num_states(x, y);
      if (n) {
        t[x][y][0]=state;
        t[x][y][1]=n;
        state+=n;
      }
    }
  }

  // Print/generate next state table
  state=0;
  for (int i=0; i<N; ++i) {
    for (int y=0; y<=i; ++y) {
      int x=i-y;
      for (int k=0; k<t[x][y][1]; ++k) {
        int x0=x, y0=y, x1=x, y1=y;  // next x,y for input 0,1
        int ns0=0, ns1=0;
        if (state<15) {
          ++x0;
          ++y1;
          ns0=t[x0][y0][0]+state-t[x][y][0];
          ns1=t[x1][y1][0]+state-t[x][y][0];
          if (x>0) ns1+=t[x-1][y+1][1];
          ns[state*4]=ns0;
          ns[state*4+1]=ns1;
          ns[state*4+2]=x;
          ns[state*4+3]=y;
        }
        else if (t[x][y][1]) {
          next_state(x0, y0, 0);
          next_state(x1, y1, 1);
          ns[state*4]=ns0=t[x0][y0][0];
          ns[state*4+1]=ns1=t[x1][y1][0]+(t[x1][y1][1]>1);
          ns[state*4+2]=x;
          ns[state*4+3]=y;
        }
          // uncomment to print table above
//        printf("{%3d,%3d,%2d,%2d},", ns[state*4], ns[state*4+1], 
//          ns[state*4+2], ns[state*4+3]);
//        if (state%4==3) printf(" // %d-%d\n  ", state-3, state);
        assert(state>=0 && state<256);
        assert(t[x][y][1]>0);
        assert(t[x][y][0]<=state);
        assert(t[x][y][0]+t[x][y][1]>state);
        assert(t[x][y][1]<=6);
        assert(t[x0][y0][1]>0);
        assert(t[x1][y1][1]>0);
        assert(ns0-t[x0][y0][0]<t[x0][y0][1]);
        assert(ns0-t[x0][y0][0]>=0);
        assert(ns1-t[x1][y1][0]<t[x1][y1][1]);
        assert(ns1-t[x1][y1][0]>=0);
        ++state;
      }
    }
  }
//  printf("%d states\n", state); exit(0);  // uncomment to print table above
}

#endif

///////////////////////////// Squash //////////////////////////////

// return p = 1/(1 + exp(-d)), d scaled by 8 bits, p scaled by 12 bits
int squash(int d) {
  static const int t[33]={
    1,2,3,6,10,16,27,45,73,120,194,310,488,747,1101,
    1546,2047,2549,2994,3348,3607,3785,3901,3975,4022,
    4050,4068,4079,4085,4089,4092,4093,4094};
  if (d>2047) return 4095;
  if (d<-2047) return 0;
  int w=d&127;
  d=(d>>7)+16;
  return (t[d]*(128-w)+t[(d+1)]*w+64) >> 7;
}

//////////////////////////// Stretch ///////////////////////////////

// Inverse of squash. d = ln(p/(1-p)), d scaled by 8 bits, p by 12 bits.
// d has range -2047 to 2047 representing -8 to 8.  p has range 0 to 4095.

class Stretch {
  Array<short> t;
public:
  Stretch();
  int operator()(int p) const {
    assert(p>=0 && p<4096);
    return t[p];
  }
} stretch;

Stretch::Stretch(): t(4096) {
  int pi=0;
  for (int x=-2047; x<=2047; ++x) {  // invert squash()
    int i=squash(x);
    for (int j=pi; j<=i; ++j)
      t[j]=x;
    pi=i+1;
  }
  t[4095]=2047;
}

//////////////////////////// Mixer /////////////////////////////

// Mixer m(N, M, S=1, w=0) combines models using M neural networks with
//   N inputs each, of which up to S may be selected.  If S > 1 then
//   the outputs of these neural networks are combined using another
//   neural network (with parameters S, 1, 1).  If S = 1 then the
//   output is direct.  The weights are initially w (+-32K).
//   It is used as follows:
// m.update() trains the network where the expected output is the
//   last bit (in the global variable y).
// m.add(stretch(p)) inputs prediction from one of N models.  The
//   prediction should be positive to predict a 1 bit, negative for 0,
//   nominally +-256 to +-2K.  The maximum allowed value is +-32K but
//   using such large values may cause overflow if N is large.
// m.set(cxt, range) selects cxt as one of 'range' neural networks to
//   use.  0 <= cxt < range.  Should be called up to S times such
//   that the total of the ranges is <= M.
// m.p() returns the output prediction that the next bit is 1 as a
//   12 bit number (0 to 4095).

// dot_product returns dot product t*w of n elements.  n is rounded
// up to a multiple of 8.  Result is scaled down by 8 bits.
#ifdef NOASM  // no assembly language
int dot_product(short *t, short *w, int n) {
  int sum=0;
  n=(n+7)&-8;
  for (int i=0; i<n; i+=2)
    sum+=(t[i]*w[i]+t[i+1]*w[i+1]) >> 8;
  return sum;
}
#else  // The NASM version uses MMX and is about 8 times faster.
extern "C" int dot_product(short *t, short *w, int n);  // in NASM
#endif

// Train neural network weights w[n] given inputs t[n] and err.
// w[i] += t[i]*err, i=0..n-1.  t, w, err are signed 16 bits (+- 32K).
// err is scaled 16 bits (representing +- 1/2).  w[i] is clamped to +- 32K
// and rounded.  n is rounded up to a multiple of 8.
#ifdef NOASM
void train(short *t, short *w, int n, int err) {
  n=(n+7)&-8;
  for (int i=0; i<n; ++i) {
    int wt=w[i]+((t[i]*err*2>>16)+1>>1);
    if (wt<-32768) wt=-32768;
    if (wt>32767) wt=32767;
    w[i]=wt;
  }
}
#else
extern "C" void train(short *t, short *w, int n, int err);  // in NASM
#endif

class Mixer {
  const int N, M, S;   // max inputs, max contexts, max context sets
  Array<short, 16> tx; // N inputs from add()
  Array<short, 16> wx; // N*M weights
  Array<int> cxt;  // S contexts
  int ncxt;        // number of contexts (0 to S)
  int base;        // offset of next context
  int nx;          // Number of inputs in tx, 0 to N
  Array<int> pr;   // last result (scaled 12 bits)
  Mixer* mp;       // points to a Mixer to combine results
public:
  Mixer(int n, int m, int s=1, int w=0);

  // Adjust weights to minimize coding cost of last prediction
  void update() {
    for (int i=0; i<ncxt; ++i) {
      int err=((y<<12)-pr[i])*7;
      assert(err>=-32768 && err<32768);
      if (err) train(&tx[0], &wx[cxt[i]*N], nx, err);
    }
    nx=base=ncxt=0;
  }

  // Input x (call up to N times)
  void add(int x) {
    assert(nx<N);
    tx[nx++]=x;
  }

  // Set a context (call S times, sum of ranges <= M)
  void set(int cx, int range) {
    assert(range>=0);
    assert(ncxt<S);
    assert(cx>=0);
    assert(base+cx<M);
    cxt[ncxt++]=base+cx;
    base+=range;
  }

  // predict next bit
  int p() {
    while (nx&7) tx[nx++]=0;  // pad
    if (mp) {  // combine outputs
      mp->update();
      for (int i=0; i<ncxt; ++i) {
        pr[i]=squash(dot_product(&tx[0], &wx[cxt[i]*N], nx)>>5);
        mp->add(stretch(pr[i]));
      }
      mp->set(0, 1);
      return mp->p();
    }
    else {  // S=1 context
      return pr[0]=squash(dot_product(&tx[0], &wx[0], nx)>>8);
    }
  }
  ~Mixer();
};

Mixer::~Mixer() {
  delete mp;
}


Mixer::Mixer(int n, int m, int s, int w):
    N((n+7)&-8), M(m), S(s), tx(N), wx(N*M),
    cxt(S), ncxt(0), base(0), nx(0), pr(S), mp(0) {
  assert(n>0 && N>0 && (N&7)==0 && M>0);
  int i;
  for ( i=0; i<S; ++i)
    pr[i]=2048;
  for ( i=0; i<N*M; ++i)
    wx[i]=w;
  if (S>1) mp=new Mixer(S, 1, 1, 0x7fff);
}

//////////////////////////// APM1 //////////////////////////////

// APM1 maps a probability and a context into a new probability
// that bit y will next be 1.  After each guess it updates
// its state to improve future guesses.  Methods:
//
// APM1 a(N) creates with N contexts, uses 66*N bytes memory.
// a.p(pr, cx, rate=7) returned adjusted probability in context cx (0 to
//   N-1).  rate determines the learning rate (smaller = faster, default 7).
//   Probabilities are scaled 12 bits (0-4095).

class APM1 {
  int index;     // last p, context
  const int N;   // number of contexts
  Array<U16> t;  // [N][33]:  p, context -> p
public:
  APM1(int n);
  int p(int pr=2048, int cxt=0, int rate=7) {
    assert(pr>=0 && pr<4096 && cxt>=0 && cxt<N && rate>0 && rate<32);
    pr=stretch(pr);
    int g=(y<<16)+(y<<rate)-y-y;
    t[index] += g-t[index] >> rate;
    t[index+1] += g-t[index+1] >> rate;
    const int w=pr&127;  // interpolation weight (33 points)
    index=(pr+2048>>7)+cxt*33;
    return t[index]*(128-w)+t[index+1]*w >> 11;
  }
};

// maps p, cxt -> p initially
APM1::APM1(int n): index(0), N(n), t(n*33) {
  for (int i=0; i<N; ++i)
    for (int j=0; j<33; ++j)
      t[i*33+j] = i==0 ? squash((j-16)*128)*16 : t[j];
}

//////////////////////////// StateMap, APM //////////////////////////

// A StateMap maps a context to a probability.  Methods:
//
// Statemap sm(n) creates a StateMap with n contexts using 4*n bytes memory.
// sm.p(y, cx, limit) converts state cx (0..n-1) to a probability (0..4095).
//     that the next y=1, updating the previous prediction with y (0..1).
//     limit (1..1023, default 1023) is the maximum count for computing a
//     prediction.  Larger values are better for stationary sources.

static int dt[1024];  // i -> 16K/(i+3)

class StateMap {
protected:
  const int N;  // Number of contexts
  int cxt;      // Context of last prediction
  Array<U32> t;       // cxt -> prediction in high 22 bits, count in low 10 bits
  inline void update(int limit) {
    assert(cxt>=0 && cxt<N);
    U32 *p=&t[cxt], p0=p[0];
    int n=p0&1023, pr=p0>>10;  // count, prediction
    if (n<limit) ++p0;
    else p0=p0&0xfffffc00|limit;;
    p0+=(((y<<22)-pr)>>3)*dt[n]&0xfffffc00;
    p[0]=p0;
  }

public:
  StateMap(int n=256);

  // update bit y (0..1), predict next bit in context cx
  int p(int cx, int limit=1023) {
    assert(cx>=0 && cx<N);
    assert(limit>0 && limit<1024);
    update(limit);
    return t[cxt=cx]>>20;
  }
};

StateMap::StateMap(int n): N(n), cxt(0), t(n) {
  for (int i=0; i<N; ++i)
    t[i]=1<<31;
}

// An APM maps a probability and a context to a new probability.  Methods:
//
// APM a(n) creates with n contexts using 96*n bytes memory.
// a.pp(y, pr, cx, limit) updates and returns a new probability (0..4095)
//     like with StateMap.  pr (0..4095) is considered part of the context.
//     The output is computed by interpolating pr into 24 ranges nonlinearly
//     with smaller ranges near the ends.  The initial output is pr.
//     y=(0..1) is the last bit.  cx=(0..n-1) is the other context.
//     limit=(0..1023) defaults to 255.

class APM: public StateMap {
public:
  APM(int n);
  int p(int pr, int cx, int limit=255) {
   // assert(y>>1==0);
    assert(pr>=0 && pr<4096);
    assert(cx>=0 && cx<N/24);
    assert(limit>0 && limit<1024);
    update(limit);
    pr=(stretch(pr)+2048)*23;
    int wt=pr&0xfff;  // interpolation weight of next element
    cx=cx*24+(pr>>12);
    assert(cx>=0 && cx<N-1);
    cxt=cx+(wt>>11);
	pr=(t[cx]>>13)*(0x1000-wt)+(t[cx+1]>>13)*wt>>19;
    return pr;
  }
};

APM::APM(int n): StateMap(n*24) {
  for (int i=0; i<N; ++i) {
    int p=((i%24*2+1)*4096)/48-2048;
    t[i]=(U32(squash(p))<<20)+6;
  }
}


//////////////////////////// hash //////////////////////////////

// Hash 2-5 ints.
inline U32 hash(U32 a, U32 b, U32 c=0xffffffff, U32 d=0xffffffff,
    U32 e=0xffffffff) {
  U32 h=a*200002979u+b*30005491u+c*50004239u+d*70004807u+e*110002499u;
  return h^h>>9^a>>2^b>>3^c>>4^d>>5^e>>6;
}

///////////////////////////// BH ////////////////////////////////

// A BH maps a 32 bit hash to an array of B bytes (checksum and B-2 values)
//
// BH bh(N); creates N element table with B bytes each.
//   N must be a power of 2.  The first byte of each element is
//   reserved for a checksum to detect collisions.  The remaining
//   B-1 bytes are values, prioritized by the first value.  This
//   byte is 0 to mark an unused element.
//   
// bh[i] returns a pointer to the i'th element, such that
//   bh[i][0] is a checksum of i, bh[i][1] is the priority, and
//   bh[i][2..B-1] are other values (0-255).
//   The low lg(n) bits as an index into the table.
//   If a collision is detected, up to M nearby locations in the same
//   cache line are tested and the first matching checksum or
//   empty element is returned.
//   If no match or empty element is found, then the lowest priority
//   element is replaced.

// 2 byte checksum with LRU replacement (except last 2 by priority)
template <int B> class BH {
  enum {M=8};  // search limit
  Array<U8, 64> t; // elements
  U32 n; // size-1
public:
  BH(int i): t(i*B), n(i-1) {
    assert(B>=2 && i>0 && (i&(i-1))==0); // size a power of 2?
  }
  U8* operator[](U32 i);
};

template <int B>
inline  U8* BH<B>::operator[](U32 i) {
  int chk=(i>>16^i)&0xffff;
  i=i*M&n;
  U8 *p;
  U16 *cp;
  int j;
  for (j=0; j<M; ++j) {
    p=&t[(i+j)*B];
    cp=(U16*)p;
    if (p[2]==0) {*cp=chk;break;}
    if (*cp==chk) break;  // found
  }
  if (j==0) return p+1;  // front
  static U8 tmp[B];  // element to move to front
  if (j==M) {
    --j;
    memset(tmp, 0, B);
    *(U16*)tmp=chk;
    if (M>2 && t[(i+j)*B+2]>t[(i+j-1)*B+2]) --j;
  }
  else memcpy(tmp, cp, B);
  memmove(&t[(i+1)*B], &t[i*B], j*B);
  memcpy(&t[i*B], tmp, B);
  return &t[i*B+1];
}

/////////////////////////// ContextMap /////////////////////////
//
// A ContextMap maps contexts to a bit histories and makes predictions
// to a Mixer.  Methods common to all classes:
//
// ContextMap cm(M, C); creates using about M bytes of memory (a power
//   of 2) for C contexts.
// cm.set(cx);  sets the next context to cx, called up to C times
//   cx is an arbitrary 32 bit value that identifies the context.
//   It should be called before predicting the first bit of each byte.
// cm.mix(m) updates Mixer m with the next prediction.  Returns 1
//   if context cx is found, else 0.  Then it extends all the contexts with
//   global bit y.  It should be called for every bit:
//
//     if (bpos==0) 
//       for (int i=0; i<C; ++i) cm.set(cxt[i]);
//     cm.mix(m);
//
// The different types are as follows:
//
// - RunContextMap.  The bit history is a count of 0-255 consecutive
//     zeros or ones.  Uses 4 bytes per whole byte context.  C=1.
//     The context should be a hash.
// - SmallStationaryContextMap.  0 <= cx < M/512.
//     The state is a 16-bit probability that is adjusted after each
//     prediction.  C=1.
// - ContextMap.  For large contexts, C >= 1.  Context need not be hashed.

// Predict to mixer m from bit history state s, using sm to map s to
// a probability.
inline int mix2(Mixer& m, int s, StateMap& sm) {
  int p1=sm.p(s);
  int n0=-!nex(s,2);
  int n1=-!nex(s,3);
  int st=stretch(p1)>>2;
  m.add(st);
  p1>>=4;
  int p0=255-p1;
  m.add(p1-p0);
  m.add(st*(n1-n0));
  m.add((p1&n0)-(p0&n1));
  m.add((p1&n1)-(p0&n0));
  return s>0;
}

// A RunContextMap maps a context into the next byte and a repeat
// count up to M.  Size should be a power of 2.  Memory usage is 3M/4.
class RunContextMap {
  BH<4> t;
  U8* cp;
public:
  RunContextMap(int m): t(m/4) {cp=t[0]+1;}
  void set(U32 cx) {  // update count
    if (cp[0]==0 || cp[1]!=buf(1)) cp[0]=1, cp[1]=buf(1);
    else if (cp[0]<255) ++cp[0];
    cp=t[cx]+1;
  }
  int p() {  // predict next bit
    if (cp[1]+256>>8-bpos==c0)
      return ((cp[1]>>7-bpos&1)*2-1)*ilog(cp[0]+1)*8;
    else
      return 0;
  }
  int mix(Mixer& m) {  // return run length
    m.add(p());
    return cp[0]!=0;
  }
};

// Context is looked up directly.  m=size is power of 2 in bytes.
// Context should be < m/512.  High bits are discarded.
class SmallStationaryContextMap {
  Array<U16> t;
  int cxt;
  U16 *cp;
public:
  SmallStationaryContextMap(int m): t(m/2), cxt(0) {
    assert((m/2&m/2-1)==0); // power of 2?
    for (int i=0; i<t.size(); ++i)
      t[i]=32768;
    cp=&t[0];
  }
  void set(U32 cx) {
    cxt=cx*256&t.size()-256;
  }
  void mix(Mixer& m, int rate=7) {
    *cp += (y<<16)-*cp+(1<<rate-1) >> rate;
    cp=&t[cxt+c0];
    m.add(stretch(*cp>>4));
  }
};

// Context map for large contexts.  Most modeling uses this type of context
// map.  It includes a built in RunContextMap to predict the last byte seen
// in the same context, and also bit-level contexts that map to a bit
// history state.
//
// Bit histories are stored in a hash table.  The table is organized into
// 64-byte buckets alinged on cache page boundaries.  Each bucket contains
// a hash chain of 7 elements, plus a 2 element queue (packed into 1 byte) 
// of the last 2 elements accessed for LRU replacement.  Each element has
// a 2 byte checksum for detecting collisions, and an array of 7 bit history
// states indexed by the last 0 to 2 bits of context.  The buckets are indexed
// by a context ending after 0, 2, or 5 bits of the current byte.  Thus, each
// byte modeled results in 3 main memory accesses per context, with all other
// accesses to cache.
//
// On bits 0, 2 and 5, the context is updated and a new bucket is selected.
// The most recently accessed element is tried first, by comparing the
// 16 bit checksum, then the 7 elements are searched linearly.  If no match
// is found, then the element with the lowest priority among the 5 elements 
// not in the LRU queue is replaced.  After a replacement, the queue is
// emptied (so that consecutive misses favor a LFU replacement policy).
// In all cases, the found/replaced element is put in the front of the queue.
//
// The priority is the state number of the first element (the one with 0
// additional bits of context).  The states are sorted by increasing n0+n1
// (number of bits seen), implementing a LFU replacement policy.
//
// When the context ends on a byte boundary (bit 0), only 3 of the 7 bit
// history states are used.  The remaining 4 bytes implement a run model
// as follows: <count:7,d:1> <b1> <unused> <unused> where <b1> is the last byte
// seen, possibly repeated.  <count:7,d:1> is a 7 bit count and a 1 bit
// flag (represented by count * 2 + d).  If d=0 then <count> = 1..127 is the 
// number of repeats of <b1> and no other bytes have been seen.  If d is 1 then 
// other byte values have been seen in this context prior to the last <count> 
// copies of <b1>.
//
// As an optimization, the last two hash elements of each byte (representing
// contexts with 2-7 bits) are not updated until a context is seen for
// a second time.  This is indicated by <count,d> = <1,0> (2).  After update,
// <count,d> is updated to <2,0> or <1,1> (4 or 3).

class ContextMap {
  const int C;  // max number of contexts
  class E {  // hash element, 64 bytes
    U16 chk[7];  // byte context checksums
    U8 last;     // last 2 accesses (0-6) in low, high nibble
  public:
    U8 bh[7][7]; // byte context, 3-bit context -> bit history state
      // bh[][0] = 1st bit, bh[][1,2] = 2nd bit, bh[][3..6] = 3rd bit
      // bh[][0] is also a replacement priority, 0 = empty
    U8* get(U16 chk);  // Find element (0-6) matching checksum.
      // If not found, insert or replace lowest priority (not last).
  };
  Array<E, 64> t;  // bit histories for bits 0-1, 2-4, 5-7
    // For 0-1, also contains a run count in bh[][4] and value in bh[][5]
    // and pending update count in bh[7]
  Array<U8*> cp;   // C pointers to current bit history
  Array<U8*> cp0;  // First element of 7 element array containing cp[i]
  Array<U32> cxt;  // C whole byte contexts (hashes)
  Array<U8*> runp; // C [0..3] = count, value, unused, unused
  StateMap *sm;    // C maps of state -> p
  int cn;          // Next context to set by set()
  void update(U32 cx, int c);  // train model that context cx predicts c
  int mix1(Mixer& m, int cc, int bp, int c1, int y1);
    // mix() with global context passed as arguments to improve speed.
public:
  ContextMap(int m, int c=1);  // m = memory in bytes, a power of 2, C = c
  ~ContextMap();
  void set(U32 cx, int next=-1);   // set next whole byte context to cx
    // if next is 0 then set order does not matter
  int mix(Mixer& m) {return mix1(m, c0, bpos, buf(1), y);}
};

// Find or create hash element matching checksum ch
inline U8* ContextMap::E::get(U16 ch) {
  if (chk[last&15]==ch) return &bh[last&15][0];
  int b=0xffff, bi=0;
  for (int i=0; i<7; ++i) {
    if (chk[i]==ch) return last=last<<4|i, (U8*)&bh[i][0];
    int pri=bh[i][0];
    if (pri<b && (last&15)!=i && last>>4!=i) b=pri, bi=i;
  }
  return last=0xf0|bi, chk[bi]=ch, (U8*)memset(&bh[bi][0], 0, 7);
}

// Construct using m bytes of memory for c contexts
ContextMap::ContextMap(int m, int c): C(c), t(m>>6), cp(c), cp0(c),
    cxt(c), runp(c), cn(0) {
  assert(m>=64 && (m&m-1)==0);  // power of 2?
  assert(sizeof(E)==64);
  sm=new StateMap[C];
  for (int i=0; i<C; ++i) {
    cp0[i]=cp[i]=&t[0].bh[0][0];
    runp[i]=cp[i]+3;
  }
}

ContextMap::~ContextMap() {
  delete[] sm;
}

// Set the i'th context to cx
inline void ContextMap::set(U32 cx, int next) {
  int i=cn++;
  i&=next;
  assert(i>=0 && i<C);
  cx=cx*987654323+i;  // permute (don't hash) cx to spread the distribution
  cx=cx<<16|cx>>16;
  cxt[i]=cx*123456791+i;
}

// Update the model with bit y1, and predict next bit to mixer m.
// Context: cc=c0, bp=bpos, c1=buf(1), y1=y.
int ContextMap::mix1(Mixer& m, int cc, int bp, int c1, int y1) {

  // Update model with y
  int result=0;
  for (int i=0; i<cn; ++i) {
    if (cp[i]) {
      assert(cp[i]>=&t[0].bh[0][0] && cp[i]<=&t[t.size()-1].bh[6][6]);
      assert((long(cp[i])&63)>=15);
      int ns=nex(*cp[i], y1);
      if (ns>=204 && rnd() << (452-ns>>3)) ns-=4;  // probabilistic increment
      *cp[i]=ns;
    }

    // Update context pointers
    if (bpos>1 && runp[i][0]==0)
    {
     cp[i]=0;
    }
    else
    {
     switch(bpos)
     {
      case 1: case 3: case 6: cp[i]=cp0[i]+1+(cc&1); break;
      case 4: case 7: cp[i]=cp0[i]+3+(cc&3); break;
      case 2: case 5: cp0[i]=cp[i]=t[cxt[i]+cc&t.size()-1].get(cxt[i]>>16); break;
      default:
      {
       cp0[i]=cp[i]=t[cxt[i]+cc&t.size()-1].get(cxt[i]>>16);
       // Update pending bit histories for bits 2-7
       if (cp0[i][3]==2) {
         const int c=cp0[i][4]+256;
         U8 *p=t[cxt[i]+(c>>6)&t.size()-1].get(cxt[i]>>16);
         p[0]=1+((c>>5)&1);
         p[1+((c>>5)&1)]=1+((c>>4)&1);
         p[3+((c>>4)&3)]=1+((c>>3)&1);
         p=t[cxt[i]+(c>>3)&t.size()-1].get(cxt[i]>>16);
         p[0]=1+((c>>2)&1);
         p[1+((c>>2)&1)]=1+((c>>1)&1);
         p[3+((c>>1)&3)]=1+(c&1);
         cp0[i][6]=0;
       }
       // Update run count of previous context
       if (runp[i][0]==0)  // new context
         runp[i][0]=2, runp[i][1]=c1;
       else if (runp[i][1]!=c1)  // different byte in context
         runp[i][0]=1, runp[i][1]=c1;
       else if (runp[i][0]<254)  // same byte in context
         runp[i][0]+=2;
       else if (runp[i][0]==255)
         runp[i][0]=128;
       runp[i]=cp0[i]+3;
      } break;
     }
    }

    // predict from last byte in context
    if (runp[i][1]+256>>8-bp==cc) {
      int rc=runp[i][0];  // count*2, +1 if 2 different bytes seen
      int b=(runp[i][1]>>7-bp&1)*2-1;  // predicted bit + for 1, - for 0
      int c=ilog(rc+1)<<2+(~rc&1);
      m.add(b*c);
    }
    else
      m.add(0);

    // predict from bit context
   if(cp[i])
   {
    result+=mix2(m, *cp[i], sm[i]);
   }
   else
   {
    mix2(m, 0, sm[i]);
   }


  }
  if (bp==7) cn=0;
  return result;
}

//////////////////////////// Models //////////////////////////////

// All of the models below take a Mixer as a parameter and write
// predictions to it.

//////////////////////////// matchModel ///////////////////////////

// matchModel() finds the longest matching context and returns its length

int matchModel(Mixer& m) {
  const int MAXLEN=65534;  // longest allowed match + 1
  static Array<int> t(MEM);  // hash table of pointers to contexts
  static int h=0;  // hash of last 7 bytes
  static int ptr=0;  // points to next byte of match if any
  static int len=0;  // length of match, or 0 if no match
  static int result=0;
  
  static SmallStationaryContextMap scm1(0x20000);

  if (!bpos) {
    h=h*997*8+buf(1)+1&t.size()-1;  // update context hash
    if (len) ++len, ++ptr;
    else {  // find match
      ptr=t[h];
      if (ptr && pos-ptr<buf.size())
        while (buf(len+1)==buf[ptr-len-1] && len<MAXLEN) ++len;
    }
    t[h]=pos;  // update hash table
    result=len;
//    if (result>0 && !(result&0xfff)) printf("pos=%d len=%d ptr=%d\n", pos, len, ptr);
    scm1.set(pos);
  }

  // predict
  if (len)
  {
   if (buf(1)==buf[ptr-1] && c0==buf[ptr]+256>>8-bpos)
   {
    if (len>MAXLEN) len=MAXLEN;
    if (buf[ptr]>>7-bpos&1)
    {
     m.add(ilog(len)<<2);
     m.add(min(len, 32)<<6);
    }
    else 
    {
     m.add(-(ilog(len)<<2));
     m.add(-(min(len, 32)<<6));
    }
   }
   else
   {
    len=0;
    m.add(0);
    m.add(0);
   }
  }
  else
  {
   m.add(0);
   m.add(0);
  }

  scm1.mix(m);
  return result;
}

//////////////////////////// picModel //////////////////////////

// Model a 1728 by 2376 2-color CCITT bitmap image, left to right scan,
// MSB first (216 bytes per row, 513216 bytes total).  Insert predictions
// into m.

void picModel(Mixer& m) {
  static U32 r0, r1, r2, r3;  // last 4 rows, bit 8 is over current pixel
  static Array<U8> t(0x10200);  // model: cxt -> state
  const int N=3;  // number of contexts
  static int cxt[N];  // contexts
  static StateMap sm[N];

  // update the model
  int i;
  for ( i=0; i<N; ++i)
    t[cxt[i]]=nex(t[cxt[i]],y);

  // update the contexts (pixels surrounding the predicted one)
  r0+=r0+y;
  r1+=r1+((buf(215)>>(7-bpos))&1);
  r2+=r2+((buf(431)>>(7-bpos))&1);
  r3+=r3+((buf(647)>>(7-bpos))&1);
  cxt[0]=r0&0x7|r1>>4&0x38|r2>>3&0xc0;
  cxt[1]=0x100+(r0&1|r1>>4&0x3e|r2>>2&0x40|r3>>1&0x80);
  cxt[2]=0x200+(r0&0x3f^r1&0x3ffe^r2<<2&0x7f00^r3<<5&0xf800);

  // predict
  for ( i=0; i<N; ++i)
    m.add(stretch(sm[i].p(t[cxt[i]])));
}

//////////////////////////// wordModel /////////////////////////

// Model English text (words and columns/end of line)

void wordModel(Mixer& m) {
  static U32 word0=0, word1=0, word2=0, word3=0, word4=0, word5=0;  // hashes
  static U32 text0=0;  // hash stream of letters
  static ContextMap cm(MEM*16, 20);
  static int nl1=-3, nl=-2;  // previous, current newline position

  // Update word hashes
  if (bpos==0) {
    int c=c4&255;
    if (c>='A' && c<='Z')
      c+='a'-'A';
    if (c>='a' && c<='z' || c>=128) {
      word0=word0*263*32+c;
      text0=text0*997*16+c;
    }
    else if (word0) {
      word5=word4*23;
      word4=word3*19;
      word3=word2*17;
      word2=word1*13;
      word1=word0*11;
      word0=0;
    }
    if (c==10) nl1=nl, nl=pos-1;
    int col=min(255, pos-nl), above=buf[nl1+col]; // text column context
    U32 h=word0*271+buf(1);
    
    cm.set(h);
    cm.set(word0);
    cm.set(h+word1);
    cm.set(word0+word1*31);
    cm.set(h+word1+word2*29);
    cm.set(text0&0xffffff);
    cm.set(text0&0xfffff);

    cm.set(h+word2);
    cm.set(h+word3);
    cm.set(h+word4);
    cm.set(h+word5);
    cm.set(buf(1)|buf(3)<<8|buf(5)<<16);
    cm.set(buf(2)|buf(4)<<8|buf(6)<<16);

    cm.set(h+word1+word3);
    cm.set(h+word2+word3);

    // Text column models
    cm.set(col<<16|buf(1)<<8|above);
    cm.set(buf(1)<<8|above);
    cm.set(col<<8|buf(1));
    cm.set(col);
  }
  cm.mix(m);
}

//////////////////////////// recordModel ///////////////////////

// Model 2-D data with fixed record length.  Also order 1-2 models
// that include the distance to the last match.

void recordModel(Mixer& m) {
  static int cpos1[256] , cpos2[256], cpos3[256], cpos4[256];
  static int wpos1[0x10000]; // buf(1..2) -> last position
  static int rlen=2, rlen1=3, rlen2=4;  // run length and 2 candidates
  static int rcount1=0, rcount2=0;  // candidate counts
  static ContextMap cm(32768, 3), cn(32768/2, 3), co(32768*2, 3), cp(MEM, 3);

  // Find record length
  if (!bpos) {
    int w=c4&0xffff, c=w&255, d=w>>8;
#if 1
    int r=pos-cpos1[c];
    if (r>1 && r==cpos1[c]-cpos2[c]
        && r==cpos2[c]-cpos3[c] && r==cpos3[c]-cpos4[c]
        && (r>15 || (c==buf(r*5+1)) && c==buf(r*6+1))) {
      if (r==rlen1) ++rcount1;
      else if (r==rlen2) ++rcount2;
      else if (rcount1>rcount2) rlen2=r, rcount2=1;
      else rlen1=r, rcount1=1;
    }
    if (rcount1>15 && rlen!=rlen1) rlen=rlen1, rcount1=rcount2=0;
    if (rcount2>15 && rlen!=rlen2) rlen=rlen2, rcount1=rcount2=0;

    // Set 2 dimensional contexts
    assert(rlen>0);
#endif
    cm.set(c<<8| (min(255, pos-cpos1[c])/4) );
    cm.set(w<<9| llog(pos-wpos1[w])>>2);
    
    cm.set(rlen|buf(rlen)<<10|buf(rlen*2)<<18);
    cn.set(w|rlen<<8);
    cn.set(d|rlen<<16);
    cn.set(c|rlen<<8);

    co.set(buf(1)<<8|min(255, pos-cpos1[buf(1)]));
    co.set(buf(1)<<17|buf(2)<<9|llog(pos-wpos1[w])>>2);
    int col=pos%rlen;
    co.set(buf(1)<<8|buf(rlen));

    //cp.set(w*16);
    //cp.set(d*32);
    //cp.set(c*64);
    cp.set(rlen|buf(rlen)<<10|col<<18);
    cp.set(rlen|buf(1)<<10|col<<18);
    cp.set(col|rlen<<12);

    // update last context positions
    cpos4[c]=cpos3[c];
    cpos3[c]=cpos2[c];
    cpos2[c]=cpos1[c];
    cpos1[c]=pos;
    wpos1[w]=pos;
  }
  cm.mix(m);
  cn.mix(m);
  co.mix(m);
  cp.mix(m);
}


//////////////////////////// sparseModel ///////////////////////

// Model order 1-2 contexts with gaps.

void sparseModel(Mixer& m, int seenbefore, int howmany) {
  static ContextMap cm(MEM*2, 48);
  static int mask = 0;

  if (bpos==0) {

    cm.set( c4&0x00f0f0f0);
    cm.set((c4&0xf0f0f0f0)+1);
    cm.set((c4&0x00f8f8f8)+2);
    cm.set((c4&0xf8f8f8f8)+3);
    cm.set((c4&0x00e0e0e0)+4);
    cm.set((c4&0xe0e0e0e0)+5);
    cm.set((c4&0x00f0f0ff)+6);

    cm.set(seenbefore);
    cm.set(howmany);
    cm.set(c4&0x00ff00ff);
    cm.set(c4&0xff0000ff);
    cm.set(buf(1)|buf(5)<<8);
    cm.set(buf(1)|buf(6)<<8);
    cm.set(buf(3)|buf(6)<<8);
    cm.set(buf(4)|buf(8)<<8);
    
    for (int i=1; i<8; ++i) {
      cm.set((buf(i+1)<<8)|buf(i+2));
      cm.set((buf(i+1)<<8)|buf(i+3));
      cm.set(seenbefore|buf(i)<<8);
    }

    int fl = 0;
    if( c4&0xff != 0 ){
           if( isalpha( c4&0xff ) ) fl = 1;
      else if( ispunct( c4&0xff ) ) fl = 2;
      else if( isspace( c4&0xff ) ) fl = 3;
      else if( c4&0xff == 0xff ) fl = 4;
      else if( c4&0xff < 16 ) fl = 5;
      else if( c4&0xff < 64 ) fl = 6;
      else fl = 7;
    }
    mask = (mask<<3)|fl;
    cm.set(mask);
    cm.set(mask<<8|buf(1));
    cm.set(mask<<17|buf(2)<<8|buf(3));
    cm.set(mask&0x1ff|((c4&0xf0f0f0f0)<<9));
  }
  cm.mix(m);
}

//////////////////////////// distanceModel ///////////////////////

// Model for modelling distances between symbols

void distanceModel(Mixer& m) {
  static ContextMap cr(MEM, 3);
  if( bpos == 0 ){
    static int pos00=0,pos20=0,posnl=0;
    int c=c4&0xff;
    if(c==0x00)pos00=pos;
    if(c==0x20)pos20=pos;
    if(c==0xff||c=='\r'||c=='\n')posnl=pos;
    cr.set(min(pos-pos00,255)|(c<<8));
    cr.set(min(pos-pos20,255)|(c<<8));
    cr.set(min(pos-posnl,255)|(c<<8)+234567);
  }
  cr.mix(m);
}

//////////////////////////// bmpModel /////////////////////////////////

// Model a 24-bit color uncompressed .bmp or .tif file.  Return
// width in pixels if an image file is detected, else 0.

// 32-bit little endian number at buf(i)..buf(i-3)
inline U32 i4(int i) {
  assert(i>3);
  return buf(i)+256*buf(i-1)+65536*buf(i-2)+16777216*buf(i-3);
}

// 16-bit
inline int i2(int i) {
  assert(i>1);
  return buf(i)+256*buf(i-1);
}

// Square buf(i)
inline int sqrbuf(int i) {
  assert(i>0);
  return buf(i)*buf(i);
}

int bmpModel(Mixer& m) {
  static int w=0;  // width of image in bytes (pixels * 3)
  static int eoi=0;     // end of image
  static U32 tiff=0;  // offset of tif header
  const int SC=0x20000;
  static SmallStationaryContextMap scm1(SC), scm2(SC),
    scm3(SC), scm4(SC), scm5(SC), scm6(SC), scm7(SC), scm8(SC), scm9(SC*2), scm10(SC);
  static ContextMap cm(MEM*4, 13);

  // Detect .bmp file header (24 bit color, not compressed)
  if (!bpos && buf(54)=='B' && buf(53)=='M'
      && i4(44)==54 && i4(40)==40 && i4(24)==0) {
    w=(i4(36)+3&-4)*3;  // image width
    const int height=i4(32);
    eoi=pos;
    if (w<0x30000 && height<0x10000) {
      eoi=pos+w*height;  // image size in bytes
      printf("BMP %dx%d ", w/3, height);
    }
    else
      eoi=pos;
  }

  // Detect .tif file header (24 bit color, not compressed).
  // Parsing is crude, won't work with weird formats.
  if (!bpos) {
    if (c4==0x49492a00) tiff=pos;  // Intel format only
    if (pos-tiff==4 && c4!=0x08000000) tiff=0; // 8=normal offset to directory
    if (tiff && pos-tiff==200) {  // most of directory should be read by now
      int dirsize=i2(pos-tiff-4);  // number of 12-byte directory entries
      w=0;
      int bpp=0, compression=0, width=0, height=0;
      for (int i=tiff+6; i<pos-12 && --dirsize>0; i+=12) {
        int tag=i2(pos-i);  // 256=width, 257==height, 259: 1=no compression
          // 277=3 samples/pixel
        int tagfmt=i2(pos-i-2);  // 3=short, 4=long
        int taglen=i4(pos-i-4);  // number of elements in tagval
        int tagval=i4(pos-i-8);  // 1 long, 1-2 short, or points to array
        if ((tagfmt==3||tagfmt==4) && taglen==1) {
          if (tag==256) width=tagval;
          if (tag==257) height=tagval;
          if (tag==259) compression=tagval; // 1 = no compression
          if (tag==277) bpp=tagval;  // should be 3
        }
      }
      if (width>0 && height>0 && width*height>50 && compression==1
          && (bpp==1||bpp==3))
        eoi=tiff+width*height*bpp, w=width*bpp;
      if (eoi>pos)
        printf("TIFF %dx%dx%d ", width, height, bpp);
      else
        tiff=w=0;
    }
  }
  if (pos>eoi) return w=0;

  // Select nearby pixels as context
  if (!bpos) {
    assert(w>3);
    int color=pos%3;
    int mean=buf(3)+buf(w-3)+buf(w)+buf(w+3);
    const int var=sqrbuf(3)+sqrbuf(w-3)+sqrbuf(w)+sqrbuf(w+3)-mean*mean/4>>2;
    mean>>=2;
    const int logvar=ilog(var);
    int i=0;
    cm.set(hash(++i, buf(3), color));
    cm.set(hash(++i, buf(3), buf(1), color));
    cm.set(hash(++i, buf(3), buf(1)>>2, buf(2)>>6, color));
    cm.set(hash(++i, buf(w), color));
    cm.set(hash(++i, buf(w), buf(1), color));
    cm.set(hash(++i, buf(w), buf(1)>>2, buf(2)>>6, color));
    cm.set(hash(++i, buf(3)+buf(w)>>3, buf(1)>>5, buf(2)>>5, color));
    cm.set(hash(++i, buf(1), buf(2), color));
    cm.set(hash(++i, buf(3), buf(1)-buf(4), color));
    cm.set(hash(++i, buf(3)+buf(1)-buf(4), color));
    cm.set(hash(++i, buf(w), buf(1)-buf(w+1), color));
    cm.set(hash(++i, buf(w)+buf(1)-buf(w+1), color));
    cm.set(hash(++i, mean, logvar>>5, color));
    scm1.set(buf(3)+buf(w)-buf(w+3));
    scm2.set(buf(3)+buf(w-3)-buf(w));
    scm3.set(buf(3)*2-buf(6));
    scm4.set(buf(w)*2-buf(w*2));
    scm5.set(buf(w+3)*2-buf(w*2+6));
    scm6.set(buf(w-3)*2-buf(w*2-6));
    scm7.set(buf(w-3)+buf(1)-buf(w-2));
    scm8.set(buf(w)+buf(w-3)-buf(w*2-3));
    scm9.set(mean>>1|logvar<<1&0x180);
  }

  // Predict next bit
  scm1.mix(m);
  scm2.mix(m);
  scm3.mix(m);
  scm4.mix(m);
  scm5.mix(m);
  scm6.mix(m);
  scm7.mix(m);
  scm8.mix(m);
  scm9.mix(m);
  scm10.mix(m);
  cm.mix(m);
  return w;
}

void model8bit(Mixer& m, int w) {
	const int SC=0x20000;
	static SmallStationaryContextMap scm1(SC), scm2(SC),
		scm3(SC), scm4(SC), scm5(SC), scm6(SC*2),scm7(SC);
	static ContextMap cm(MEM*4, 32);
	
	// Select nearby pixels as context
	if (!bpos) {
		assert(w>3);
		int mean=buf(1)+buf(w-1)+buf(w)+buf(w+1);
		const int var=sqrbuf(1)+sqrbuf(w-1)+sqrbuf(w)+sqrbuf(w+1)-mean*mean/4>>2;
		mean>>=2;
		const int logvar=ilog(var);
		int i=0;
		// 2 x 
		cm.set(hash(++i, buf(1)>>2, buf(w)>>2));
		cm.set(hash(++i, buf(1)>>2, buf(2)>>2));
		cm.set(hash(++i, buf(w)>>2, buf(w*2)>>2));
		cm.set(hash(++i, buf(1)>>2, buf(w-1)>>2));
		cm.set(hash(++i, buf(w)>>2, buf(w+1)>>2));
		cm.set(hash(++i, buf(w+1)>>2, buf(w+2)>>2));
		cm.set(hash(++i, buf(w+1)>>2, buf(w*2+2)>>2));
		cm.set(hash(++i, buf(w-1)>>2, buf(w*2-2)>>2));
		cm.set(hash(++i, buf(1)+buf(w)>>1));
		cm.set(hash(++i, buf(1)+buf(2)>>1));
		cm.set(hash(++i, buf(w)+buf(w*2)>>1));
		cm.set(hash(++i, buf(1)+buf(w-1)>>1));
		cm.set(hash(++i, buf(w)+buf(w+1)>>1));
		cm.set(hash(++i, buf(w+1)+buf(w+2)>>1));
		cm.set(hash(++i, buf(w+1)+buf(w*2+2)>>1));
		cm.set(hash(++i, buf(w-1)+buf(w*2-2)>>1));

		// 3 x
		cm.set(hash(++i, buf(w)>>2, buf(1)>>2, buf(w-1)>>2));
		cm.set(hash(++i, buf(w-1)>>2, buf(w)>>2, buf(w+1)>>2));
		cm.set(hash(++i, buf(1)>>2, buf(w-1)>>2, buf(w*2-1)>>2));

		// mixed
		cm.set(hash(++i, buf(3)+buf(w)>>1, buf(1)>>2, buf(2)>>2));
		cm.set(hash(++i, buf(2)+buf(1)>>1,buf(w)+buf(w*2)>>1,buf(w-1)>>2));
		cm.set(hash(++i, buf(2)+buf(1)>>2,buf(w-1)+buf(w)>>2));
		cm.set(hash(++i, buf(2)+buf(1)>>1,buf(w)+buf(w*2)>>1));
		cm.set(hash(++i, buf(2)+buf(1)>>1,buf(w-1)+buf(w*2-2)>>1));
		cm.set(hash(++i, buf(2)+buf(1)>>1,buf(w+1)+buf(w*2+2)>>1));
		cm.set(hash(++i, buf(w)+buf(w*2)>>1,buf(w-1)+buf(w*2+2)>>1));
		cm.set(hash(++i, buf(w-1)+buf(w)>>1,buf(w)+buf(w+1)>>1));
		cm.set(hash(++i, buf(1)+buf(w-1)>>1,buf(w)+buf(w*2)>>1));
		cm.set(hash(++i, buf(1)+buf(w-1)>>2,buf(w)+buf(w+1)>>2));

		cm.set(hash(++i, (buf(1)-buf(w-1)>>1)+buf(w)>>2));
		cm.set(hash(++i, (buf(w-1)-buf(w)>>1)+buf(1)>>2));
		cm.set(hash(++i, -buf(1)+buf(w-1)+buf(w)>>2));

		scm1.set(buf(1)+buf(w)>>1);
		scm2.set(buf(1)+buf(w)-buf(w+1)>>1);
		scm3.set(buf(1)*2-buf(2)>>1);
		scm4.set(buf(w)*2-buf(w*2)>>1);
		scm5.set(buf(1)+buf(w)-buf(w-1)>>1);
		scm6.set(mean>>1|logvar<<1&0x180);
	}

	// Predict next bit
	scm1.mix(m);
	scm2.mix(m);
	scm3.mix(m);
	scm4.mix(m);
	scm5.mix(m);
	scm6.mix(m);
	scm7.mix(m); // Amazingly but improves compression!
	cm.mix(m);
	//return w;
}

//////////////////////////// pgmModel /////////////////////////////////

// Model a 8-bit grayscale uncompressed binary .pgm and 8-bit color
// uncompressed .bmp images.  Return width in pixels if an image file
// is detected, else 0.

#define ISWHITESPACE(i) (buf(i) == ' ' || buf(i) == '\t' || buf(i) == '\n' || buf(i) == '\r')
#define ISCRLF(i) (buf(i) == '\n' || buf(i) == '\r')

int pgmModel(Mixer& m) {
	static int h = 0;		// height of image in bytes (pixels)
	static int w = 0;		// width of image in bytes (pixels)
	static int eoi = 0;     // end of image
	static int pgm  = 0;    // offset of pgm header
	static int pgm_hdr[3];  // 0 - Width, 1 - Height, 2 - Max value
	static int pgm_ptr;		// which record in header should be parsed next
	int isws;				// is white space
	char v_buf[32];			
	int  v_ptr;
	if (!bpos)
	{
		if(buf(3)=='P' && buf(2)=='5' && ISWHITESPACE(1)) // Detect PGM file
		{
			pgm = pos;
			pgm_ptr = 0;
			return w = 0; // PGM header just detected, not enough info to get header yet
		}else 
			if(pgm && pgm_ptr!=3) 		// PGM detected, let's parse header records
			{ 
				for (int i = pgm; i<pos-1 && pgm_ptr<3; i++)
				{
					// Skip white spaces
					while ((isws = ISWHITESPACE(pos-i)) && i<pos-1) i++; 
					if(isws) break; // buffer end is reached

					// Skip comments
					if(buf(pos-i)=='#')
					{ 
						do {
							i++;
						}while(!ISCRLF(pos-i) && i<pos-1);
					}else
					{ 
						// Get header record as a string into v_buf
						v_ptr = 0;
						do {
							v_buf[v_ptr++] = buf(pos-i);
							i++;
						}while(!(isws = ISWHITESPACE(pos-i)) && i<pos-1 && v_ptr<32);

						if(isws)
						{
							pgm_hdr[pgm_ptr++] = atoi(v_buf);
							pgm = i; // move pointer 
						}
					}
				}

				// Header is finished, next byte is first pixel
				if(pgm_ptr==3)
				{ 
					if(pgm_hdr[2] == 255 && pgm_hdr[0]>0 && pgm_hdr[1]>0)
					{
						w = pgm_hdr[0];
						h = pgm_hdr[1];
						eoi = pos+w*h;
						printf("PGM %dx%d",w,h);
					}
				}
			}
	}
	if (pos>eoi) return w=0;
    model8bit(m,w);
	static int col=0;
	  if (++col>=8) col=0; // reset after every 24 columns?
	  m.set(2, 8);
	  m.set(col, 8);
	  m.set(buf(w)+buf(1)>>4, 32);
	  m.set(c0, 256);
	return w;
}

int bmpModel8(Mixer& m) {
	static int h = 0;		// height of image in bytes (pixels)
	static int w = 0;		// width of image in bytes (pixels)
	static int eoi = 0;     // end of image
	static int col = 0;
	static int ibmp=0,w1=0;
	 if (bpos==0) {
        //  8-bit .bmp images                    data offset      windows bmp    compression   bpp
		if (/*buf(54)=='B' && buf(53)=='M' && */(i4(44)< 1079) && i4(40)==40 && i4(24)==0 && (buf(26)==8 /*| buf(26)==4)*/)){
		/*	if  (buf(26)==4)
			w1=i4(36)/2;  // image width
			else*/
            w1=i4(36);  // image width 8453632 -> 2079974
			h=i4(32);   // image height
			ibmp=pos+i4(44)-54;
        }
        if (ibmp==pos) {
			w=w1;
			eoi=pos+w*h;
			printf("BMP(8-bit) %dx%d",w,h);
			ibmp=0;
		}
	 }
	if (pos>eoi) return w=0;
	  model8bit(m,w);
	  if (++col>=8) col=0; // reset after every 24 columns?
	  m.set(2, 8);
	  m.set(col, 8);
	  m.set(buf(w)+buf(1)>>4, 32);
	  m.set(c0, 256);
	  return w;
}

int rgbModel8(Mixer& m) {
	int h = 0;		        // height of image in bytes (pixels)
	static int w = 0;		// width of image in bytes (pixels)
	static int eoi = 0;     // end of image
	static int col = 0;
	 // for .rgb gray images
	 if (bpos==0) {
		if (buf(507)==1 && buf(506)==218 && buf(505)==0 && i2(496)==1)
        {
			w=(buf(501)&255)*256|(buf(500)&255); // image width
			h=(buf(499)&255)*256|(buf(498)&255);  // image height
			eoi=pos+w*h;
			printf("RGB(8-bit) %dx%d",w,h);
		}
	 }
	if (pos>eoi) return w=0;
	  model8bit(m,w);
	  if (++col>=8) col=0; // reset after every 24 columns?
	  m.set(2, 8);
	  m.set(col, 8);
	  m.set(buf(w)+buf(1)>>4, 32);
	  m.set(c0, 256);
	  return w;
}

//////////////////////////// jpegModel /////////////////////////

// Model JPEG. Return 1-257 if a JPEG file is detected or else 0.
// Only the baseline and 8 bit extended Huffman coded DCT modes are
// supported.  The model partially decodes the JPEG image to provide
// context for the Huffman coded symbols.

// Print a JPEG segment at buf[p...] for debugging
void dump(const char* msg, int p) {
  printf("%s:", msg);
  int len=buf[p+2]*256+buf[p+3];
  for (int i=0; i<len+2; ++i)
    printf(" %02X", buf[p+i]);
  printf("\n");
}

// Detect invalid JPEG data.  The proper response is to silently
// fall back to a non-JPEG model.
#define jassert(x) if (!(x)) { \
/*  printf("JPEG error at %d, line %d: %s\n", pos, __LINE__, #x); */ \
  jpeg=0; \
  return next_jpeg;}

struct HUF {U32 min, max; int val;}; // Huffman decode tables
  // huf[Tc][Th][m] is the minimum, maximum+1, and pointer to codes for
  // coefficient type Tc (0=DC, 1=AC), table Th (0-3), length m+1 (m=0-15)

void update_k(int v1, int v2, int &k1, int &k2) {
  int a, b, c;
  a=abs(v1*(k1-1)+v2*(8-(k1-1)))/8;
  b=abs(v1*(k1+0)+v2*(8-(k1+0)))/8;
  c=abs(v1*(k1+1)+v2*(8-(k1+1)))/8;
  if (k1==0) a=b; else if (k1==8) c=b;
  if (a<b && a<c) k2--;
  if (c<a && c<b) k2++;
  if (k2<-2) {k1--;k2=0;}
  if (k2>+2) {k1++;k2=0;}
}

int jpegModel(Mixer& m) {

  // State of parser
  enum {SOF0=0xc0, SOF1, SOF2, SOF3, DHT, RST0=0xd0, SOI=0xd8, EOI, SOS, DQT,
    DNL, DRI, APP0=0xe0, COM=0xfe, FF};  // Second byte of 2 byte codes
  static int jpeg=0;  // 1 if JPEG is header detected, 2 if image data
  static int next_jpeg=0;  // updated with jpeg on next byte boundary
  static int app;  // Bytes remaining to skip in APPx or COM field
  static int sof=0, sos=0, data=0;  // pointers to buf
  static Array<int> ht(8);  // pointers to Huffman table headers
  static int htsize=0;  // number of pointers in ht

  // Huffman decode state
  static U32 huffcode=0;  // Current Huffman code including extra bits
  static int huffbits=0;  // Number of valid bits in huffcode
  static int huffsize=0;  // Number of bits without extra bits
  static int rs=-1;  // Decoded huffcode without extra bits.  It represents
    // 2 packed 4-bit numbers, r=run of zeros, s=number of extra bits for
    // first nonzero code.  huffcode is complete when rs >= 0.
    // rs is -1 prior to decoding incomplete huffcode.
  static int mcupos=0;  // position in MCU (0-639).  The low 6 bits mark
    // the coefficient in zigzag scan order (0=DC, 1-63=AC).  The high
    // bits mark the block within the MCU, used to select Huffman tables.

  // Decoding tables
  static Array<HUF> huf(128);  // Tc*64+Th*16+m -> min, max, val
  static int mcusize=0;  // number of coefficients in an MCU
  static int linesize=0; // width of image in MCU
  static int hufsel[2][10];  // DC/AC, mcupos/64 -> huf decode table
  static Array<U8> hbuf(2048);  // Tc*1024+Th*256+hufcode -> RS

  // Image state
  static Array<int> color(10);  // block -> component (0-3)
  static Array<int> pred(4);  // component -> last DC value
  static int dc=0;  // DC value of the current block
  static int width=0;  // Image width in MCU
  static int row=0, column=0;  // in MCU (column 0 to width-1)
  static Buf cbuf(0x20000); // Rotating buffer of coefficients, coded as:
    // DC: level shifted absolute value, low 4 bits discarded, i.e.
    //   [-1023...1024] -> [0...255].
    // AC: as an RS code: a run of R (0-15) zeros followed by an S (0-15)
    //   bit number, or 00 for end of block (in zigzag order).
    //   However if R=0, then the format is ssss11xx where ssss is S,
    //   xx is the first 2 extra bits, and the last 2 bits are 1 (since
    //   this never occurs in a valid RS code).
  static int cpos=0;  // position in cbuf
  static U32 huff1=0, huff2=0, huff3=0, huff4=0;  // hashes of last codes
  static int rs1, rs2, rs3, rs4;  // last 4 RS codes
  static int ssum=0, ssum1=0, ssum2=0, ssum3=0, ssum4=0;
    // sum of S in RS codes in block and last 4 values

  static IntBuf cbuf2(0x20000);
  static Array<int> adv_pred(7), sumu(8), sumv(8);
  static Array<int> ls(10);  // block -> distance to previous block
  static Array<int> lcp(4), zpos(64);

    //for parsing Quantization tables
  static int dqt_state = -1, dqt_end = 0, qnum = 0;
  static Array<U8> qtab(256); // table
  static Array<int> qmap(10); // block -> table number

  const static U8 zzu[64]={  // zigzag coef -> u,v
    0,1,0,0,1,2,3,2,1,0,0,1,2,3,4,5,4,3,2,1,0,0,1,2,3,4,5,6,7,6,5,4,
    3,2,1,0,1,2,3,4,5,6,7,7,6,5,4,3,2,3,4,5,6,7,7,6,5,4,5,6,7,7,6,7};
  const static U8 zzv[64]={
    0,0,1,2,1,0,0,1,2,3,4,3,2,1,0,0,1,2,3,4,5,6,5,4,3,2,1,0,0,1,2,3,
    4,5,6,7,7,6,5,4,3,2,1,2,3,4,5,6,7,7,6,5,4,3,4,5,6,7,7,6,5,6,7,7};

  // Be sure to quit on a byte boundary
  if (!bpos) next_jpeg=jpeg>1;
  if (bpos && !jpeg) return next_jpeg;
  if (!bpos && app>0) --app;
  if (app>0) return next_jpeg;
  if (!bpos) {

    // Parse.  Baseline DCT-Huffman JPEG syntax is:
    // SOI APPx... misc... SOF0 DHT... SOS data EOI
    // SOI (= FF D8) start of image.
    // APPx (= FF Ex) len ... where len is always a 2 byte big-endian length
    //   including the length itself but not the 2 byte preceding code.
    //   Application data is ignored.  There may be more than one APPx.
    // misc codes are DQT, DNL, DRI, COM (ignored).
    // SOF0 (= FF C0) len 08 height width Nf [C HV Tq]...
    //   where len, height, width (in pixels) are 2 bytes, Nf is the repeat
    //   count (1 byte) of [C HV Tq], where C is a component identifier
    //   (color, 0-3), HV is the horizontal and vertical dimensions
    //   of the MCU (high, low bits, packed), and Tq is the quantization
    //   table ID (not used).  An MCU (minimum compression unit) consists
    //   of 64*H*V DCT coefficients for each color.
    // DHT (= FF C4) len [TcTh L1...L16 V1,1..V1,L1 ... V16,1..V16,L16]...
    //   defines Huffman table Th (1-4) for Tc (0=DC (first coefficient)
    //   1=AC (next 63 coefficients)).  L1..L16 are the number of codes
    //   of length 1-16 (in ascending order) and Vx,y are the 8-bit values.
    //   A V code of RS means a run of R (0-15) zeros followed by S (0-15)
    //   additional bits to specify the next nonzero value, negative if
    //   the first additional bit is 0 (e.g. code x63 followed by the
    //   3 bits 1,0,1 specify 7 coefficients: 0, 0, 0, 0, 0, 0, 5.
    //   Code 00 means end of block (remainder of 63 AC coefficients is 0).
    // SOS (= FF DA) len Ns [Cs TdTa]... 0 3F 00
    //   Start of scan.  TdTa specifies DC/AC Huffman tables (0-3, packed
    //   into one byte) for component Cs matching C in SOF0, repeated
    //   Ns (1-4) times.
    // EOI (= FF D9) is end of image.
    // Huffman coded data is between SOI and EOI.  Codes may be embedded:
    // RST0-RST7 (= FF D0 to FF D7) mark the start of an independently
    //   compressed region.
    // DNL (= FF DC) 04 00 height
    //   might appear at the end of the scan (ignored).
    // FF 00 is interpreted as FF (to distinguish from RSTx, DNL, EOI).

    // Detect JPEG (SOI, APPx)
    if (!jpeg && buf(4)==FF && buf(3)==SOI && buf(2)==FF && buf(1)>>4==0xe) {
      jpeg=1;
      app=sos=sof=htsize=data=mcusize=linesize=0;
      huffcode=huffbits=huffsize=mcupos=cpos=0, rs=-1;
      memset(&huf[0], 0, huf.size()*sizeof(HUF));
      memset(&pred[0], 0, pred.size()*sizeof(int));
    }

    // Detect end of JPEG when data contains a marker other than RSTx
    // or byte stuff (00).
    if (jpeg && data && buf(2)==FF && buf(1) && (buf(1)&0xf8)!=RST0) {
      jassert(buf(1)==EOI);
      jpeg=0;
    }
    if (!jpeg) return next_jpeg;

    // Detect APPx or COM field
    if (!data && !app && buf(4)==FF && (buf(3)>>4==0xe || buf(3)==COM))
      app=buf(2)*256+buf(1)+2;

    // Save pointers to sof, ht, sos, data,
    if (buf(5)==FF && buf(4)==SOS) {
      int len=buf(3)*256+buf(2);
      if (len==6+2*buf(1) && buf(1) && buf(1)<=4)  // buf(1) is Ns
        sos=pos-5, data=sos+len+2, jpeg=2;
    }
    if (buf(4)==FF && buf(3)==DHT && htsize<8) ht[htsize++]=pos-4;
    if (buf(4)==FF && buf(3)==SOF0) sof=pos-4;

    // Parse Quantizazion tables
    if (buf(4)==FF && buf(3)==DQT)
      dqt_end=pos+buf(2)*256+buf(1)-1, dqt_state=0;
    else if (dqt_state>=0) {
      if (pos>=dqt_end)
        dqt_state = -1;
      else {
        if (dqt_state%65==0)
          qnum = buf(1);
        else {
          jassert(buf(1)>0);
          jassert(qnum>=0 && qnum<4);
          qtab[qnum*64+((dqt_state%65)-1)]=buf(1)-1;
        }
        dqt_state++;
      }
    }

    // Restart
    if (buf(2)==FF && (buf(1)&0xf8)==RST0) {
      huffcode=huffbits=huffsize=mcupos=0, rs=-1;
      memset(&pred[0], 0, pred.size()*sizeof(int));
    }
  }

  {
    // Build Huffman tables
    // huf[Tc][Th][m] = min, max+1 codes of length m, pointer to byte values
    if (pos==data && bpos==1) {
      jassert(htsize>0);
      int i;
      for ( i=0; i<htsize; ++i) {
        int p=ht[i]+4;  // pointer to current table after length field
        int end=p+buf[p-2]*256+buf[p-1]-2;  // end of Huffman table
        int count=0;  // sanity check
        while (p<end && end<pos && end<p+2100 && ++count<10) {
          int tc=buf[p]>>4, th=buf[p]&15;
          if (tc>=2 || th>=4) break;
          jassert(tc>=0 && tc<2 && th>=0 && th<4);
          HUF* h=&huf[tc*64+th*16]; // [tc][th][0]; 
          int val=p+17;  // pointer to values
          int hval=tc*1024+th*256;  // pointer to RS values in hbuf
          int j;
          for ( j=0; j<256; ++j) // copy RS codes
            hbuf[hval+j]=buf[val+j];
          int code=0;
          for ( j=0; j<16; ++j) {
            h[j].min=code;
            h[j].max=code+=buf[p+j+1];
            h[j].val=hval;
            val+=buf[p+j+1];
            hval+=buf[p+j+1];
            code*=2;
          }
          p=val;
          jassert(hval>=0 && hval<2048);
        }
        jassert(p==end);
      }
      huffcode=huffbits=huffsize=0, rs=-1;

      // Build Huffman table selection table (indexed by mcupos).
      // Get image width.
      if (!sof && sos) return next_jpeg;
      int ns=buf[sos+4];
      int nf=buf[sof+9];
      jassert(ns<=4 && nf<=4);
      mcusize=0;  // blocks per MCU
      int hmax=0;  // MCU horizontal dimension
      for ( i=0; i<ns; ++i) {
        for (int j=0; j<nf; ++j) {
          if (buf[sos+2*i+5]==buf[sof+3*j+10]) { // Cs == C ?
            int hv=buf[sof+3*j+11];  // packed dimensions H x V
            if (hv>>4>hmax) hmax=hv>>4;
            hv=(hv&15)*(hv>>4);  // number of blocks in component C
            jassert(hv>=1 && hv+mcusize<=10);
            while (hv) {
              jassert(mcusize<10);
              hufsel[0][mcusize]=buf[sos+2*i+6]>>4&15;
              hufsel[1][mcusize]=buf[sos+2*i+6]&15;
              jassert (hufsel[0][mcusize]<4 && hufsel[1][mcusize]<4);
              color[mcusize]=i;
              int tq=buf[sof+3*j+12];  // quantization table index (0..3)
              jassert(tq>=0 && tq<4);
              qmap[mcusize]=tq; // quantizazion table mapping
              --hv;
              ++mcusize;
            }
          }
        }
      }
      jassert(hmax>=1 && hmax<=10);
      int j;
      for ( j=0; j<mcusize; ++j) {
        ls[j]=0;
        for (int i=1; i<mcusize; ++i) if (color[(j+i)%mcusize]==color[j]) ls[j]=i;
        ls[j]=mcusize-ls[j]<<6;
      }
      for ( j=0; j<64; ++j) zpos[zzu[j]+8*zzv[j]]=j;
      width=buf[sof+7]*256+buf[sof+8];  // in pixels
      int height=buf[sof+5]*256+buf[sof+6];
      printf("JPEG %dx%d ", width, height);
      width=(width-1)/(hmax*8)+1;  // in MCU
      jassert(width>0);
      mcusize*=64;  // coefficients per MCU
      row=column=0;
    }
  }


  // Decode Huffman
  {
    if (mcusize && buf(1+(!bpos))!=FF) {  // skip stuffed byte
      jassert(huffbits<=32);
      huffcode+=huffcode+y;
      ++huffbits;
      if (rs<0) {
        jassert(huffbits>=1 && huffbits<=16);
        const int ac=(mcupos&63)>0;
        jassert(mcupos>=0 && (mcupos>>6)<10);
        jassert(ac==0 || ac==1);
        const int sel=hufsel[ac][mcupos>>6];
        jassert(sel>=0 && sel<4);
        const int i=huffbits-1;
        jassert(i>=0 && i<16);
        const HUF *h=&huf[ac*64+sel*16]; // [ac][sel];
        jassert(h[i].min<=h[i].max && h[i].val<2048 && huffbits>0);
        if (huffcode<h[i].max) {
          jassert(huffcode>=h[i].min);
          int k=h[i].val+huffcode-h[i].min;
          jassert(k>=0 && k<2048);
          rs=hbuf[k];
          huffsize=huffbits;
        }
      }
      if (rs>=0) {
        if (huffsize+(rs&15)==huffbits) { // done decoding
          huff4=huff3;
          huff3=huff2;
          huff2=huff1;
          huff1=hash(huffcode, huffbits);
          rs4=rs3;
          rs3=rs2;
          rs2=rs1;
          rs1=rs;
          int x=0;  // decoded extra bits
          if (mcupos&63) {  // AC
            if (rs==0) { // EOB
              mcupos=mcupos+63&-64;
              jassert(mcupos>=0 && mcupos<=mcusize && mcupos<=640);
              while (cpos&63) {
                cbuf2[cpos]=0;
                cbuf[cpos++]=0;
              }
            }
            else {  // rs = r zeros + s extra bits for the next nonzero value
                    // If first extra bit is 0 then value is negative.
              jassert((rs&15)<=10);
              const int r=rs>>4;
              const int s=rs&15;
              jassert(mcupos>>6==mcupos+r>>6);
              mcupos+=r+1;
              x=huffcode&(1<<s)-1;
              if (s && !(x>>s-1)) x-=(1<<s)-1;
              for (int i=r; i>=1; --i) {
                cbuf2[cpos]=0;
                cbuf[cpos++]=i<<4|s;
              }
              cbuf2[cpos]=x;
              cbuf[cpos++]=s<<4|huffcode<<2>>s&3|12;
              ssum+=s;
            }
          }
          else {  // DC: rs = 0S, s<12
            jassert(rs<12);
            ++mcupos;
            x=huffcode&(1<<rs)-1;
            if (rs && !(x>>rs-1)) x-=(1<<rs)-1;
            jassert(mcupos>=0 && mcupos>>6<10);
            const int comp=color[mcupos>>6];
            jassert(comp>=0 && comp<4);
            dc=pred[comp]+=x;
            jassert((cpos&63)==0);
            cbuf2[cpos]=dc;
            cbuf[cpos++]=dc+1023>>3;
            ssum4=ssum3;
            ssum3=ssum2;
            ssum2=ssum1;
            ssum1=ssum;
            ssum=rs;
          }
          jassert(mcupos>=0 && mcupos<=mcusize);
          if (mcupos>=mcusize) {
            mcupos=0;
            if (++column==width) column=0, ++row;
          }
          huffcode=huffsize=huffbits=0, rs=-1;


          // UPDATE_ADV_PRED !!!!
          {
            const int acomp=mcupos>>6, q=64*qmap[acomp];
            const int zz=mcupos&63, cpos_dc=cpos-zz;
            const static int we[8]={181, 282, 353, 456, 568, 671, 742, 767};
            static int sumu2[8], sumv2[8], sumu3[8], sumv3[8], kx[32];
            if (zz == 0) {
              for (int i=0; i<8; i++) {
                update_k(sumv2[i], sumv3[i], kx[i], kx[i+16]);
                update_k(sumu2[i], sumu3[i], kx[i+8], kx[i+24]);
                sumu2[i]=sumv2[i]=sumu3[i]=sumv3[i]=0;
              }
              int cpos_dc_ls_acomp = cpos_dc-ls[acomp];
              int cpos_dc_mcusize_width = cpos_dc-mcusize*width;
              for (int i=0; i<64; i++) {
                sumu2[zzu[i]]+=we[zzv[i]]*(zzv[i]&1?-1:+1)*(qtab[q+i]+1)*cbuf2[cpos_dc_mcusize_width+i];
                sumv2[zzv[i]]+=we[zzu[i]]*(zzu[i]&1?-1:+1)*(qtab[q+i]+1)*cbuf2[cpos_dc_ls_acomp+i];
                sumu3[zzu[i]]+=(zzv[i]?(zzv[i]&1?-256:256):181)*(qtab[q+i]+1)*cbuf2[cpos_dc_mcusize_width+i];
                sumv3[zzv[i]]+=(zzu[i]?(zzu[i]&1?-256:256):181)*(qtab[q+i]+1)*cbuf2[cpos_dc_ls_acomp+i];
              }
            } else {
              sumu2[zzu[zz-1]]-=we[zzv[zz-1]]*(qtab[q+zz-1]+1)*cbuf2[cpos-1];
              sumv2[zzv[zz-1]]-=we[zzu[zz-1]]*(qtab[q+zz-1]+1)*cbuf2[cpos-1];
              sumu3[zzu[zz-1]]-=(zzv[zz-1]?256:181)*(qtab[q+zz-1]+1)*cbuf2[cpos-1];
              sumv3[zzv[zz-1]]-=(zzu[zz-1]?256:181)*(qtab[q+zz-1]+1)*cbuf2[cpos-1];
            }
            for (int i=0; i<8; ++i) {
              int k=kx[i];
              sumv[i]=(sumv2[i]*k+sumv3[i]*(8-k))/8;
              k=kx[i+8];
              sumu[i]=(sumu2[i]*k+sumu3[i]*(8-k))/8;
            }

            for (int i=0; i<3; ++i)
              for (int st=0; st<8; ++st) {
                const int zz2 = min(zz+st, 63);
                int p=(sumu[zzu[zz2]]*i+sumv[zzv[zz2]]*(2-i))/2;
                p/=(qtab[q+zz2]+1)*181;
                if (zz2==0) p-=cbuf2[cpos_dc-ls[acomp]], p=(p<0?-1:+1)*ilog(14*abs(p)+1)/10;
                else p=(p<0?-1:+1)*ilog(10*abs(p)+1)/10;
                if (st==0) {
                  adv_pred[i]=p;
                  adv_pred[i+4]=p/4;
                }
                else if (abs(p)>abs(adv_pred[i])+1) {
                  adv_pred[i]+=st*2+(p>0)<<6;
                  if (abs(p/4)>abs(adv_pred[i+4])+1) adv_pred[i+4]+=st*2+(p>0)<<6;
                  break;
                }
              }
            x=2*sumu[zzu[zz]]+2*sumv[zzv[zz]];
            for (int i=0; i<8; ++i) {
              if (zzu[zz]<i) x-=sumu[i];
              if (zzv[zz]<i) x-=sumv[i];
            }
            x/=(qtab[q+zz]+1)*181;
            if (zz==0) x-=cbuf2[cpos_dc-ls[acomp]];
            adv_pred[3]=(x<0?-1:+1)*ilog(10*abs(x)+1)/10;

            for (int i=0; i<4; ++i) {
              const int a=(i&1?zzv[zz]:zzu[zz]), b=(i&2?2:1);
              if (a<b) x=255;
              else {
                const int zz2=zpos[zzu[zz]+8*zzv[zz]-(i&1?8:1)*b];
                x=(qtab[q+zz2]+1)*cbuf2[cpos_dc+zz2]/(qtab[q+zz]+1);
                x=(x<0?-1:+1)*ilog(8*abs(x)+1)/8;
              }
              lcp[i]=x;
            }
            if (column==0) adv_pred[1]=adv_pred[2], adv_pred[0]=1;
            if (row==0) adv_pred[1]=adv_pred[0], adv_pred[2]=1;
          } // !!!!

        }
      }
    }
  }

  // Estimate next bit probability
  if (!jpeg || !data) return next_jpeg;
  if (buf(1+(!bpos))==FF) {
    m.add(128);
    return 1;
  }

  // Context model
  const int N=28; // size of t, number of contexts
  static BH<9> t(MEM);  // context hash -> bit history
    // As a cache optimization, the context does not include the last 1-2
    // bits of huffcode if the length (huffbits) is not a multiple of 3.
    // The 7 mapped values are for context+{"", 0, 00, 01, 1, 10, 11}.
  static Array<U32> cxt(N);  // context hashes
  static Array<U8*> cp(N);  // context pointers
  static StateMap sm[N];
  static Mixer m1(32, 770, 3);
  static APM a1(0x8000), a2(0x10000);


  // Update model
  if (cp[N-1]) {
    for (int i=0; i<N; ++i)
      *cp[i]=nex(*cp[i],y);
  }
  m1.update();

  // Update context
  const int comp=color[mcupos>>6];
  const int coef=(mcupos&63)|comp<<6;
  const int hc=(huffcode*2+(comp==0))|1<<(huffbits+1);
  static int hbcount=2;
  if (++hbcount>2 || huffbits==0) hbcount=0;
  jassert(coef>=0 && coef<256);
  const int zu=zzu[mcupos&63], zv=zzv[mcupos&63];
  if (hbcount==0) {
    int n=0;
    cxt[0]=hash(++n, hc, coef, adv_pred[2]);
    cxt[1]=hash(++n, hc, coef, adv_pred[0]);
    cxt[2]=hash(++n, hc, coef, adv_pred[1]);
    cxt[3]=hash(++n, hc, rs1, adv_pred[2]);
    cxt[4]=hash(++n, hc, rs1, adv_pred[0]);
    cxt[5]=hash(++n, hc, rs1, adv_pred[1]);
    cxt[6]=hash(++n, hc, adv_pred[2], adv_pred[0]);
    cxt[7]=hash(++n, hc, cbuf[cpos-width*mcusize], adv_pred[3]);
    cxt[8]=hash(++n, hc, cbuf[cpos-ls[mcupos>>6]], adv_pred[3]);
    cxt[9]=hash(++n, hc, lcp[0], lcp[1], adv_pred[1]);
    cxt[10]=hash(++n, hc, lcp[0], lcp[1], coef);
    cxt[11]=hash(++n, hc, zu, lcp[0], lcp[2]/3);
    cxt[12]=hash(++n, hc, zv, lcp[1], lcp[3]/3);
    cxt[13]=hash(++n, hc, mcupos>>2, min(3, mcupos&63));
    cxt[14]=hash(++n, hc, coef, column>>1);
    cxt[15]=hash(++n, hc, column>>2, lcp[0]+256*(lcp[2]/3), lcp[1]+256*(lcp[3]/3));
    cxt[16]=hash(++n, hc, ssum>>4, coef);
    cxt[17]=hash(++n, hc, rs1, coef);
    cxt[18]=hash(++n, hc, mcupos>>3, ssum3>>3, adv_pred[3]);
    cxt[19]=hash(++n, hc, lcp[0]/3, lcp[1]/3, adv_pred[5]);
    cxt[20]=hash(++n, hc, cbuf[cpos-width*mcusize], adv_pred[6]);
    cxt[21]=hash(++n, hc, cbuf[cpos-ls[mcupos>>6]], adv_pred[4]);
    cxt[22]=hash(++n, hc, adv_pred[2]);
    cxt[23]=hash(n, hc, adv_pred[0]);
    cxt[24]=hash(n, hc, adv_pred[1]);
    cxt[25]=hash(++n, hc, zv, lcp[1], adv_pred[6]);
    cxt[26]=hash(++n, hc, zu, lcp[0], adv_pred[4]);
    cxt[27]=hash(++n, hc, lcp[0], lcp[1], adv_pred[3]);
  }

  // Predict next bit
  m1.add(128);
  assert(hbcount<=2);
 switch(hbcount)
  {
   case 0: for (int i=0; i<N; ++i) cp[i]=t[cxt[i]]+1, m1.add(stretch(sm[i].p(*cp[i]))); break;
   case 1: { int hc=1+(huffcode&1)*3; for (int i=0; i<N; ++i) cp[i]+=hc, m1.add(stretch(sm[i].p(*cp[i]))); } break;
   default: { int hc=1+(huffcode&1); for (int i=0; i<N; ++i) cp[i]+=hc, m1.add(stretch(sm[i].p(*cp[i]))); } break;
  }

  m1.set(column==0, 2);
  m1.set(coef, 256);
  m1.set(hc&511, 512);
  int pr=m1.p();
  m.add(stretch(pr));
  pr=a1.p(pr, hc&511|(adv_pred[1]&63)<<9, 1023);
  pr=a2.p(pr, hc&255|coef<<8, 255);
  m.add(stretch(pr));
  return 2+(hc&255);
}

//////////////////////////// wavModel /////////////////////////////////

// Model a 16/8-bit stereo/mono uncompressed .wav file.  Return
// number of channels and bits per sample if a wav file is detected, else 0.
// Based on 'An asymptotically Optimal Predictor for Stereo Lossless Audio Compression'
// by Florin Ghido.

  static int S,D;
  static int wmode;

inline U32 c(int b, int i1=0, int i2=0, int i3=0, int i4=0) {
    int c;
    c=buf(i1)>>(8-b);
    if (i2) c=c<<b|buf(i2)>>(8-b);
    if (i3) c=c<<b|buf(i3)>>(8-b);
    if (i4) c=c<<b|buf(i4)>>(8-b);
    return c;
  }

inline int s2(int i) {
    return int(short(buf(i)+256*buf(i-1)));
}

inline int X(int i, int j) {
  if (wmode==18) {
     if (i<=S) return s2(i+j<<2); else return s2((i+j-S<<2)-2);
  }
     else if (wmode==17) return s2(i+j<<1);
          else if (wmode==10) {
                  if (i<=S) return buf(i+j<<1); else return buf((i+j-S<<1)-1);
          }
               else return buf(i+j);
}

int wavModel(Mixer& m) {
  static int channels;  // number of channels
  static int bits;  // bits per sample
  static int bytes;  // bytes per sample
  static int eof=0;     // end of wav
  static int s=0;  // size in bytes
  static int w,K=128>>(level-1);
  static int pr[4][2], n[2], counter[2];
  int chn,ch,msb,j,k,l,i=0;
  double sum,a=0.996;  
  double F[49][49][2],L[49][49];
  const int SC=0x20000;
  static SmallStationaryContextMap scm1(SC), scm2(SC), scm3(SC), scm4(SC), scm5(SC), scm6(SC), scm7(SC), scm8(SC);
  static ContextMap cm(MEM*4, 10);

  // Detect .wav file header
  if (!bpos && buf(8)=='d' && buf(7)=='a' && buf(6)=='t' && buf(5)=='a') {
    for (int i=32; i<=1000; i++) 
      if (buf(i)=='f' && buf(i-1)=='m' && buf(i-2)=='t' && buf(i-3)==' ' && (i2(i-8)==1||i2(i-8)==65534)) {
    bits=buf(i-22);
    bytes=bits+7>>3;
    channels=buf(i-10);
    w=channels*bytes;
    s=i4(4);
    if ((channels==1 || channels==2) && (bits==8 || bits==16)) {
      eof=pos+s;
      for (int j=0; j<channels; j++) {
          for (k=0; k<=S+D; k++) for (l=k; l<=S+D; l++) F[k][l][j]=0;
          F[1][0][j]=1;
          n[j]=counter[j]=0;
      } 
      wmode=channels+bits; 
      printf("WAV %ibits/",bits);
      if (channels==1) {printf("mono "); S=48; D=0;}
         else {printf("stereo "); S=36; D=12;} 
    }
      else eof=pos;
      }
  }
  if (pos>eof) return bits=channels=0;

  // Select previous samples and predicted sample as context
  if (!bpos) {
    msb=(pos+s-eof)%bytes;
    ch=(pos+s-eof)%w;
    chn=ch/bytes;
  if (!msb) {
    for (l=0; l<=S+D; l++) if (l<counter[chn]||(l-S-1>=0&&l-S-1<counter[chn])) F[0][l][chn]=F[0][l][chn]*a+X(0,1)*X(l,1);
    if (channels==2) {
       for (l=S+1; l<=S+D; l++) if (l-S-1<counter[chn]) F[S+1][l][chn]=F[S+1][l][chn]*a+X(S+1,1)*X(l,1);
       for (k=1; k<=S; k++) if (k<counter[chn]) F[k][S+1][chn]=F[k][S+1][chn]*a+X(k,1)*X(S+1,1);
    }
    if (++n[chn]==K) {        
       if (channels==1) for (k=1; k<=S+D; k++) for (l=k; l<=S+D; l++) F[k][l][chn]=(F[k-1][l-1][chn]-X(k-1,1)*X(l-1,1))/a;
          else for (k=1; k<=S+D; k++) if (k!=S+1) for (l=k; l<=S+D; l++) if (l!=S+1) F[k][l][chn]=(F[k-1][l-1][chn]-X(k-1,1)*X(l-1,1))/a;
       for (i=1; i<=S+D; i++) {
           sum=F[i][i][chn];
           for (k=1; k<i; k++) sum-=L[i][k]*L[i][k];
           if (sum>0) {
              L[i][i]=sqrt(sum);
              for (j=(i+1); j<=S+D; j++) {
                  sum=F[i][j][chn];
                  for (k=1; k<i; k++) sum-=L[j][k]*L[i][k];
                  L[j][i]=sum/L[i][i];
              }
           }
              else break;
       }
       if (i>S+D && counter[chn]>S+1) { 
          for (k=1; k<=S+D; k++) {
              F[k][0][chn]=F[0][k][chn];
              for (j=1; j<k; j++) F[k][0][chn]-=L[k][j]*F[j][0][chn];
              F[k][0][chn]/=L[k][k];
          }
          for (k=S+D; k>0; k--) {
              for (j=k+1; j<=S+D; j++) F[k][0][chn]-=L[j][k]*F[j][0][chn];
              F[k][0][chn]/=L[k][k];
          }
       }
       n[chn]=0;
    }
    sum=0;
    for (l=1; l<=S+D; l++) sum+=F[l][0][chn]*X(l,0);
    pr[3][chn]=pr[2][chn];
    pr[2][chn]=pr[1][chn];
    pr[1][chn]=pr[0][chn];
    pr[0][chn]=int(floor(sum));
    counter[chn]++;
    i=0;
    cm.set(hash(++i, buf(1), ch));
    cm.set(hash(++i, buf(1), buf(2), ch));
    cm.set(hash(++i, buf(1), buf(2)>>3, buf(3), ch));
    cm.set(hash(++i, s2(4)+s2(2)-s2(6)&0xff, ch));
    cm.set(hash(++i, pr[0][chn]&0xff, ch));
    cm.set(hash(++i, pr[0][chn]+s2(w)-pr[1][chn]&0xff ,ch));
    cm.set(hash(++i, pr[0][chn]&0xff, (s2(w)-pr[1][chn]+s2(w*2)-pr[2][chn]>>1)&0xff, ch));
    cm.set(hash(++i, pr[0][chn]+s2(w)*2-pr[1][chn]*2-s2(w*2)+pr[2][chn]&0xff, ch));
    scm1.set(s2(w)&0x1ff);
    scm2.set(s2(w)*2-s2(w*2)&0x1ff);
    scm3.set(s2(w)*3-s2(w*2)*3+s2(w*3)&0x1ff);
  } 
    else {
    cm.set(hash(++i, buf(1), ch));
    cm.set(hash(++i, buf(1)>>7, buf(2), buf(3)>>7, ch));
    cm.set(hash(++i, c(7, w,w*2,w*3,w*4), ch));
    cm.set(hash(++i, c(5, w,w*2,w*3,w*4), c(5, w*5,w*6), ch));
    cm.set(hash(++i, c(4, w,w*2,w*3,w*4), c(3, w*5,w*6,w*7,w*8), c(2, w*9,w*10,w*11,w*12), ch));
    cm.set(hash(++i, c(2, w,w*2,w*3,w*4)<<8|c(2, w*5,w*6,w*7,w*8), c(2, w*9,w*10,w*11,w*12)<<8|c(2, w*13,w*14,w*15,w*16),  c(2, w*17,w*18,w*19,w*20)<<8|c(2, w*21,w*22,w*23,w*24), ch));
    cm.set(hash(++i, pr[0][chn]>>8, ch));
    cm.set(hash(++i, pr[0][chn]+s2(w+1)-pr[1][chn]>>8 ,ch));
    cm.set(hash(++i, pr[0][chn]>>8, s2(w+1)-pr[1][chn]+s2(w*2+1)-pr[2][chn]>>9, ch));
    cm.set(hash(++i, pr[0][chn]+s2(w+1)*2-pr[1][chn]*2-s2(w*2+1)+pr[2][chn]>>8, ch));
    scm1.set(s2(5)+s2(3)-s2(7)-buf(1)+pr[0][chn]>>9);
    scm2.set(s2(w+1)-buf(1)+pr[0][chn]>>9);
    scm3.set(s2(w+1)*2-s2(w*2+1)-buf(1)+pr[0][chn]>>8);
    scm4.set(s2(w+1)*3-s2(w*2+1)*3+s2(w*3+1)-buf(1)>>7);
    scm5.set(s2(w-1)+s2(w+1)-buf(1)+pr[0][chn]*2>>10);
    scm7.set(s2(w+1)*4-s2(w*2+1)*6+s2(w*3+1)*4-s2(w*4+1)-buf(1)>>7);
    scm8.set(s2(w+1)*5-s2(w*2+1)*10+s2(w*3+1)*10-s2(w*4+1)*5+s2(w*5+1)-buf(1)+pr[0][chn]>>9);
    }  
  }

  // Predict next bit
  scm1.mix(m);
  scm2.mix(m);
  scm3.mix(m);
  scm4.mix(m);
  scm5.mix(m);
  scm6.mix(m);
  scm7.mix(m);
  scm8.mix(m);
  cm.mix(m);
  return channels<<8|bits;
}

//////////////////////////// exeModel /////////////////////////

// Model x86 code.  The contexts are sparse containing only those
// bits relevant to parsing (2 prefixes, opcode, and mod and r/m fields
// of modR/M byte).

// Get context at buf(i) relevant to parsing 32-bit x86 code
U32 execxt(int i, int x=0) {
  int prefix=(buf(i+2)==0x0f)+2*(buf(i+2)==0x66)+3*(buf(i+2)==0x67)
    +4*(buf(i+3)==0x0f)+8*(buf(i+3)==0x66)+12*(buf(i+3)==0x67);
  int opcode=buf(i+1);
  int modrm=i ? buf(i)&0xc7 : 0;
  return prefix|opcode<<4|modrm<<12|x<<20;
}

void exeModel(Mixer& m) {
  const int N=12;
  static ContextMap cm(MEM, N);
  if (!bpos) {
    for (int i=0; i<N; ++i)
      cm.set(execxt(i, buf(1)*(i>4)));
  }
  cm.mix(m);
}

//////////////////////////// indirectModel /////////////////////

// The context is a byte string history that occurs within a
// 1 or 2 byte context.

void indirectModel(Mixer& m) {
  static ContextMap cm(MEM, 6);
  static U32 t1[256];
  static U16 t2[0x10000];

  if (!bpos) {
    U32 d=c4&0xffff, c=d&255;
    U32& r1=t1[d>>8];
    r1=r1<<8|c;
    U16& r2=t2[c4>>8&0xffff];
    r2=r2<<8|c;
    U32 t=c|t1[c]<<8;
    cm.set(t&0xffff);
    cm.set(t&0xffffff);
    cm.set(t);
    cm.set(t&0xff00);
    t=d|t2[d]<<16;
    cm.set(t&0xffffff);
    cm.set(t);

  }
  cm.mix(m);
}

//////////////////////////// dmcModel //////////////////////////

// Model using DMC.  The bitwise context is represented by a state graph,
// initilaized to a bytewise order 1 model as in 
// http://plg.uwaterloo.ca/~ftp/dmc/dmc.c but with the following difference:
// - It uses integer arithmetic.
// - The threshold for cloning a state increases as memory is used up.
// - Each state maintains both a 0,1 count and a bit history (as in a
//   context model).  The 0,1 count is best for stationary data, and the
//   bit history for nonstationary data.  The bit history is mapped to
//   a probability adaptively using a StateMap.  The two computed probabilities
//   are combined.
// - When memory is used up the state graph is reinitialized to a bytewise
//   order 1 context as in the original DMC.  However, the bit histories
//   are not cleared.

struct DMCNode {  // 12 bytes
  unsigned int nx[2];  // next pointers
  U8 state;  // bit history
  unsigned int c0:12, c1:12;  // counts * 256
};

void dmcModel(Mixer& m) {
  static int top=0, curr=0;  // allocated, current node
  static Array<DMCNode> t(MEM*2);  // state graph
  static StateMap sm;
  static int threshold=256;

  // clone next state
  if (top>0 && top<t.size()) {
    int next=t[curr].nx[y];
    int n=y?t[curr].c1:t[curr].c0;
    int nn=t[next].c0+t[next].c1;
    if (n>=threshold*2 && nn-n>=threshold*3) {
      int r=n*4096/nn;
      assert(r>=0 && r<=4096);
      t[next].c0 -= t[top].c0 = t[next].c0*r>>12;
      t[next].c1 -= t[top].c1 = t[next].c1*r>>12;
      t[top].nx[0]=t[next].nx[0];
      t[top].nx[1]=t[next].nx[1];
      t[top].state=t[next].state;
      t[curr].nx[y]=top;
      ++top;
      if (top==MEM*2) threshold=512;
      if (top==MEM*3) threshold=768;
    }
  }

  // Initialize to a bytewise order 1 model at startup or when flushing memory
  if (top==t.size() && bpos==1) top=0;
  if (top==0) {
    assert(t.size()>=65536);
    for (int i=0; i<256; ++i) {
      for (int j=0; j<256; ++j) {
        if (i<127) {
          t[j*256+i].nx[0]=j*256+i*2+1;
          t[j*256+i].nx[1]=j*256+i*2+2;
        }
        else {
          t[j*256+i].nx[0]=(i-127)*256;
          t[j*256+i].nx[1]=(i+1)*256;
        }
        t[j*256+i].c0=128;
        t[j*256+i].c1=128;
      }
    }
    top=65536;
    curr=0;
    threshold=256;
  }

  // update count, state
  if (y) {
    if (t[curr].c1<3800) t[curr].c1+=256;
  }
  else if (t[curr].c0<3800) t[curr].c0+=256;
  t[curr].state=nex(t[curr].state, y);
  curr=t[curr].nx[y];

  // predict
  const int pr1=sm.p(t[curr].state);
  const int n1=t[curr].c1;
  const int n0=t[curr].c0;
  const int pr2=(n1+5)*4096/(n0+n1+10);
  m.add(stretch(pr1));
  m.add(stretch(pr2));
}

//////////////////////////// contextModel //////////////////////

typedef enum {DEFAULT, JPEG, BMPFILE4, BMPFILE8, BMPFILE24, TIFFFILE,
              PGMFILE, RGBFILE, EXE, TEXT} Filetype;

// This combines all the context models with a Mixer.

int contextModel2() {
  static ContextMap cm(MEM*32, 9);
  static RunContextMap rcm7(MEM), rcm9(MEM), rcm10(MEM);
  static Mixer m(800, 3088, 7, 128);
  static U32 cxt[16];  // order 0-11 contexts
  static Filetype filetype=DEFAULT;
  static int size=0;  // bytes remaining in block
//  static const char* typenames[4]={"", "jpeg ", "exe ", "text "};

  // Parse filetype and size
  if (bpos==0) {
    --size;
    if (size==-1) filetype=(Filetype)buf(1);
    if (size==-5) {
      size=buf(4)<<24|buf(3)<<16|buf(2)<<8|buf(1);
//      if (filetype<=3) printf("(%s%d)", typenames[filetype], size);
      if (filetype==EXE) size+=8;
    }
  }

  m.update();
  m.add(256);

  // Test for special file types
  int ismatch=ilog(matchModel(m));  // Length of longest matching context
  int iswav=wavModel(m);  // number of channels and bits per sample if WAV is detected, else 0
  if (filetype==JPEG){
     int isjpeg=jpegModel(m);  // 1-257 if JPEG is detected, else 0
     if (isjpeg) {
        m.set(1, 8);
        m.set(isjpeg-1, 257);
        m.set(buf(1), 256);
       return m.p();
     }
  }
  if (filetype==BMPFILE24 || filetype==TIFFFILE){ 
     int isbmp=bmpModel(m); // Image width (bytes) if BMP or TIFF detected, or 0
     if (isbmp>0) {
       static int col=0;
       if (++col>=24) col=0;
       m.set(2, 8);
       m.set(col, 24);
       m.set(buf(isbmp)+buf(3)>>4, 32);
       m.set(c0, 256);
       return m.p();
     }
  }
  if (filetype==PGMFILE){
     if (pgmModel(m)>0) return m.p(); // Image width (bytes) if PGM (P5,PGM_MAXVAL = 255) detected, or 0
  }
  if (filetype==BMPFILE8){ 
     if (bmpModel8(m)>0) return m.p(); // Image width (bytes) if BMP8 detected, or 0 
  }
if (filetype==RGBFILE){ 
     if (rgbModel8(m)>0) return m.p(); // Image width (bytes) if RGB8 detected, or 0 
  }
  if (iswav>0) {
    int bits=iswav&0xff;
    int tbits=(iswav>>8)*bits;
    static int col=0;
    if (++col>=tbits) col=0;
    if (tbits!=bits) m.set(col, tbits);
    m.set(col, bits);
    m.set(c0, 256);
    return m.p();
  }

  // Normal model
  if (bpos==0) {
    int i;
    for ( i=15; i>0; --i)  // update order 0-11 context hashes
      cxt[i]=cxt[i-1]*257+(c4&255)+1;
    for ( i=0; i<7; ++i)
      cm.set(cxt[i]);
    rcm7.set(cxt[7]);
    cm.set(cxt[8]);
    rcm9.set(cxt[10]);
    rcm10.set(cxt[12]);
    cm.set(cxt[14]);
  }
  int order=cm.mix(m);
  
  rcm7.mix(m);
  rcm9.mix(m);
  rcm10.mix(m);

  if (level>=4) {
    sparseModel(m,ismatch,order);
    distanceModel(m);
    picModel(m);
    recordModel(m);  
    wordModel(m);
    indirectModel(m);
    dmcModel(m);
    if (filetype==EXE) exeModel(m);
  }



  order = order-2;
  if(order<0) order=0;

  U32 c1=buf(1), c2=buf(2), c3=buf(3), c;

  m.set(c1+8, 264);
  m.set(c0, 256);
  m.set(order+8*(c4>>5&7)+64*(c1==c2)+128*(filetype==EXE), 256);
  m.set(c2, 256);
  m.set(c3, 256);
  m.set(ismatch, 256);
  
  if(bpos)
  {	
    c=c0<<(8-bpos); if(bpos==1)c+=c3/2;
    c=(min(bpos,5))*256+c1/32+8*(c2/32)+(c&192);
  }
  else c=c3/128+(c4>>31)*2+4*(c2/64)+(c1&240);
  m.set(c, 1536);
  int pr=m.p();
  return pr;
}


//////////////////////////// Predictor /////////////////////////

// A Predictor estimates the probability that the next bit of
// uncompressed data is 1.  Methods:
// p() returns P(1) as a 12 bit number (0-4095).
// update(y) trains the predictor with the actual bit (0 or 1).

class Predictor {
  int pr;  // next prediction
public:
  Predictor();
  int p() const {assert(pr>=0 && pr<4096); return pr;}
  void update();
};

Predictor::Predictor(): pr(2048) {}

void Predictor::update() {
  static APM1 a(256), a1(0x10000), a2(0x10000), a3(0x10000),
                      a4(0x10000), a5(0x10000), a6(0x10000);

  // Update global context: pos, bpos, c0, c4, buf
  c0+=c0+y;
  if (c0>=256) {
    buf[pos++]=c0;
    c4=(c4<<8)+c0-256;
    c0=1;
  }
  bpos=(bpos+1)&7;

  // Filter the context model with APMs
  int pr0=contextModel2();

  pr=a.p(pr0, c0);
  
  int pr1=a1.p(pr0, c0+256*buf(1));
  int pr2=a2.p(pr0, c0^hash(buf(1), buf(2))&0xffff);
  int pr3=a3.p(pr0, c0^hash(buf(1), buf(2), buf(3))&0xffff);
  pr0=pr0+pr1+pr2+pr3+2>>2;
  
      pr1=a4.p(pr, c0+256*buf(1));
      pr2=a5.p(pr, c0^hash(buf(1), buf(2))&0xffff);
      pr3=a6.p(pr, c0^hash(buf(1), buf(2), buf(3))&0xffff);
  pr=pr+pr1+pr2+pr3+2>>2;

  pr=pr+pr0+1>>1;
}

//////////////////////////// Encoder ////////////////////////////

// An Encoder does arithmetic encoding.  Methods:
// Encoder(COMPRESS, f) creates encoder for compression to archive f, which
//   must be open past any header for writing in binary mode.
// Encoder(DECOMPRESS, f) creates encoder for decompression from archive f,
//   which must be open past any header for reading in binary mode.
// code(i) in COMPRESS mode compresses bit i (0 or 1) to file f.
// code() in DECOMPRESS mode returns the next decompressed bit from file f.
//   Global y is set to the last bit coded or decoded by code().
// compress(c) in COMPRESS mode compresses one byte.
// decompress() in DECOMPRESS mode decompresses and returns one byte.
// flush() should be called exactly once after compression is done and
//   before closing f.  It does nothing in DECOMPRESS mode.
// size() returns current length of archive
// setFile(f) sets alternate source to FILE* f for decompress() in COMPRESS
//   mode (for testing transforms).
// If level (global) is 0, then data is stored without arithmetic coding.

typedef enum {COMPRESS, DECOMPRESS} Mode;
class Encoder {
private:
  Predictor predictor;
  const Mode mode;       // Compress or decompress?
  FILE* archive;         // Compressed data file
  U32 x1, x2;            // Range, initially [0, 1), scaled by 2^32
  U32 x;                 // Decompress mode: last 4 input bytes of archive
  FILE *alt;             // decompress() source in COMPRESS mode

  // Compress bit y or return decompressed bit
  int code(int i=0) {
    int p=predictor.p();
    assert(p>=0 && p<4096);
    p+=p<2048;
    U32 xmid=x1 + (x2-x1>>12)*p + ((x2-x1&0xfff)*p>>12);
    assert(xmid>=x1 && xmid<x2);
    if (mode==DECOMPRESS) y=x<=xmid; else y=i;
    y ? (x2=xmid) : (x1=xmid+1);
    predictor.update();
    while (((x1^x2)&0xff000000)==0) {  // pass equal leading bytes of range
      if (mode==COMPRESS) putc(x2>>24, archive);
      x1<<=8;
      x2=(x2<<8)+255;
      if (mode==DECOMPRESS) x=(x<<8)+(getc(archive)&255);  // EOF is OK
    }
    return y;
  }

public:
  Encoder(Mode m, FILE* f);
  Mode getMode() const {return mode;}
  long size() const {return ftell(archive);}  // length of archive so far
  void flush();  // call this when compression is finished
  void setFile(FILE* f) {alt=f;}

  // Compress one byte
  void compress(int c) {
    assert(mode==COMPRESS);
    if (level==0)
      putc(c, archive);
    else 
      for (int i=7; i>=0; --i)
        code((c>>i)&1);
  }

  // Decompress and return one byte
  int decompress() {
    if (mode==COMPRESS) {
      assert(alt);
      return getc(alt);
    }
    else if (level==0)
      return getc(archive);
    else {
      int c=0;
      for (int i=0; i<8; ++i)
        c+=c+code();
      return c;
    }
  }
};

Encoder::Encoder(Mode m, FILE* f): 
    mode(m), archive(f), x1(0), x2(0xffffffff), x(0), alt(0) {
  if (level>0 && mode==DECOMPRESS) {  // x = first 4 bytes of archive
    for (int i=0; i<4; ++i)
      x=(x<<8)+(getc(archive)&255);
  }
  for (int i=0; i<1024; ++i)
    dt[i]=16384/(i+i+3);

}

void Encoder::flush() {
  if (mode==COMPRESS && level>0)
    putc(x1>>24, archive);  // Flush first unequal byte of range
}

/////////////////////////// Filters /////////////////////////////////
//
// Before compression, data is encoded in blocks with the following format:
//
//   <type> <size> <encoded-data>
//
// Type is 1 byte (type Filetype): DEFAULT=0, JPEG, EXE
// Size is 4 bytes in big-endian format.
// Encoded-data decodes to <size> bytes.  The encoded size might be
// different.  Encoded data is designed to be more compressible.
//
//   void encode(FILE* in, FILE* out, int n);
//
// Reads n bytes of in (open in "rb" mode) and encodes one or
// more blocks to temporary file out (open in "wb+" mode).
// The file pointer of in is advanced n bytes.  The file pointer of
// out is positioned after the last byte written.
//
//   en.setFile(FILE* out);
//   int decode(Encoder& en);
//
// Decodes and returns one byte.  Input is from en.decompress(), which
// reads from out if in COMPRESS mode.  During compression, n calls
// to decode() must exactly match n bytes of in, or else it is compressed
// as type 0 without encoding.
//
//   Filetype detect(FILE* in, int n, Filetype type);
//
// Reads n bytes of in, and detects when the type changes to
// something else.  If it does, then the file pointer is repositioned
// to the start of the change and the new type is returned.  If the type
// does not change, then it repositions the file pointer n bytes ahead
// and returns the old type.
//
// For each type X there are the following 2 functions:
//
//   void encode_X(FILE* in, FILE* out, int n, ...);
//
// encodes n bytes from in to out.
//
//   int decode_X(Encoder& en);
//
// decodes one byte from en and returns it.  decode() and decode_X()
// maintain state information using static variables.
#define bswap(x)	\
+   ((((x) & 0xff000000) >> 24) | \
+    (((x) & 0x00ff0000) >>  8) | \
+    (((x) & 0x0000ff00) <<  8) | \
+    (((x) & 0x000000ff) << 24))

// Detect EXE or JPEG data
Filetype detect(FILE* in, int n, Filetype type) {
  U32 buf1=0, buf0=0;  // last 8 bytes
  long start=ftell(in);

  // For EXE detection
  Array<int> abspos(256),  // CALL/JMP abs. addr. low byte -> last offset
    relpos(256);    // CALL/JMP relative addr. low byte -> last offset
  int e8e9count=0;  // number of consecutive CALL/JMPs
  int e8e9pos=0;    // offset of first CALL or JMP instruction
  int e8e9last=0;   // offset of most recent CALL or JMP
  // For BMP detection
  int bmp=0,bmp0=0,bsize=0,imgbpp=0,bmpx=0,bmpy=0,bmpimgoff=0,imgcomp=-1;
  // For PGM detection
  int pgm=0,psize=0,pgmcomment=0,pgmw=0,pgmh=0,pgmsize=0,pgm_ptr=0,pgmc=0;
  char pgm_buf[32];
  // For JPEG detection
  int soi=0, sof=0, sos=0, app=0;  // position where found
  // For .RGB detection
  int rgbi=0,rgbSTORAGE=-1,rgbBPC=0,rgbDIMENSION=0,rgbZSIZE=0,rgbXSIZE=0,rgbYSIZE=0,rgbsize=0;
  for (int i=0; i<n; ++i) {
    int c=getc(in);
    if (c==EOF) return (Filetype)(-1);
    buf1=buf1<<8|buf0>>24;
    buf0=buf0<<8|c;

    // Detect JPEG by code SOI APPx (FF D8 FF Ex) followed by
    // SOF0 (FF C0 xx xx 08) and SOS (FF DA) within a reasonable distance.
    // Detect end by any code other than RST0-RST7 (FF D9-D7) or
    // a byte stuff (FF 00).

    if (!soi && i>=3 && (buf0&0xfffffff0)==0xffd8ffe0) soi=i, app=i+2;
    if (soi) {
        if (app==i && ((buf0&0xfff00000)==0xffe00000 || (buf0&0xffff0000)==0xfffe0000))
          app=i+(buf0&0xffff)+2;    
        if (app<i && i-soi<0x10000 && (buf1&0xff)==0xff
            && (buf0&0xff0000ff)==0xc0000008)
          sof=i;
        if (sof && sof>soi && i-soi<0x10000 && i-sof<0x1000
            && (buf0&0xffff)==0xffda) {
          sos=i;
          if (type!=JPEG) return fseek(in, start+soi-3, SEEK_SET), JPEG;
        }
    }
    if (type==JPEG && sos && i>sos && (buf0&0xff00)==0xff00
        && (buf0&0xff)!=0 && (buf0&0xf8)!=0xd0)
      return DEFAULT;

	// Detect .bmp image
    
    if ((buf0&0xFFFF)==16973) bmp=i;                //possible 'BM'
    if (bmp){
		if ((i-bmp)==4) bsize=bswap(buf0);          //image size
		if ((i-bmp)==12) bmpimgoff=bswap(buf0);
        if ((i-bmp)==16 && buf0!=0x28000000) bmp=imgbpp=bsize=0,imgcomp=-1; //if windows bmp
		if ((i-bmp)==20){
			bmpx=bswap(buf0);                       //image x size
			if (bmpx==0) bmp=imgbpp=bsize=0,imgcomp=-1; // Test big size?
		}
		if ((i-bmp)==24){
			bmpy=bswap(buf0);                       //image y size
			if (bmpy==0) bmp=imgbpp=bsize=0,imgcomp=-1;
		}	
		if ((i-bmp)==27) imgbpp=c;                  //image bpp
		if ((i-bmp)==31){
                         imgcomp=buf0;              //image compression 0=none, 1=RLE-8, 2=RLE-4		
                         if (imgcomp!=0) bmp=imgbpp=bsize=0,imgcomp=-1;}
		if ((type==BMPFILE4 || type==BMPFILE8 || type==BMPFILE24 ) && (imgbpp==4 || imgbpp==8 || imgbpp==24) && imgcomp==0){
            int tbsize=0;
            if (imgbpp==4)
                if (bsize !=(tbsize=((bmpx*bmpy>>1)+bmpimgoff))) bsize=tbsize;
            if (imgbpp==8)
                if (bsize !=(tbsize=(bmpx*bmpy+bmpimgoff))) bsize=tbsize;
            if (imgbpp==24)
                if (bsize !=(tbsize=(bmpx*bmpy*3+bmpimgoff))) bsize=tbsize;
			return fseek(in, start+bsize, SEEK_SET),DEFAULT;
		}
		if (imgbpp==4 && imgcomp==0){
			return 	fseek(in, start+bmp-1, SEEK_SET),BMPFILE4;
		}
		if (imgbpp==8 && imgcomp==0){
			return 	fseek(in, start+bmp-1, SEEK_SET),BMPFILE8;
		}
		if (imgbpp==24 && imgcomp==0){
			return 	fseek(in, start+bmp-1, SEEK_SET),BMPFILE24;
		}
    }
    // Detect .pgm image
    if ((buf0&0xFFFFFF)==0x50350A) pgm=i;           //possible 'P5 '
    if (pgm){
		if ((i-pgm)==1 && c==0x23) pgmcomment=1; //pgm comment
		//not tested without comment
		if (!pgmcomment && c==0x20 && !pgmw && pgm_ptr) {
			pgm_buf[pgm_ptr++]=0;
			pgmw=atoi(pgm_buf);
			if (pgmw==0) pgm=pgm_ptr=pgmw=pgmh=pgmc=pgmcomment=0;			
			pgm_ptr=0;
		}
		if (!pgmcomment && c==0x0a && !pgmh && pgm_ptr){
			pgm_buf[pgm_ptr++]=0;
			pgmh=atoi(pgm_buf);
			if (pgmh==0) pgm=pgm_ptr=pgmw=pgmh=pgmc=pgmcomment=0;
			pgm_ptr=0;
		}
		if (!pgmcomment && c==0x0a && !pgmc && pgm_ptr){
			pgm_buf[pgm_ptr++]=0;
			pgmc=atoi(pgm_buf);
			pgm_ptr=0;
		}
		if (!pgmcomment) pgm_buf[pgm_ptr++]=c;
		if (pgm_ptr>=32) pgm=pgm_ptr=pgmw=pgmh=pgmc=pgmcomment=0;
		if (pgmcomment && c==0x0a) pgmcomment=0;
		if (type==PGMFILE && pgmw && pgmh && pgmc){
			pgmsize=pgmw *pgmh +pgm+i-1;
			return fseek(in, start+pgmsize, SEEK_SET),DEFAULT;
		}
     	if (pgmw && pgmh && pgmc){
		     return fseek(in, start+pgm-2, SEEK_SET),PGMFILE;
        }
    }
    // Detect .rgb image
	if ((buf0&0xFFFF)==0x01DA) rgbi=i;
    if (rgbi){
        if ((i-rgbi)==1)
		    if (c==0 || c==1)
			    rgbSTORAGE=c; //0 uncompressed, 1 RLE compressed
            else
			    rgbi=rgbBPC=rgbDIMENSION=rgbZSIZE=rgbXSIZE=rgbYSIZE=0,rgbSTORAGE=-1;
	    if ((i-rgbi)==2)
		    if  (c==1 || c==2) 
    			rgbBPC=c;
       		else
      			rgbi=rgbBPC=rgbDIMENSION=rgbZSIZE=rgbXSIZE=rgbYSIZE=0,rgbSTORAGE=-1;
        if ((i-rgbi)==4) 
        	if ((buf0&0xFFFF)==1 || (buf0&0xFFFF)==2 || (buf0&0xFFFF)==3) 
		    	rgbDIMENSION=buf0&0xFFFF;
		    else
			    rgbi=rgbBPC=rgbDIMENSION=rgbZSIZE=rgbXSIZE=rgbYSIZE=0,rgbSTORAGE=-1;
    	if ((i-rgbi)==6) 
	    	if ((buf0&0xFFFF)>0) 
		    	rgbXSIZE=buf0&0xFFFF;
		    else
			    rgbi=rgbBPC=rgbDIMENSION=rgbZSIZE=rgbXSIZE=rgbYSIZE=0,rgbSTORAGE=-1;
    	if ((i-rgbi)==8) 
		if ((buf0&0xFFFF)>0) 
			rgbYSIZE=buf0&0xFFFF,rgbsize=rgbYSIZE*rgbXSIZE+512;
		else
			rgbi=rgbBPC=rgbDIMENSION=rgbZSIZE=rgbXSIZE=rgbYSIZE=0,rgbSTORAGE=-1;
    	if ((i-rgbi)==10) 
	    	if ((buf0&0xFFFF)==1 || (buf0&0xFFFF)==3 || (buf0&0xFFFF)==4)  // 1 indicates greyscale
		    															   // 3 indicates RGB
			    														   // 4 indicates RGB and Alpha
    			rgbZSIZE=buf0&0xFFFF;
	    	else
		    	rgbi=rgbBPC=rgbDIMENSION=rgbZSIZE=rgbXSIZE=rgbYSIZE=0,rgbSTORAGE=-1;
		if (rgbsize != 0  && (i-rgbi)>0 && ((i-rgbi)>rgbsize)){
			if (type==RGBFILE  && rgbZSIZE==1 && rgbSTORAGE==0 ){ //uncompressed greyscale
				return fseek(in, start+rgbsize, SEEK_SET),DEFAULT;
			}
			if (rgbZSIZE==1 && rgbSTORAGE==0){
				return 	fseek(in, start+rgbi-1, SEEK_SET),RGBFILE;
			}
		}
    }
    //TIFF support needed
    // Detect .tiff file
    
    
    // Detect EXE if the low order byte (little-endian) XX is more
    // recently seen (and within 4K) if a relative to absolute address
    // conversion is done in the context CALL/JMP (E8/E9) XX xx xx 00/FF
    // 4 times in a row.  Detect end of EXE at the last
    // place this happens when it does not happen for 64KB.

    if ((buf1&0xfe)==0xe8 && (buf0+1&0xfe)==0) {
      int r=buf0>>24;  // relative address low 8 bits
      int a=(buf0>>24)+i&0xff;  // absolute address low 8 bits
      int rdist=i-relpos[r];
      int adist=i-abspos[a];
      if (adist<rdist && adist<0x1000 && abspos[a]>5) {
        e8e9last=i;
        ++e8e9count;
        if (e8e9pos==0 || e8e9pos>abspos[a]) e8e9pos=abspos[a];
      }
      else e8e9count=0;
      if (type!=EXE && e8e9count>=4 && e8e9pos>5)
        return fseek(in, start+e8e9pos-5, SEEK_SET), EXE;
      abspos[a]=i;
      relpos[r]=i;
    }
    if (type==EXE && i-e8e9last>0x1000)
      return fseek(in, start+e8e9last, SEEK_SET), DEFAULT;
  }
  return type;
}

// Default encoding as self
void encode_default(FILE* in, FILE* out, int len) {
  while (len--) putc(getc(in), out);
}

int decode_default(Encoder& en) {
  return en.decompress();
}

// JPEG encode as self.  The purpose is to shield jpegs from exe transform.
void encode_jpeg(FILE* in, FILE* out, int len) {
  while (len--) putc(getc(in), out);
}

int decode_jpeg(Encoder& en) {
  return en.decompress();
}
// BMP encode as self.
void encode_bmp(FILE* in, FILE* out, int len) {
  while (len--) putc(getc(in), out);
}

int decode_bmp(Encoder& en) {
  return en.decompress();
}

// PGM encode as self.
void encode_pgm(FILE* in, FILE* out, int len) {
  while (len--) putc(getc(in), out);
}

int decode_pgm(Encoder& en) {
  return en.decompress();
}

// RGB encode as self.
void encode_rgb(FILE* in, FILE* out, int len) {
  while (len--) putc(getc(in), out);
}

int decode_rgb(Encoder& en) {
  return en.decompress();
}

// EXE transform: <encoded-size> <begin> <block>...
// Encoded-size is 4 bytes, MSB first.
// begin is the offset of the start of the input file, 4 bytes, MSB first.
// Each block applies the e8e9 transform to strings falling entirely
// within the block starting from the end and working backwards.
// The 5 byte pattern is E8/E9 xx xx xx 00/FF (x86 CALL/JMP xxxxxxxx)
// where xxxxxxxx is a relative address LSB first.  The address is
// converted to an absolute address by adding the offset mod 2^25
// (in range +-2^24).

void encode_exe(FILE* in, FILE* out, int len, int begin) {
  const int BLOCK=0x10000;
  Array<U8> blk(BLOCK);
  fprintf(out, "%c%c%c%c", len>>24, len>>16, len>>8, len); // size, MSB first
  fprintf(out, "%c%c%c%c", begin>>24, begin>>16, begin>>8, begin); 

  // Transform
  for (int offset=0; offset<len; offset+=BLOCK) {
    int size=min(len-offset, BLOCK);
    int bytesRead=fread(&blk[0], 1, size, in);
    if (bytesRead!=size) quit("encode_exe read error");
    for (int i=bytesRead-1; i>=4; --i) {
      if ((blk[i-4]==0xe8||blk[i-4]==0xe9) && (blk[i]==0||blk[i]==0xff)) {
        int a=(blk[i-3]|blk[i-2]<<8|blk[i-1]<<16|blk[i]<<24)+offset+begin+i+1;
        a<<=7;
        a>>=7;
        blk[i]=a>>24;
        blk[i-1]=a>>16;
        blk[i-2]=a>>8;
        blk[i-3]=a;
      }
    }
    fwrite(&blk[0], 1, bytesRead, out);
  }
}

int decode_exe(Encoder& en) {
  const int BLOCK=0x10000;  // block size
  static int offset=0, q=0;  // decode state: file offset, queue size
  static int size=0;  // where to stop coding
  static int begin=0;  // offset in file
  static U8 c[5];  // queue of last 5 bytes, c[0] at front

  // Read size from first 4 bytes, MSB first
  while (offset==size && q==0) {
    offset=0;
    size=en.decompress()<<24;
    size|=en.decompress()<<16;
    size|=en.decompress()<<8;
    size|=en.decompress();
    begin=en.decompress()<<24;
    begin|=en.decompress()<<16;
    begin|=en.decompress()<<8;
    begin|=en.decompress();
  }

  // Fill queue
  while (offset<size && q<5) {
    memmove(c+1, c, 4);
    c[0]=en.decompress();
    ++q;
    ++offset;
  }

  // E8E9 transform: E8/E9 xx xx xx 00/FF -> subtract location from x
  if (q==5 && (c[4]==0xe8||c[4]==0xe9) && (c[0]==0||c[0]==0xff)
      && ((offset-1^offset-5)&-BLOCK)==0) { // not crossing block boundary
    int a=(c[3]|c[2]<<8|c[1]<<16|c[0]<<24)-offset-begin;
    a<<=7;
    a>>=7;
    c[3]=a;
    c[2]=a>>8;
    c[1]=a>>16;
    c[0]=a>>24;
  }

  // return oldest byte in queue
  assert(q>0 && q<=5);
  return c[--q];
}



// Split n bytes into blocks by type.  For each block, output
// <type> <size> and call encode_X to convert to type X.
void encode(FILE* in, FILE* out, int n) {
  Filetype type=DEFAULT;
  long begin=ftell(in);
  while (n>0) {
    Filetype nextType=detect(in, n, type);
    long end=ftell(in);
    fseek(in, begin, SEEK_SET);
    int len=int(end-begin);
    if (len>0) {
      fprintf(out, "%c%c%c%c%c", type, len>>24, len>>16, len>>8, len);
      switch(type) {
        case JPEG: encode_jpeg(in, out, len); break;
        case BMPFILE4:
		case BMPFILE8:
		case BMPFILE24:
			encode_bmp(in, out, len); break;
		case PGMFILE: encode_pgm(in, out, len); break;
		case RGBFILE: encode_rgb(in, out, len); break;
        case EXE:  encode_exe(in, out, len, begin); break;
        default:   encode_default(in, out, len); break;
      }
    }
    n-=len;
    type=nextType;
    begin=end;
  }
}

// Decode <type> <len> <data>...
int decode(Encoder& en) {
  static Filetype type=DEFAULT;
  static int len=0;
  while (len==0) {
    type=(Filetype)en.decompress();
    len=en.decompress()<<24;
    len|=en.decompress()<<16;
    len|=en.decompress()<<8;
    len|=en.decompress();
    if (len<0) len=1;
  }
  --len;
  switch (type) {
    case JPEG: return decode_jpeg(en);
    case BMPFILE4:
    case BMPFILE8:
    case BMPFILE24:
		return decode_bmp(en);
    case PGMFILE: return decode_pgm(en);
	case RGBFILE: return decode_rgb(en);
    case EXE:  return decode_exe(en);
    default:   return decode_default(en);
  }
}

//////////////////// Compress, Decompress ////////////////////////////

// Print progress: n is the number of bytes compressed or decompressed
void printStatus(int n) {
  if (n>0 && !(n&0x0fff))
    printf("%12d\b\b\b\b\b\b\b\b\b\b\b\b", n), fflush(stdout);
}

// Compress a file
void compress(const char* filename, long filesize, Encoder& en) {
  assert(en.getMode()==COMPRESS);
  assert(filename && filename[0]);
  FILE *f=fopen(filename, "rb");
  if (!f) perror(filename), quit();
  long start=en.size();
  printf("%s %ld -> ", mybasename(filename), filesize);

  // Transform and test in blocks
  const int BLOCK=MEM*64;
  for (int i=0; filesize>0; i+=BLOCK) {
    int size=BLOCK;
    if (size>filesize) size=filesize;
    FILE* tmp=tmpfile();
    if (!tmp) perror("tmpfile"), quit();
    long savepos=ftell(f);
    encode(f, tmp, size);

    // Test transform
    rewind(tmp);
    en.setFile(tmp);
    fseek(f, savepos, SEEK_SET);
    long j;
    int c1=0, c2=0;
    for (j=0; j<size; ++j)
      if ((c1=decode(en))!=(c2=getc(f))) break;

    // Test fails, compress without transform
    if (j!=size || getc(tmp)!=EOF) {
      printf("Transform fails at %ld, input=%d decoded=%d, skipping...\n", i+j, c2, c1);
      en.compress(0);
      en.compress(size>>24);
      en.compress(size>>16);
      en.compress(size>>8);
      en.compress(size);
      fseek(f, savepos, SEEK_SET);
      for (int j=0; j<size; ++j) {
        printStatus(i+j);
        en.compress(getc(f));
      }
    }

    // Test succeeds, decode(encode(f)) == f, compress tmp
    else {
      rewind(tmp);
      int c;
      j=0;
      while ((c=getc(tmp))!=EOF) {
        printStatus(i+j++);
        en.compress(c);
      }
    }
    filesize-=size;
    fclose(tmp);  // deletes
  }
  if (f) fclose(f);
  printf("%-12ld\n", en.size()-start);
}

// Try to make a directory, return true if successful
bool makedir(const char* dir) {
#ifdef WINDOWS
  return CreateDirectory(dir, 0)==TRUE;
#else
#ifdef UNIX
  return mkdir(dir, 0777)==0;
#else
  return false;
#endif
#endif
}

// Decompress a file
void decompress(const char* filename, long filesize, Encoder& en) {
  assert(en.getMode()==DECOMPRESS);
  assert(filename && filename[0]);

  // Test if output file exists.  If so, then compare.
  FILE* f=fopen(filename, "rb");
  if (f) {
    printf("Comparing %s %ld -> ", mybasename(filename), filesize);
    bool found=false;  // mismatch?
    for (int i=0; i<filesize; ++i) {
      printStatus(i);
      int c1=found?EOF:getc(f);
      int c2=decode(en);
      if (c1!=c2 && !found) {
        printf("differ at %d: file=%d archive=%d\n", i, c1, c2);
        found=true;
      }
    }
    if (!found && getc(f)!=EOF)
      printf("file is longer\n");
    else if (!found)
      printf("identical   \n");
    fclose(f);
  }

  // Create file
  else {
    f=fopen(filename, "wb");
    if (!f) {  // Try creating directories in path and try again
      String path(filename);
      for (int i=0; path[i]; ++i) {
        if (path[i]=='/' || path[i]=='\\') {
          char savechar=path[i];
          path[i]=0;
          if (makedir(path.c_str()))
            printf("Created directory %s\n", path.c_str());
          path[i]=savechar;
        }
      }
      f=fopen(filename, "wb");
    }

    // Decompress
    if (f) {
      printf("Extracting %s %ld -> ", filename, filesize);
      for (int i=0; i<filesize; ++i) {
        printStatus(i);
        putc(decode(en), f);
      }
      fclose(f);
      printf("done        \n");
    }

    // Can't create, discard data
    else {
      perror(filename);
      printf("Skipping %s %ld -> ", filename, filesize);
      for (int i=0; i<filesize; ++i) {
        printStatus(i);
        decode(en);
      }
      printf("not extracted\n");
    }
  }
}

//////////////////////////// User Interface ////////////////////////////

// Read one line, return NULL at EOF or ^Z.  f may be opened ascii or binary.
// Trailing \r\n is dropped.  Line length is unlimited.

const char* getline(FILE *f=stdin) {
  static String s;
  int len=0, c;
  while ((c=getc(f))!=EOF && c!=26 && c!='\n') {
    if (len>=s.size()) s.resize(len*2+1);
    if (c!='\r') s[len++]=c;
  }
  if (len>=s.size()) s.resize(len+1);
  s[len]=0;
  if (c==EOF || c==26)
    return 0;
  else
    return s.c_str();
}

// int expand(String& archive, String& s, const char* fname, int base) {
// Given file name fname, print its length and base name (beginning
// at fname+base) to archive in format "%ld\t%s\r\n" and append the
// full name (including path) to String s in format "%s\n".  If fname
// is a directory then substitute all of its regular files and recursively
// expand any subdirectories.  Base initially points to the first
// character after the last / in fname, but in subdirectories includes
// the path from the topmost directory.  Return the number of files
// whose names are appended to s and archive.

// Same as expand() except fname is an ordinary file
int putsize(String& archive, String& s, const char* fname, int base) {
  int result=0;
  FILE *f=fopen(fname, "rb");
  if (f) {
    fseek(f, 0, SEEK_END);
    long len=ftell(f);
    if (len>=0) {
      static char blk[24];
      sprintf(blk, "%ld\t", len);
      archive+=blk;
      archive+=(fname+base);
      archive+="\r\n";
      s+=fname;
      s+="\n";
      ++result;
    }
    fclose(f);
  }
  return result;
}

#ifdef WINDOWS

int expand(String& archive, String& s, const char* fname, int base) {
  int result=0;
  DWORD attr=GetFileAttributes(fname);
  if ((attr != 0xFFFFFFFF) && (attr & FILE_ATTRIBUTE_DIRECTORY)) {
    WIN32_FIND_DATA ffd;
    String fdir(fname);
    fdir+="/*";
    HANDLE h=FindFirstFile(fdir.c_str(), &ffd);
    while (h!=INVALID_HANDLE_VALUE) {
      if (!equals(ffd.cFileName, ".") && !equals(ffd.cFileName, "..")) {
        String d(fname);
        d+="/";
        d+=ffd.cFileName;
        result+=expand(archive, s, d.c_str(), base);
      }
      if (FindNextFile(h, &ffd)!=TRUE) break;
    }
    FindClose(h);
  }
  else // ordinary file
    result=putsize(archive, s, fname, base);
  return result;
}

#else
#ifdef UNIX

int expand(String& archive, String& s, const char* fname, int base) {
  int result=0;
  struct stat sb;
  if (stat(fname, &sb)<0) return 0;

  // If a regular file and readable, get file size
  if (sb.st_mode & S_IFREG && sb.st_mode & 0400)
    result+=putsize(archive, s, fname, base);

  // If a directory with read and execute permission, traverse it
  else if (sb.st_mode & S_IFDIR && sb.st_mode & 0400 && sb.st_mode & 0100) {
    DIR *dirp=opendir(fname);
    if (!dirp) {
      perror("opendir");
      return result;
    }
    dirent *dp;
    while(errno=0, (dp=readdir(dirp))!=0) {
      if (!equals(dp->d_name, ".") && !equals(dp->d_name, "..")) {
        String d(fname);
        d+="/";
        d+=dp->d_name;
        result+=expand(archive, s, d.c_str(), base);
      }
    }
    if (errno) perror("readdir");
    closedir(dirp);
  }
  else printf("%s is not a readable file or directory\n", fname);
  return result;
}

#else  // Not WINDOWS or UNIX, ignore directories

int expand(String& archive, String& s, const char* fname, int base) {
  return putsize(archive, s, fname, base);
}  

#endif
#endif


// To compress to file1.paq8p: paq8p [-n] file1 [file2...]
// To decompress: paq8p file1.paq8p [output_dir]
int paqmain(int argc, char** argv) {
  // LLVM hack: Use the executable name as a suffix to allow nat and llc tests
  // to run in parallel.
  const char *suffix = mybasename(argv[0]);

  bool pause=argc<=2;  // Pause when done?
  try {

    // Get option
    bool doExtract=false;  // -d option
    if (argc>1 && argv[1][0]=='-' && argv[1][1] && !argv[1][2]) {
      if (argv[1][1]>='0' && argv[1][1]<='9')
        level=argv[1][1]-'0';
      else if (argv[1][1]=='d')
        doExtract=true;
      else
        quit("Valid options are -0 through -9 or -d\n");
      --argc;
      ++argv;
      pause=false;
    }

    // Print help message
    if (argc<2) {
      printf(PROGNAME " archiver (C) 2008, Matt Mahoney et al.\n"
        "Free under GPL, http://www.gnu.org/licenses/gpl.txt\n\n"
#ifdef WINDOWS
        "To compress or extract, drop a file or folder on the "
        PROGNAME " icon.\n"
        "The output will be put in the same folder as the input.\n"
        "\n"
        "Or from a command window: "
#endif
        "To compress:\n"
        "  " PROGNAME " -level file               (compresses to file." PROGNAME ")\n"
        "  " PROGNAME " -level archive files...   (creates archive." PROGNAME ")\n"
        "  " PROGNAME " file                      (level -%d, pause when done)\n"
        "level: -0 = store, -1 -2 -3 = faster (uses 35, 48, 59 MB)\n"
        "-4 -5 -6 -7 -8 = smaller (uses 133, 233, 435, 837, 1643 MB)\n"
#if defined(WINDOWS) || defined (UNIX)
        "You may also compress directories.\n"
#endif
        "\n"
        "To extract or compare:\n"
        "  " PROGNAME " -d dir1/archive." PROGNAME "      (extract to dir1)\n"
        "  " PROGNAME " -d dir1/archive." PROGNAME " dir2 (extract to dir2)\n"
        "  " PROGNAME " archive." PROGNAME "              (extract, pause when done)\n"
        "\n"
        "To view contents: more < archive." PROGNAME "\n"
        "\n",
        DEFAULT_OPTION);
      quit();
    }

    FILE* archive=0;  // compressed file
    int files=0;  // number of files to compress/decompress
    Array<char*> fname(1);  // file names (resized to files)
    Array<long> fsize(1);   // file lengths (resized to files)

    // Compress or decompress?  Get archive name
    Mode mode=COMPRESS;
    String archiveName(mybasename(argv[1]));
    {
      const int prognamesize=strlen(suffix);
      const int arg1size=strlen(argv[1]);
      if (arg1size>prognamesize+1 && argv[1][arg1size-prognamesize-1]=='.'
          && equals(suffix, argv[1]+arg1size-prognamesize)) {
        mode=DECOMPRESS;
      }
      else if (doExtract)
        mode=DECOMPRESS;
      else {
        archiveName+=".";
        archiveName+=suffix;
      }
    }
   
    // Compress: write archive header, get file names and sizes
    String filenames;
    if (mode==COMPRESS) {

      // Expand filenames to read later.  Write their base names and sizes
      // to archive.
      String header_string;
      int i;
      for ( i=1; i<argc; ++i) {
        String name(argv[i]);
        int len=name.size()-1;
        for (int j=0; j<=len; ++j)  // change \ to /
          if (name[j]=='\\') name[j]='/';
        while (len>0 && name[len-1]=='/')  // remove trailing /
          name[--len]=0;
        int base=len-1;
        while (base>=0 && name[base]!='/') --base;  // find last /
        ++base;
        if (base==0 && len>=2 && name[1]==':') base=2;  // chop "C:"
        int expanded=expand(header_string, filenames, name.c_str(), base);
        if (!expanded && (i>1||argc==2))
          printf("%s: not found, skipping...\n", name.c_str());
        files+=expanded;
      }

      // If archive doesn't exist and there is at least one file to compress
      // then create the archive header.
      if (files<1) quit("Nothing to compress\n");
//      archive=fopen(archiveName.c_str(), "rb");
//      if (archive)
//        printf("%s already exists\n", archiveName.c_str()), quit();
      archive=fopen(archiveName.c_str(), "wb+");
      if (!archive) perror(archiveName.c_str()), quit();
      fprintf(archive, PROGNAME " -%d\r\n%s\x1A",
        level, header_string.c_str());
      printf("Creating archive with %d file(s)...\n", files);

      // Fill fname[files], fsize[files] with input filenames and sizes
      fname.resize(files);
      fsize.resize(files);
      char *p=&filenames[0];
      rewind(archive);
      getline(archive);
      for ( i=0; i<files; ++i) {
        const char *num=getline(archive);
        assert(num);
        fsize[i]=atol(num);
        assert(fsize[i]>=0);
        fname[i]=p;
        while (*p!='\n') ++p;
        assert(p-filenames.c_str()<filenames.size());
        *p++=0;
      }
      fseek(archive, 0, SEEK_END);
    }

    // Decompress: open archive for reading and store file names and sizes
    if (mode==DECOMPRESS) {
      archive=fopen(archiveName.c_str(), "rb+");
      if (!archive) perror(archiveName.c_str()), quit();

      // Check for proper format and get option
      const char* header=getline(archive);
      if (strncmp(header, PROGNAME " -", strlen(PROGNAME)+2))
        printf("%s: not a %s file\n", archiveName.c_str(), PROGNAME), quit();
      level=header[strlen(PROGNAME)+2]-'0';
      if (level<0||level>9) level=DEFAULT_OPTION;

      // Fill fname[files], fsize[files] with output file names and sizes
      while (getline(archive)) ++files;  // count files
      printf("Extracting %d file(s) from archive -%d\n", files, level);
      long header_size=ftell(archive);
      filenames.resize(header_size+4);  // copy of header
      rewind(archive);
      fread(&filenames[0], 1, header_size, archive);
      fname.resize(files);
      fsize.resize(files);
      char* p=&filenames[0];
      while (*p && *p!='\r') ++p;  // skip first line
      ++p;
      for (int i=0; i<files; ++i) {
        fsize[i]=atol(p+1);
        while (*p && *p!='\t') ++p;
        fname[i]=p+1;
        while (*p && *p!='\r') ++p;
        if (!*p) printf("%s: header corrupted at %ld\n", archiveName.c_str(),
          p-&filenames[0]), quit();
        assert(p-&filenames[0]<header_size);
        *p++=0;
      }
    }
        
    // Set globals according to option
    assert(level>=0 && level<=9);
    buf.setsize(MEM*8);

    // Compress or decompress files
    assert(fname.size()==files);
    assert(fsize.size()==files);
    long total_size=0;  // sum of file sizes
    for (int i=0; i<files; ++i) total_size+=fsize[i];
    Encoder en(mode, archive);
    if (mode==COMPRESS) {
      for (int i=0; i<files; ++i)
        compress(fname[i], fsize[i], en);
      en.flush();
      printf("%ld -> %ld\n", total_size, en.size());
    }

    // Decompress files to dir2: paq8p -d dir1/archive.paq8p dir2
    // If there is no dir2, then extract to dir1
    // If there is no dir1, then extract to .
    else {
      assert(argc>=2);
      String dir(argc>2?argv[2]:argv[1]);
      if (argc==2) {  // chop "/archive.paq8p"
        int i;
        for (i=dir.size()-2; i>=0; --i) {
          if (dir[i]=='/' || dir[i]=='\\') {
            dir[i]=0;
            break;
          }
          if (i==1 && dir[i]==':') {  // leave "C:"
            dir[i+1]=0;
            break;
          }
        }
        if (i==-1) dir=".";  // "/" not found
      }
      dir=dir.c_str();
      if (dir[0] && (dir.size()!=3 || dir[1]!=':')) dir+="/";
      for (int i=0; i<files; ++i) {
        String out(dir.c_str());
        out+=fname[i];
        decompress(out.c_str(), fsize[i], en);
      }
    }
    fclose(archive);
    programChecker.print();
  }
  catch(const char* s) {
    if (s) printf("%s\n", s);
  }
  if (pause) {
    printf("\nClose this window or press ENTER to continue...\n");
    getchar();
  }
  return 0;
}

int main(int argc, char **argv)
{
#ifndef LLVM
  return paqmain(argc, argv);
#else
  int rc = 1;
  /* there is lot of static data, need a clean state for decompress to work
   * properly, so fork */
  pid_t pid = fork();
  if (pid == 0) {
    /* compress files */
    exit(paqmain(argc, argv));
  } else if (pid == -1) {
    perror("fork() failed");
    exit(1);
  }
  wait(&rc);
  if (rc)
    return rc;
  /* now decompress and verify */
  char *deargv[3];
  deargv[0] = argv[0];
  deargv[1] = strdup("-d");

  argc--;
  argv++;
  while(argc && argv[0][0] == '-') {argc--; argv++;}  
  String archiveName(argv[0]);
  archiveName += ".";
  archiveName += mybasename(deargv[0]);
  deargv[2] = strdup(archiveName.c_str());
  if (paqmain(3, deargv))
    return 1;
  free(deargv[1]);
  unlink(deargv[2]);
  free(deargv[2]);
  return 0;
#endif
}
