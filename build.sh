#!/usr/bin/env bash

SRC="main.c"
OUT="sorta"
CFLAGS="-Iraylib/build/raylib/include -Lraylib/build/raylib -lraylib -lm"

# Build the executable
clang $SRC -o $OUT $CFLAGS

# Update the etags files
etags --language=c *.c  raylib/src/*.c raylib/src/*.h
