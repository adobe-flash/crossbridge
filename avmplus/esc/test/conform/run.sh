# Usage:
#   run.sh [ testname ... ]
# where 
#   "testname" is full path to the .es file to run
#
# All tests are run if none are selected specifically.

function check {
  ( cd ../../build
    ./esc.sh ../test/conform/$1 )
  rm -f $1.log
  ../../bin/shell $1.abc > $1.log
  dos2unix $1.log
  diff -s $1.key $1.log
}

./clean.sh

if [ "$1" != "" ]; then
  while [ "$1" != "" ]; do
    case $1 in
    "piece") special/piece.sh ;;
    *)       check $1 ;;
    esac
    shift
  done
  exit
fi

# ALL TESTS

for i in stmt/*.es; do
  check $i
done

for i in expr/*.es; do
  check $i
done

for i in es4-expr/*.es; do
  check $i
done

for i in es4-class/*.es; do
  check $i
done

for i in misc/*.es; do
  check $i
done

special/piece.sh

