#!/bin/bash

WIGNORE="-Wno-nested-anon-types -Wno-variadic-macros -Wno-old-style-cast -Wno-sign-conversion -Wno-cast-align -Wno-gnu-anonymous-struct -Wno-c++98-compat -Wno-padded -Wno-double-promotion -Wno-c++98-compat-pedantic -Wno-missing-prototypes -Wno-float-equal -Wno-unused-parameter -Wno-gnu-zero-variadic-macro-arguments -Wno-writable-strings -Wno-global-constructors -Wno-switch-enum -Wno-char-subscripts -Wno-unused-function -Wno-zero-as-null-pointer-constant -Wno-format-nonliteral -Wno-reserved-id-macro -Wno-format-security -Wno-missing-variable-declarations -Wno-cast-qual -Wno-int-to-void-pointer-cast"

CommonCompilerFlags="-pthread -isystem ../libs  -isystem ../libs/glfw/include  -isystem ../libs/glad/include -isystem ../libs/fmod/include -isystem ../libs/gameanalytics/source/gameanalytics"
CommonLinkerFlags=" -L../libs/glad -L/usr/local/lib -L../libs/glfw/libglfw3.a -L../libs/glfw -L../libs/fmod/lib  -ldl -lm -lpng -lz -lfmod -lglad -lglfw -o main -Wl,-rpath,\$ORIGIN/../build  ../libs/gameanalytics/source/dependencies/curl/lib/linux_x64/libcurl.a ../libs/gameanalytics/source/dependencies/openssl/1.0.2h/libs/linux_x64/* ../libs/gameanalytics/linux-x64-clang-static/Release/libGameAnalytics.a"

pushd build

clang --std=c++14 -Weverything $WIGNORE -mssse3 -msse4.1 -mavx  -DDEBUG=1 $CommonCompilerFlags -g -O0 ../src/main.cpp $CommonLinkerFlags

popd
