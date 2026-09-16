#!/bin/zsh
set -e

PRESET="${1-debug}"
open "build/$PRESET/LightDAW.app"
