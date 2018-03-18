#!/bin/bash

WIGNORE="-Wno-nested-anon-types -Wno-variadic-macros -Wno-old-style-cast -Wno-sign-conversion -Wno-cast-align -Wno-gnu-anonymous-struct -Wno-c++98-compat -Wno-padded -Wno-double-promotion -Wno-c++98-compat-pedantic -Wno-missing-prototypes -Wno-float-equal -Wno-unused-parameter  -Wno-gnu-zero-variadic-macro-arguments -Wno-writable-strings -Wno-global-constructors -Wno-switch-enum -Wno-char-subscripts -Wno-unused-function -Wno-zero-as-null-pointer-constant"

CommonCompilerFlags="-isystem ../libs -isystem $VULKAN_SDK/include -isystem ../libs/glfw/include  -isystem ../libs/FreeType/include -isystem ../libs/glad/include -isystem ../libs/fmod/include -isystem  -g"
CommonLinkerFlags=" -L../libs/glad -L/usr/local/lib -L../libs/glfw/libglfw3.a -L../libs/glfw -L../libs/fmod/lib -L $VULKAN_SDK/lib -lvulkan -ldl -lfreetype -lm -lpng -lz -lfmod -lglad -lglfw -o main -Wl,-rpath,\$ORIGIN/../build"

pushd build

clang-tidy -checks='-*,readability-identifier-naming' -fix -fix-errors \
-config="{CheckOptions: [ {key: readability-identifier-naming.StructCase, value: CamelCase}, \
{key: readability-identifier-naming.FunctionCase, value: lower_case}, \
{key: readability-identifier-naming.LocalVariableCase, value: lower_case}, \
{key: readability-identifier-naming.EnumCase, value: CamelCase},\
{key: readability-identifier-naming.EnumConstantCase, value: UPPER_CASE},\
{key: readability-identifier-naming.MemberCase, value: lower_case},\
{key: readability-identifier-naming.PublicMemberCase, value: lower_case}, \
{key: readability-identifier-naming.UnionCase, value: CamelCase}, \
{key: readability-identifier-naming.GlobalConstantCase, value: lower_case}, \
{key: readability-identifier-naming.ConstantCase, value: lower_case}, \
{key: readability-identifier-naming.GlobalVariableCase, value: lower_case}, \
{key: readability-identifier-naming.VariableCase, value: lower_case}, \
{key: readability-identifier-naming.ConstantMemberCase, value: lower_case}, \
{key: readability-identifier-naming.GlobalFunctionCase, value: lower_case}, \
{key: readability-identifier-naming.LocalConstantCase, value: lower_case}, \
{key: readability-identifier-naming.MethodCase, value: lower_case}, \
{key: readability-identifier-naming.NamespaceCase, value: lower_case}, \
{key: readability-identifier-naming.ConstantParameterCase, value: lower_case}, \
{key: readability-identifier-naming.PureFunctionCase, value: lower_case}, \
{key: readability-identifier-naming.PureMethodCase, value: lower_case}, \
{key: readability-identifier-naming.StaticConstantCase, value: lower_case}, \
{key: readability-identifier-naming.StaticVariableCase, value: lower_case}, \
{key: readability-identifier-naming.ParameterCase, value: lower_case}]}" \
-header-filter=.* ../src/main.cpp -- -std=c++14 $WIGNORE -fno-delayed-template-parsing -isystem ../libs -isystem ../libs/glad/include -isystem ../libs/fmod/include -isystem $VULKAN_SDK/include -isystem ../libs/FreeType/include -DGLITCH_DEBUG=1

popd