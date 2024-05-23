#!/bin/bash

#set -xe
if [ "$(uname)" == "Linux" ]; then 
    export LD_LIBRARY_PATH=deps/linux
    CC="gcc"
    EXTRA_FLAGS="-lraylib -lGL -lm -lpthread -ldl -lrt -lX11"
    INCLUDE="-I./deps/linux"
    DEPS="-L./deps/linux"
    echo $LD_LIBRARY_PATH
else
    CC="clang"
    EXTRA_FLAGS="-framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL"
    INCLUDE="-Ideps/macos"
    DEPS="deps/macos/libraylib.a"
fi
CFLAGS="-Wall -Wextra -pedantic -std=c11 -ggdb"
$CC src/three-body.c $DEPS -o build/three-body $CFLAGS $EXTRA_FLAGS $INCLUDE -Isrc
