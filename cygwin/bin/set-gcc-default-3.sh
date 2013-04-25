#!/bin/bash

# We only switch the alternatives that exist for both compilers;
# gdc, gpc, g77 and gfortran never get switched.

for x in gcc g++ gcj gnat ; do
  /usr/sbin/alternatives --set $x /usr/bin/$x-3.exe
done
