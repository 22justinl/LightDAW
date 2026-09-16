#!/bin/zsh
set -e

PRESET="${1:-debug}"

cmake --preset "$PRESET"
cmake --build "build/$PRESET"
ln -sf "build/$PRESET/compile_commands.json" compile_commands.json
