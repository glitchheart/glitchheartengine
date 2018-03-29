#!/bin/bash

WIGNORE="-Wno-nested-anon-types -Wno-variadic-macros -Wno-old-style-cast -Wno-sign-conversion -Wno-cast-align -Wno-gnu-anonymous-struct -Wno-c++98-compat -Wno-padded -Wdocumentation -Wdocumentation-unknown-command -Wno-double-promotion -Wno-c++98-compat-pedantic -Wno-missing-prototypes -Wno-float-equal -Wno-unused-parameter  -Wno-gnu-zero-variadic-macro-arguments -Wno-writable-strings -Wno-global-constructors -Wno-switch-enum -Wno-char-subscripts -Wno-unused-function"

CommonCompilerFlags="-isystem ../libs -isystem ../libs/glfw/include  -isystem ../libs/FreeType/include -isystem ../libs/glad/include -isystem ../libs/fmod/include -g"
CommonLinkerFlags=" -L../libs/glad/osx/ -L/usr/local/lib -L../libs/glfw/osx/ -L../libs/fmod/lib/osx/ -ldl -lfreetype -lm -lpng -lz -lfmod -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo -F$VULKAN_SDK/Frameworks -framework Vulkan ../libs/glad/osx/glad.o -rpath @executable_path -o main"

pushd build


clang-tidy -checks='-*,readability-identifier-naming' \
-config="{CheckOptions: [ {key: readability-identifier-naming.StructCase, value: CamelCase}, \
{key: readability-identifier-naming.FunctionCase, value: lower_case}, \
{key: readability-identifier-naming.LocalVariableCase, value: lower_case}]}" \
-header-filter=.* ../src/main.cpp -- -std=c++14 -fno-delayed-template-parsing -isystem ../libs -isystem ../libs/glad/include -isystem ../libs/fmod/include -isystem $VULKAN_SDK/include -isystem ../libs/FreeType/include -DGLITCH_DEBUG=1

popd
