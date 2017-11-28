#!/bin/bash

WIGNORE="-Wno-nested-anon-types -Wno-variadic-macros -Wno-old-style-cast -Wno-sign-conversion -Wno-reserved-id-macro -Wno-disabled-macro-expansion -Wno-cast-align -Wno-gnu-anonymous-struct -Wno-unused-value -Wno-c++98-compat -Wno-padded -Wno-implicit-fallthrough -Wno-double-promotion -Wno-c++98-compat-pedantic -Wno-conversion -Wno-newline-eof -Wno-missing-prototypes -Wno-float-equal -Wno-unused-parameter -Wno-documentation -Wno-documentation-unknown-command -Wno-undef -Wno-gnu-zero-variadic-macro-arguments -Wno-writable-strings -Wno-missing-variable-declarations -Wno-global-constructors -Wno-missing-noreturn -Wno-format-security -Wno-char-subscripts"

WIGNORETEMP="-Wno-shadow -Wno-deprecated -Wno-switch-enum -Wno-switch "
W="-Wno-null-dereference"

CommonCompilerFlags="-I ../libs -I ../libs/glfw/include -I ../libs/openal/include -I ../libs/FreeType/include -I ../libs/glad/include -I ../libs/fmod/include -g"
CommonLinkerFlags=" -L../libs/glad -L/usr/local/lib -L../libs/glfw/libglfw3.a -L../libs/glfw -L../libs/fmod/lib -ldl -lfreetype -lm -lpng -lz -lfmod -lglad -lglfw -o main -Wl,-rpath,\$ORIGIN/../build"

pushd build

clang --std=c++14 -Weverything $WIGNORE -DGLITCH_DEBUG=1 $CommonCompilerFlags -O0 ../src/linux_main.cpp $CommonLinkerFlags

popd
