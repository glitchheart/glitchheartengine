#!/bin/bash

pushd $GLITCH_ENGINE
chmod 755 osx_actual_build.sh
time ./osx_actual_build.sh
popd

