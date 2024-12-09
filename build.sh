#!/usr/bin/env bash

# Detect the operating system
OS=$(uname)

# Common flags
SRC="main.c"
OUTPUT="sorta"
INCLUDE="-Iraylib/src"
CFLAGS="-Wall -Wextra"

if [ "$OS" = "Darwin" ]; then
    # macOS-specific options
    LIB_PATH="-Lraylib/build/raylib"
    LIBS="-lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo"
elif [ "$OS" = "Linux" ]; then
    # Linux-specific options
    LIB_PATH="-Lraylib/build/raylib"
    LIBS="-lraylib -lm"
else
    echo "Unsupported OS: $OS"
    exit 1
fi

# Compile
clang -o $OUTPUT $SRC $INCLUDE $LIB_PATH $LIBS $CFLAGS

# Update the etags files
# etags --language=c *.c  raylib/src/*.c raylib/src/*.h


