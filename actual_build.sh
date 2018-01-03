#!/bin/bash

WIGNORE="-Wno-nested-anon-types -Wno-variadic-macros -Wno-old-style-cast -Wno-sign-conversion -Wno-cast-align -Wno-gnu-anonymous-struct -Wno-c++98-compat -Wno-padded -Wdocumentation -Wdocumentation-unknown-command -Wno-double-promotion -Wno-c++98-compat-pedantic -Wno-missing-prototypes -Wno-float-equal -Wno-unused-parameter  -Wno-gnu-zero-variadic-macro-arguments -Wno-writable-strings -Wno-global-constructors -Wno-switch-enum -Wno-char-subscripts -Wno-unused-function"

CommonCompilerFlags="-isystem ../libs -isystem ../libs/glfw/include  -isystem ../libs/FreeType/include -isystem ../libs/glad/include -isystem ../libs/fmod/include -g"
CommonLinkerFlags=" -L../libs/glad/osx -L/usr/local/lib -L../libs/glfw/osx/ -L../libs/glfw/osx -L../libs/fmod/lib/osx/ -ldl -lfreetype -lm -lpng -lz -lfmod -lglad -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo -o main -Wl,-rpath,\$ORIGIN/../build"

pushd build

clang --std=c++14 -Weverything $WIGNORE -DGLITCH_DEBUG=1 $CommonCompilerFlags -O0 ../src/main.cpp $CommonLinkerFlags

popd
