This is version 2.04 of agrep - a new tool for fast 
text searching allowing errors.
agrep is similar to egrep (or grep or fgrep), but it is much more general
(and usually faster).
The main changes from version 1.1 are 1) incorporating Boyer-Moore
type filtering to speed up search considerably, 2) allowing multi patterns 
via the -f option; this is similar to fgrep, but from our experience 
agrep is much faster, 3) searching for "best match" without having to
specify the number of errors allowed, and 4) ascii is no longer required.
Several more options were added.

To compile, simply run make in the agrep directory after untar'ing
the tar file (tar -xf agrep-2.04.tar will do it).

The three most significant features of agrep that are not supported by
the grep family are 
1) the ability to search for approximate patterns;
    for example, "agrep -2 homogenos foo" will find homogeneous as well 
    as any other word that can be obtained from homogenos with at most 
    2 substitutions, insertions, or deletions.
    "agrep -B homogenos foo" will generate a message of the form
    best match has 2 errors, there are 5 matches, output them? (y/n)
2) agrep is record oriented rather than just line oriented;  a record
    is by default a line, but it can be user defined;
    for example, "agrep -d '^From ' 'pizza' mbox"
    outputs all mail messages that contain the keyword "pizza".
    Another example:  "agrep -d '$$' pattern foo" will output all
    paragraphs (separated by an empty line) that contain pattern.
3) multiple patterns with AND (or OR) logic queries.   
    For example, "agrep -d '^From ' 'burger,pizza' mbox" 
    outputs all mail messages containing at least one of the 
    two keywords (, stands for OR).
    "agrep -d '^From ' 'good;pizza' mbox" outputs all mail messages
    containing both keywords.

Putting these options together one can ask queries like

agrep -d '$$' -2 '<CACM>;TheAuthor;Curriculum;<198[5-9]>' bib

which outputs all paragraphs referencing articles in CACM between 
1985 and 1989 by TheAuthor dealing with curriculum.  
Two errors are allowed, but they cannot be in either CACM or the year 
(the <> brackets forbid errors in the pattern between them).  

Other features include searching for regular expressions (with or
without errors), unlimited wild cards, limiting the errors to only 
insertions or only substitutions or any combination, 
allowing each deletion, for example, to be counted as, say, 
2 substitutions or 3 insertions, restricting parts of the query 
to be exact and parts to be approximate, and many more.

agrep is available by anonymous ftp from cs.arizona.edu (IP 192.12.69.5)
as agrep/agrep-2.04.tar.Z (or in uncompressed form as agrep/agrep-2.04.tar).
The tar file contains the source code (in C), man pages (agrep.1),
and two additional files, agrep.algorithms and agrep.chronicle,
giving more information.
The agrep directory also includes two postscript files: 
agrep.ps.1 is a technical report from June 1991 
describing the design and implementation of agrep;
agrep.ps.2 is a copy of the paper as appeared in the 1992
Winter USENIX conference.

Please mail bug reports (or any other comments) 
to sw@cs.arizona.edu or to udi@cs.arizona.edu.

We would appreciate if users notify us (at the address above)
of any extensions, improvements, or interesting uses of this software.

January 17, 1992


BUGS_fixed/option_update

1. remove multiple definitions of some global variables.
2. fix a bug in -G option.
3. fix a bug in -w option.
January 23, 1992

4. fix a bug in pipeline input.
5. make the definition of word-delimiter consistant.
March 16, 1992

6. add option '-y' which, if specified with -B option, will always
output the best-matches without a prompt.
April 10, 1992

7. fix a bug regarding exit status.
April 15, 1992
