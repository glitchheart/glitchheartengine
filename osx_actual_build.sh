#!/bin/bash

DEBUG=-DDEBUG=0
OPT=-O3

WIGNORE="-Wno-reserved-id-macro -Wno-deprecated -Wno-format-nonliteral -Wno-int-to-void-pointer-cast -Wno-undef -Wno-missing-variable-declarations -Wno-nested-anon-types -Wno-variadic-macros -Wno-old-style-cast -Wno-sign-conversion -Wno-cast-align -Wno-gnu-anonymous-struct -Wno-c++98-compat -Wno-padded -Wdocumentation -Wdocumentation-unknown-command -Wno-double-promotion -Wno-c++98-compat-pedantic -Wno-missing-prototypes -Wno-float-equal -Wno-unused-parameter  -Wno-gnu-zero-variadic-macro-arguments -Wno-writable-strings -Wno-global-constructors -Wno-switch-enum -Wno-char-subscripts -Wno-unused-function"

CommonCompilerFlags="-pthread -isystem ../libs -isystem ../libs/glfw/include -isystem ../libs/glad/include -isystem ../libs/fmod/include -isystem ../libs/libcurl/include -isystem ../libs/gameanalytics/source/gameanalytics"
CommonLinkerFlags="../libs/glad/osx/glad.o -L/usr/local/lib -L../libs/glfw/osx/ -L../libs/fmod/lib/osx/ ../libs/gameanalytics/source/dependencies/curl/lib/osx/libcurl.a ../libs/gameanalytics/source/dependencies/openssl/1.0.2h/libs/osx/* ../libs/gameanalytics/osx-static/Release/libGameAnalytics.a -ldl -lc++ -lm -lz -lfmod -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo -o main -rpath @executable_path"

pushd build

clang --std=c++17 -Weverything $WIGNORE $DEBUG $CommonCompilerFlags -g $OPT ../src/main.cpp $CommonLinkerFlags

popd
