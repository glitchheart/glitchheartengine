#!/bin/bash

DEBUG=-DDEBUG=1
OPT=-O0
SYMBOLS=-g
EXECUTABLE=altered

pushd "`dirname "$0"`"

ARCH=`uname -m`

if [ "$ARCH" == "x86_64" ]; then
    FMOD="libs/fmod/x86_64"
    GLAD="libs/glad/x86_64"
    GLFW="libs/glfw/x86_64"
    BUILD_DIR=x86_64
else
    FMOD="libs/fmod/x86"
    GLAD="libs/glad/x86"
    GLFW="libs/glfw/x86"
    BUILD_DIR=x86
fi

WIGNORE="-Wno-nested-anon-types -Wno-variadic-macros -Wno-old-style-cast -Wno-sign-conversion -Wno-conversion -Wno-c++11-compat-deprecated-writable-strings -Wno-cast-align -Wno-gnu-anonymous-struct -Wno-c++98-compat -Wno-padded -Wno-double-promotion -Wno-c++98-compat-pedantic -Wno-missing-prototypes -Wno-float-equal -Wno-unused-parameter -Wno-gnu-zero-variadic-macro-arguments -Wno-writable-strings -Wno-global-constructors -Wno-switch-enum -Wno-char-subscripts -Wno-unused-function -Wno-zero-as-null-pointer-constant -Wno-format-nonliteral -Wno-reserved-id-macro -Wno-format-security -Wno-missing-variable-declarations -Wno-cast-qual -Wno-int-to-void-pointer-cast -Wno-shift-sign-overflow -Wno-shorten-64-to-32"

CommonCompilerFlags="-pthread -isystem /usr/include -isystem /usr/include -isystem ../libs  -isystem ../libs/glfw/include  -isystem ../libs/glad/include -isystem ../libs/fmod/include `pkg-config --cflags glfw3`"

CommonLinkerFlags="-L../$GLAD -L/usr/local/lib `pkg-config --static --libs glfw3` -L../$FMOD -lglfw3 -ldl -lm -lfmod -lglad  -latomic -o $EXECUTABLE -Wl,-rpath,\$ORIGIN/../$BUILD_DIR"

mkdir -p $BUILD_DIR

cp $FMOD/libfmod.so $BUILD_DIR
cp $FMOD/libfmod.so.10 $BUILD_DIR
cp $FMOD/libfmod.so.10.12 $BUILD_DIR
cp $GLAD/libglad.so $BUILD_DIR

pushd $BUILD_DIR

clang --std=c++11 -stdlib=libstdc++ -Weverything $WIGNORE -msse2  $DEBUG $CommonCompilerFlags $SYMBOLS $OPT ../src/main.cpp $CommonLinkerFlags -fno-exceptions

popd
popd
