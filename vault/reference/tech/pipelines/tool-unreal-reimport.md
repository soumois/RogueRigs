---
id: tool-unreal-reimport
description: "Headless Python tool reimporting static meshes in place from their FBX: command, arguments, output, exit code, limits, test. Open after a Blender re-export."
parent: pipelines-index
kind: doc
---
# Tool: reimport a static mesh in place

`Tools/unreal/reimport_static_mesh.py` reimports existing static meshes from the source FBX stored in their import data. Use it after a re-export from Blender ([[blender-asset-route]]): the MCP import refuses an existing asset ([[unreal-mcp-quirks]]).

## Command
Editor closed. From Git Bash (`MSYS_NO_PATHCONV=1` keeps `/Game/...` intact):
```
MSYS_NO_PATHCONV=1 timeout 600 powershell.exe -NoProfile -Command "& 'C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor-Cmd.exe' 'C:\Users\soumo\Documents\Unreal Projects\RogueRigs\RogueRigs.uproject' -run=pythonscript '-script=\"C:/Users/soumo/Documents/Unreal Projects/RogueRigs/Tools/unreal/reimport_static_mesh.py /Game/RogueRigs/Art/XPNut/SM_XPNut /Game/RogueRigs/Art/XPChip/SM_XPChipBig\"' -unattended -nopause -nosplash -stdout -FullStdOutLogOutput; exit \$LASTEXITCODE"
```
- Arguments: the script path then one or more package paths, all inside the quoted `-script=` value (absolute script path, forward slashes).
- `-stdout -FullStdOutLogOutput` puts the result lines on the console; without them they are only in `Saved/Logs/RogueRigs.log` (`LogPython`).

## Behaviour
- Interchange `reimport_asset` (the engine path behind Content Browser Reimport; `unreal.EditorReimportHandler` does not exist in 5.8 Python), then save. Fallback: `AssetImportTask` over the same asset (replace existing, no materials, combined meshes).
- The asset stores no FBX translator settings; headless Interchange then converts the scene unit and Blender FBX (UnitScaleFactor 100) come back 100 times larger. The tool pins `convert_scene_unit` false, as the first FbxImportUI import did.
- One line per asset: `REIMPORT <path> ok method=... source=... bounds_min=... bounds_max=... slots=[...] unchanged`, or `CHANGED (before: ...)`, or `REIMPORT <path> error <reason>`. A new asset in the folder counts as an error.
- Exit code: 0 if every asset succeeded; otherwise the script raises, the commandlet logs `Python script executed with errors` and the process returns -1 (Git Bash through PowerShell shows 127). Other assets in the list are still processed.

## Companion scripts (task-220-06, same folder, editor closed)
- `reimport_texture.py <paths>` (`-run=pythonscript`): Interchange reimport from the stored source, writes back compression, mips, filter, sRGB, streaming, address and LOD group, saves; line `TEXREIMPORT <path> ok ... size=WxH ...`.
- `static_mesh_build_settings.py <paths>` (`-ExecutePythonScript`: editor subsystems are None in the commandlet): Generate Lightmap UVs off, full precision UVs, LightMapCoordinateIndex 1, rebuild, save; line `BUILDSETTINGS` with slot materials.
- `check_uv_channel.py <paths>` (`-ExecutePythonScript`, read-only): RENDER_DATA UV channel 2 per material ID, bins (i + 0.5) / 10, V flipped by the import (1 - centre); lines `UVCHECK`, off_bin and mixed must be 0.
- `pickup_simple_collision.py [--analyse] <path>[:aabb|:best] ...` (task-229-01, `-ExecutePythonScript`): replaces the simple collision of a mesh by one primitive fitted to the body (LOD 0 source without the `glow_disc` slot): `aabb` = axis-aligned box; `best` = oriented box when at least 15 percent smaller than the AABB, convex hull (48 faces) when the box is more than 2 times the hull volume; trace flag untouched (never simple as complex); saves, reads the body setup back; line `PICKUPCOLLISION <path> ok|error ...` (body bounds, volumes, choice, primitives before and after). `--analyse` and paths outside `/Game` are read only. A failed mesh logs `Python script executed with errors` but the process still exits 0 in this mode: check the lines.
- A mesh first imported by FbxImportUI keeps `FbxStaticMeshImportData` (no `get_translator_settings`): this tool falls back to `import_task`, which resets Generate Lightmap UVs on; run `static_mesh_build_settings.py` after it.

## Limits
- Editor closed, one Unreal process at a time; the Python Editor Script plugin must stay enabled.
- Static meshes only; materials are never created; the asset is saved even when nothing changed (binary diff): check `git status` and restore unchanged `.uasset` files.

## Test (task-137-01, unchanged FBX)
- SM_XPNut: z -5.85 to 5.85, slots M_XPNut_body, M_XPNut_hole, unchanged.
- SM_XPChipBig (deleted in task-221-03, 2026-10-17, replaced by SM_XPCubeBig): z -5.3 to 5.3, seven M_XPChipBig_* slots, unchanged.
- No new asset; BP_PickupXP and BP_PickupXPBig keep their PickupMesh mesh and every OverrideMaterials entry (read from the CDO after the run).
- Bad path: `error asset not found`, exit -1. The two rewritten `.uasset` files were restored with git.

## Relations
- [[pipelines-index]] — parent index.
- [[blender-asset-route]] — produces the FBX this tool reimports.
- [[unreal-mcp-quirks]] — why MCP cannot reimport.
- [[build-toolchain]] — the other headless UnrealEditor-Cmd commands.
