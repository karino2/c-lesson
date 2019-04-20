#!/bin/sh

FILE_BASE=`basename $1 .c`

clang -m32 -emit-llvm $1 -c -o "$FILE_BASE.bc"
llc -march=arm "$FILE_BASE.bc" -o "$FILE_BASE.s"