#!/bin/bash

DEBUG=-DDEBUG=1
OPT=-O0
SYMBOLS=-g
EXECUTABLE=altered

WIGNORE="-Wno-nested-anon-types -Wno-variadic-macros -Wno-old-style-cast -Wno-sign-conversion -Wno-cast-align -Wno-gnu-anonymous-struct -Wno-c++98-compat -Wno-padded -Wno-double-promotion -Wno-c++98-compat-pedantic -Wno-missing-prototypes -Wno-float-equal -Wno-unused-parameter -Wno-gnu-zero-variadic-macro-arguments -Wno-writable-strings -Wno-global-constructors -Wno-switch-enum -Wno-char-subscripts -Wno-unused-function -Wno-zero-as-null-pointer-constant -Wno-format-nonliteral -Wno-reserved-id-macro -Wno-format-security -Wno-missing-variable-declarations -Wno-cast-qual -Wno-int-to-void-pointer-cast"

CommonCompilerFlags="-pthread -isystem /usr/include -isystem /usr/include -isystem ../libs  -isystem ../libs/glfw/include  -isystem ../libs/glad/include -isystem ../libs/fmod/include "

WIGNORE="-Wno-nested-anon-types -Wno-variadic-macros -Wno-old-style-cast -Wno-sign-conversion -Wno-cast-align -Wno-gnu-anonymous-struct -Wno-c++98-compat -Wno-padded -Wno-double-promotion -Wno-c++98-compat-pedantic -Wno-missing-prototypes -Wno-float-equal -Wno-unused-parameter -Wno-gnu-zero-variadic-macro-arguments -Wno-writable-strings -Wno-global-constructors -Wno-switch-enum -Wno-char-subscripts -Wno-unused-function -Wno-zero-as-null-pointer-constant -Wno-format-nonliteral -Wno-reserved-id-macro -Wno-format-security -Wno-missing-variable-declarations -Wno-cast-qual -Wno-int-to-void-pointer-cast"

CommonLinkerFlags="-L../libs/glad -L/usr/local/lib -L../libs/glfw/libglfw3.a -L../libs/glfw -L../libs/fmod/lib -ldl -lm -lpng -lz -lfmod -lglad -lglfw -latomic -o $EXECUTABLE -Wl,-rpath,\$ORIGIN/../build"

pushd build

clang --std=c++17 -stdlib=libstdc++ -Weverything $WIGNORE -msse2  $DEBUG $CommonCompilerFlags $SYMBOLS $OPT ../src/main.cpp $CommonLinkerFlags -fno-exceptions

popd
