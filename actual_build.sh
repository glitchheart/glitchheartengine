#!/bin/bash

WIGNORE="-Wno-nested-anon-types -Wno-variadic-macros -Wno-old-style-cast -Wno-sign-conversion -Wno-reserved-id-macro -Wno-disabled-macro-expansion -Wno-cast-align -Wno-gnu-anonymous-struct -Wno-unused-value -Wno-c++98-compat -Wno-double-precision -Wno-padded -Wno-implicit-fallthrough -Wno-double-promotion -Wno-c++98-compat-pedantic -Wno-conversion -Wno-newline-eof"

CommonCompilerFlags="-I ../libs ../libs/glfw/include ../libs/openal/include -I ../libs/freetype/include ../libs/glad/include"
CommonLinkerFlags="-L../libs/glad/glad.obj -libfreetype -libglfw3"

pushd build

clang --std=c++14 -Weverything $WIGNORE -DGLITCH_DEBUG=1 $CommonCompilerFlags ../src/linux_main.cpp $CommonLinkerFlags

popd
