#!/bin/zsh
set -e

PRESET="${1-debug}"

cmake --build "build/$PRESET"
./build/$PRESET/LightDAW.app/Contents/MacOS/LightDAW
