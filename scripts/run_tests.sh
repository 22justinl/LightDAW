#!/bin/zsh
set -e

PRESET="${1-debug}"

cmake --build "build/$PRESET"
./build/$PRESET/unit_tests
