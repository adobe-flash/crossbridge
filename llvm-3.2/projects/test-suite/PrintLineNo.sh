#!/bin/sh

dwarfdump Output/$1.dbg.dSYM/ -e --debug-line | grep "^0x" | awk '{if ($3) prev=$3; print basename prev," ",$2}' |sort |uniq > Output/$1.dbg.ml; 
dwarfdump Output/$1.dbg.opt.dSYM/ -e --debug-line | grep "^0x" | awk '{if ($3) prev=$3; print basename prev," ",$2}' |sort |uniq > Output/$1.dbg.opt.ml; 
dwarfdump Output/$1.native.dbg.dSYM/ -e --debug-line | grep "^0x" | awk '{if ($3) prev=$3; print basename prev," ",$2}' |sort |uniq > Output/$1.native.dbg.ml; 
dwarfdump Output/$1.native.dbg.opt.dSYM/ -e --debug-line | grep "^0x" | awk '{if ($3) prev=$3; print basename prev," ",$2}' |sort |uniq > Output/$1.native.dbg.opt.ml;
