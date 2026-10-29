---
id: task-363-01-rr-blender-module
description: "3d: write `Tools/art/blender/rr_blender.py` with the decided helpers and prove it with one headless import check, no asset."
parent: task-363-blender-shared-module
kind: task
status: done
---
# Shared Blender helper module for the 3d agent: 3d

- goal: Write `Tools/art/blender/rr_blender.py` per the decided Q2 and Q3 of the global (read its `request`, `gdd_check` and `## Questions`), generalising the existing definitions: read only the `def` blocks named in the global gdd_check (`grep -n "^def"` then the line range), not whole scripts. Then run the decided Q4 check. Do not modify any existing script; no FBX export, no render, no asset. This is not a Blender-route asset: steps 2, 3 and 5 of your definition do not apply.
- agent: 3d
- depends_on: none
- inputs: task-363-blender-shared-module (vault/reference/tasks/task-363-blender-shared-module.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), `Tools/art/blender/drone_cut_parts.py` (import_fbx, tri_count, make_material), `Tools/art/blender/rock_slab_pieces.py` (srgb, make_material, setup_render), `Tools/art/blender/barrel_preview.py` (standard preview), `Tools/art/blender/barrel.py` (FBX export and report json), `Tools/art/blender/cleanup.py` (tri_count, report json)
- expected_output: `Tools/art/blender/rr_blender.py` committed alone by path (commit `task-363-01-rr-blender-module: ...`); report: the public functions with signatures, the check command and its output, Blender processes stopped.
- done_when: the module exists with every Q3 helper and a header docstring listing each one in one line; nothing runs at import time except imports; a headless `"$BLENDER_EXE" -b --factory-startup --python-expr` (or a script in the session scratch dir, outside the repo) inserts `Tools/art/blender` in `sys.path`, imports `rr_blender`, calls `srgb`, `tri_count`, `dimensions_cm` and builds the report dict on a cube created in memory, prints the values and exits 0; `git status` shows no other change under `Tools/art/` and no FBX, PNG or json written; `Get-Process blender` empty after the run; committed by path.
- editor_state: any (Unreal not used)
- token_ceiling: 1.2 M units (agent ceiling; expected well under)
- model: sonnet
- tokens: input 20, cache write 32602, cache read 213578, output 7034
- usage: turns 10, context start 11k peak 33k, resumes 0, 1 min, units 0.10 M
- runs: 1
- milestone: M2
- started: 2026-10-29T15:39:34Z
- waiting_for: none
- result: rr_blender.py committed (0ed1b95f), 186 lines, 10 helpers; headless check on in-memory cube exit 0 (srgb, tri_count 12, dimensions_cm [50,40,30], report dict). import_fbx, export_fbx and preview/turntable untested by design (no FBX, no render).

## Relations
- [[task-363-blender-shared-module]] — parent global task.
