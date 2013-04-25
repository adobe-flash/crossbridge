function run {
cd ../build
../build/esc1.sh ../test/sanity/tests/$1
../build/esc2.sh ../test/sanity/tests/$1
../build/esc3.sh ../test/sanity/tests/$1
../build/esc.sh ../test/sanity/tests/$1
../bin/shell ../test/sanity/tests/$1.abc > ../test/sanity/tests/$1.log
cd ../test
}


rm sanity/tests/*.es.ast
rm sanity/tests/*.es.asm
rm sanity/tests/*.es.abc
rm sanity/tests/*.es.log
rm sanity/tests/*.es.abc

rm sanity/log/*.es.ast
rm sanity/log/*.es.asm
rm sanity/log/*.es.abc
rm sanity/log/*.es.log
rm sanity/log/*.es

run arrayliteral.es
run cls.es
run continue.es
run expr.es
run fib.es
run for.es
run hello.es
run if.es
run numbers.es
run objectliteral.es
run prime.es
run print.es
run switch.es
run switchType.es
run try.es
run vars.es
run while.es
run logic.es
run destruct.es

cp sanity/tests/*.es.ast sanity/log
cp sanity/tests/*.es.asm sanity/log
cp sanity/tests/*.es.log sanity/log
cp sanity/tests/*.es sanity/log

dos2unix sanity/log/*.log

diff -s sanity/base sanity/log
