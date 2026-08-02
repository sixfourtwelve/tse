# TSE

Small C++ game-engine skeleton built on OGRE-Next 3.0 and SDL2.

OGRE-Next selects the native renderer at build time:

| Platform | Renderer |
| --- | --- |
| macOS | Metal |
| Windows | Direct3D 11 |
| Linux | OpenGL 3+ |

SDL2 owns the window and input. OGRE-Next owns rendering, resources, the scene graph, and the compositor. ImGui is intentionally not part of this first migration.

The default workspace uses a three-split PSSM shadow map for its directional
light. Models and procedural geometry cast and receive shadows through HLMS PBS.

## Build

Install vcpkg, set `VCPKG_ROOT`, then configure with its toolchain:

```sh
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
cmake --build build -j
./build/tse
```

The first configure also downloads the OGRE-Next 3.0.0 shader templates that its vcpkg package does not install.

## Models: glTF in, OGRE-Next mesh out

Keep editable/source models in `assets/source` as glTF 2.0 (`.gltf` or `.glb`).
The game loads cooked OGRE-Next v2 meshes from `assets/models`; it does not parse
glTF during play.

The cooker uses Blender as the glTF reader and
[blender2ogre](https://github.com/OGRECave/blender2ogre) as the exporter. This is
the path that also produces OGRE-Next HLMS material JSON instead of old OGRE 1.x
material scripts. It needs the `OgreMeshTool` executable from the matching
OGRE-Next 3.0.0 source tree. The vcpkg `ogre-next` port does not install that
tool, so build the `OgreMeshTool` target once from the same v3.0.0 tag.

For fish, point the cooker at those three tools and cook a model:

```fish
set -gx BLENDER /Applications/Blender.app/Contents/MacOS/Blender
set -gx BLENDER2OGRE_PATH /path/to/blender2ogre
set -gx OGRE_MESH_TOOL /path/to/ogre-next-build/bin/OgreMeshTool
python3 tools/cook_model.py assets/source/robot.glb
```

The result is `assets/models/robot.mesh` plus its material JSON and textures.
Instantiate it from game code with:

```cpp
#include "model.hpp"

spawn<Model>(Ogre::Vector3::ZERO, "robot.mesh");
```

This first cooker intentionally flattens the glTF scene into one static model
while preserving material slots and baking node transforms. Skins, animations,
morph targets, cameras, and lights are not cooked yet. Keep the original glTF/
GLB files; `assets/models` is generated game data.

## Controls

- `W`, `A`, `S`, `D`: move
- Mouse: look
- `Space` / left `Ctrl`: move up / down
- `Shift`: move faster
- `Esc`: quit
