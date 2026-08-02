#!/bin/bash

set -euo pipefail

cmake --preset default
cmake --build build -j
./build/tse
