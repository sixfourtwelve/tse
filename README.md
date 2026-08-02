# TSE

Small C++ game-engine skeleton built on OGRE-Next 3.0 and SDL2.

OGRE-Next selects the native renderer at build time:

| Platform | Renderer |
| --- | --- |
| macOS | Metal |
| Windows | Direct3D 11 |
| Linux | OpenGL 3+ |

SDL2 owns the window and input. OGRE-Next owns rendering, resources, the scene graph, and the compositor. ImGui is intentionally not part of this first migration.

## Build

Install vcpkg, set `VCPKG_ROOT`, then configure with its toolchain:

```sh
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
cmake --build build -j
./build/tse
```

The first configure also downloads the OGRE-Next 3.0.0 shader templates that its vcpkg package does not install.

## Controls

- `W`, `A`, `S`, `D`: move
- Mouse: look
- `Space` / left `Ctrl`: move up / down
- `Shift`: move faster
- `Esc`: quit

