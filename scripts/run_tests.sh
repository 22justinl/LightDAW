#!/bin/zsh
set -e

PRESET="${1-debug}"
./build/$PRESET/tests
