# Must be run from the conform directory as special/piece.sh

rm -f special/piece*.es.abc
( cd ../../build
  ./esc.sh ../test/conform/special/piece1.es
  ./esc.sh ../test/conform/special/piece2.es )
rm -f special/piece.log
../../bin/shell special/piece1.es.abc special/piece2.es.abc > special/piece.log
dos2unix special/piece.log
diff -s special/piece.key special/piece.log
