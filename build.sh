#!/bin/bash

set -xe

FRAMEWORK_FLAGS="-framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL"
CFLAGS="-Wall -Wextra -pedantic -std=c11 -ggdb"
clang src/three-body.c deps/libraylib.a -o build/three-body $CFLAGS $FRAMEWORK_FLAGS -Ideps -Isrc
