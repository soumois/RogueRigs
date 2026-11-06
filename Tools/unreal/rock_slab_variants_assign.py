"""Import the _02 intact rock slab pieces (task-480-03) and list them on BP_ArenaGrid.

Unreal Editor Python, headless, editor closed:

    UnrealEditor-Cmd.exe <RogueRigs.uproject>
        -ExecutePythonScript="<abs path>/Tools/unreal/rock_slab_variants_assign.py"
        -unattended -nopause -nosplash -stdout

Idempotent. For Edge, Outer, Inner, Diagonal: imports Art/Meshes/rock-slab/SM_RockSlab_<Shape>_02.fbx when the asset does not
exist (vertex colours Replace, Generate Lightmap UVs off, Nanite on, no materials), gives _01 and _02 the slot material MI_RockSlab, then
makes the RockPieceMeshes list of that shape on the BP_ArenaGrid class defaults (_01, _02). The import task comes back 100 times too
large (FBX scene unit): run reimport_static_mesh.py without --vertex-colors after it, then static_mesh_build_settings.py.
Lines tagged ROCKVARIANTS, also written to the file named by env ROCKVARIANTS_OUT.
"""

import os

import unreal

TAG = "ROCKVARIANTS"
FOLDER = "/Game/RogueRigs/Environment/Rock"
GRID_BP = "/Game/RogueRigs/Blueprints/Terrain/BP_ArenaGrid"
SHAPES = ("Edge", "Outer", "Inner", "Diagonal")
ROOT = unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_dir())
LOG_LINES = []


def log(msg):
    line = "{} {}".format(TAG, msg)
    LOG_LINES.append(line)
    unreal.log_warning(line)


def import_piece(shape):
    name = "SM_RockSlab_{}_02".format(shape)
    path = "{}/{}".format(FOLDER, name)
    if unreal.EditorAssetLibrary.does_asset_exist(path):
        log("{} exists".format(name))
        return unreal.EditorAssetLibrary.load_asset(path)
    options = unreal.FbxImportUI()
    options.set_editor_property("import_mesh", True)
    options.set_editor_property("import_materials", False)
    options.set_editor_property("import_textures", False)
    options.set_editor_property("import_as_skeletal", False)
    smid = options.static_mesh_import_data
    smid.set_editor_property("combine_meshes", True)
    smid.set_editor_property("vertex_color_import_option", unreal.VertexColorImportOption.REPLACE)
    smid.set_editor_property("generate_lightmap_u_vs", False)
    smid.set_editor_property("build_nanite", True)
    task = unreal.AssetImportTask()
    task.set_editor_property("filename", os.path.join(ROOT, "Art/Meshes/rock-slab", name + ".fbx").replace("\\", "/"))
    task.set_editor_property("destination_path", FOLDER)
    task.set_editor_property("destination_name", name)
    task.set_editor_property("replace_existing", False)
    task.set_editor_property("automated", True)
    task.set_editor_property("save", True)
    task.set_editor_property("options", options)
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
    mesh = unreal.EditorAssetLibrary.load_asset(path)
    if not isinstance(mesh, unreal.StaticMesh):
        raise RuntimeError("import failed: {}".format(name))
    log("{} imported".format(name))
    return mesh


def run():
    first = {s: unreal.EditorAssetLibrary.load_asset("{}/SM_RockSlab_{}_01".format(FOLDER, s)) for s in SHAPES}
    second = {}
    for s in SHAPES:
        mesh = import_piece(s)
        material = unreal.EditorAssetLibrary.load_asset(FOLDER + "/MI_RockSlab")
        for m in (first[s], mesh):
            m.set_material(0, material)
            if not unreal.EditorAssetLibrary.save_loaded_asset(m, only_if_is_dirty=False):
                raise RuntimeError("save failed {}".format(m.get_name()))
        second[s] = mesh
        box = mesh.get_bounding_box()
        log("{} slot {} nanite {} z {:.2f} to {:.2f}".format(mesh.get_name(), material.get_name() if material else "None",
                                                             mesh.get_editor_property("nanite_settings").get_editor_property("enabled"), box.min.z, box.max.z))
    bp = unreal.EditorAssetLibrary.load_asset(GRID_BP)
    cdo = unreal.get_default_object(unreal.EditorAssetLibrary.load_blueprint_class(GRID_BP))
    lists = cdo.get_editor_property("rock_piece_meshes")
    for s in SHAPES:
        lists.set_editor_property(s.lower(), [first[s], second[s]])
    cdo.set_editor_property("rock_piece_meshes", lists)
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    if not unreal.EditorAssetLibrary.save_loaded_asset(bp, only_if_is_dirty=False):
        raise RuntimeError("BP save failed")
    cdo = unreal.get_default_object(unreal.EditorAssetLibrary.load_blueprint_class(GRID_BP))
    lists = cdo.get_editor_property("rock_piece_meshes")
    for s in ("full", "edge", "outer", "inner", "diagonal"):
        log("{}: {}".format(s, ", ".join(m.get_name() if m else "None" for m in lists.get_editor_property(s))))


try:
    run()
    log("done")
except Exception as exc:
    log("error {}".format(exc))
    raise
finally:
    out = os.environ.get("ROCKVARIANTS_OUT")
    if out:
        with open(out, "w") as f:
            f.write("\n".join(LOG_LINES) + "\n")
