#!/usr/bin/env python3
"""Cook one static glTF/GLB asset into an OGRE-Next v2 mesh."""

from __future__ import annotations

import os
from pathlib import Path
import shutil
import subprocess
import sys


def fail(message: str) -> "NoReturn":
    raise SystemExit(f"cook_model: {message}")


def find_program(environment_name: str, executable: str) -> str:
    configured = os.environ.get(environment_name)
    if configured:
        return configured
    found = shutil.which(executable)
    if found:
        return found
    fail(f"set {environment_name} to the {executable} executable")


def run_blender() -> None:
    import bpy  # type: ignore

    separator = sys.argv.index("--")
    source = Path(sys.argv[separator + 1]).resolve()
    output = Path(sys.argv[separator + 2]).resolve()
    mesh_tool = Path(sys.argv[separator + 3]).resolve()
    addon_path = sys.argv[separator + 4]

    if addon_path:
        addon = Path(addon_path).resolve()
        root = addon if (addon / "io_ogre").is_dir() else addon.parent
        sys.path.insert(0, str(root))
        import io_ogre  # type: ignore

        io_ogre.register()
    else:
        try:
            bpy.ops.preferences.addon_enable(module="io_ogre")
        except Exception as error:
            fail(
                "install/enable blender2ogre or set BLENDER2OGRE_PATH "
                f"({error})"
            )

    preferences = bpy.context.preferences.addons["io_ogre"].preferences
    preferences.OGRETOOLS_XML_CONVERTER = str(mesh_tool)

    bpy.ops.object.select_all(action="SELECT")
    bpy.ops.object.delete(use_global=False)
    bpy.ops.import_scene.gltf(filepath=str(source))

    meshes = [obj for obj in bpy.context.scene.objects if obj.type == "MESH"]
    if not meshes:
        fail(f"{source.name} contains no meshes")

    # This cooker deliberately flattens a glTF scene into one static model. Joining
    # keeps material slots while baking each node transform into the result.
    bpy.ops.object.select_all(action="DESELECT")
    for obj in meshes:
        world = obj.matrix_world.copy()
        obj.parent = None
        obj.matrix_world = world
        obj.select_set(True)
    bpy.context.view_layer.objects.active = meshes[0]
    bpy.ops.object.join()

    model = bpy.context.view_layer.objects.active
    model.name = source.stem
    model.data.name = source.stem
    output.mkdir(parents=True, exist_ok=True)

    result = bpy.ops.ogre.export(
        filepath=str(output / f"{source.stem}.scene"),
        EX_SCENE=False,
        EX_SELECTED_ONLY=True,
        EX_FORCE_CAMERA=False,
        EX_FORCE_LIGHTS=False,
        EX_NODE_ANIMATION=False,
        EX_V2_MESH_TOOL_VERSION="v2",
        EX_GENERATE_TANGENTS="4",
        EX_V2_OPTIMISE_VERTEX_BUFFERS=True,
        EX_V2_OPTIMISE_VERTEX_BUFFERS_OPTIONS="puqs",
    )
    if "FINISHED" not in result:
        fail(f"blender2ogre export failed: {result}")

    cooked = output / f"{source.stem}.mesh"
    if not cooked.is_file():
        fail(f"OgreMeshTool did not create {cooked}")
    print(cooked)


def main() -> None:
    if "bpy" in sys.modules:
        run_blender()
        return

    if len(sys.argv) not in (2, 3):
        fail("usage: tools/cook_model.py SOURCE.gltf|SOURCE.glb [OUTPUT_DIR]")

    source = Path(sys.argv[1]).resolve()
    if source.suffix.lower() not in (".gltf", ".glb") or not source.is_file():
        fail(f"not a glTF/GLB file: {source}")

    output = (
        Path(sys.argv[2]).resolve()
        if len(sys.argv) == 3
        else Path(__file__).resolve().parents[1] / "assets" / "models"
    )
    blender = find_program("BLENDER", "blender")
    mesh_tool = find_program("OGRE_MESH_TOOL", "OgreMeshTool")
    addon = os.environ.get("BLENDER2OGRE_PATH", "")

    command = [
        blender,
        "--background",
        "--factory-startup",
        "--python",
        str(Path(__file__).resolve()),
        "--",
        str(source),
        str(output),
        mesh_tool,
        addon,
    ]
    raise SystemExit(subprocess.run(command, check=False).returncode)


if __name__ == "__main__":
    main()
