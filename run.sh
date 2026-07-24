#!/bin/bash

cmake -S . -B build -GNinja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON && cmake --build build -j16 && ./build/tes
