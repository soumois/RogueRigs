---
id: task-363-blender-shared-module
description: "Global task: shared bpy module rr_blender.py (FBX import and export, report json, preview, tris, sRGB, material) that new 3d scripts import."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Shared Blender helper module for the 3d agent

- request: owner, 2026-10-29, in French, verbatim: "Oui, planifier", answering the main session's question "Je fais planifier la bibliothèque Blender partagée par le crew ?". Context relayed by the main session (3d cost analysis, 2026-10-29): 3d subtasks 334-01..349-01 cost 3.6 M units over 6 runs; output is ~40 % of 3d cost, mostly Blender scripts written from scratch (transcripts gone, only usage-snapshot numbers and task nodes remain). `Tools/art/blender/` holds ~17,600 lines in ~40 scripts with no shared module; repeated helpers: `import_fbx` in 10 files, `srgb` 9, `tri_count` 7, render setup 4+, `make_material` 4, `apply_pose` 6; FBX export in 32 files, report json in 39, render calls in 27. Estimated gain: a new script 15-25 % shorter, about -10 % of 3d cost, fewer copy-paste bugs. Goal as relayed: one shared module (name the Planner's call) with import FBX, export FBX in centimetres, report json (tris, dimensions in cm, materials, UV layers, warnings), standard preview render / turntable, triangle count, sRGB conversion, material helper; `.claude/agents/3d.md`, agent-3d, blender-asset-route and the lessons node tell the agent to import it instead of rewriting helpers. Main session's view: leave existing scripts as they are, module for new scripts only, prove it on one tiny headless import check if `BLENDER_EXE` is available, no asset produced; measure/build split of heavy mesh-surgery subtasks is speculative (ask, default no). Constraints: one owner point = one global task; no asset runs, renders or tests beyond the import check; agent definition changes update their node.
- goal: New 3d Blender-route scripts import one shared module `Tools/art/blender/rr_blender.py` for the common helpers instead of redefining them; the 3d agent definition, its node, blender-asset-route and art-3d-lessons say so.
- acceptance: Per the decided Q1 to Q5: `Tools/art/blender/rr_blender.py` exists with the decided helper list, a header docstring listing each public function in one line (the API reference), `bpy` imported inside the module, no code run at import time; a headless `"$BLENDER_EXE" -b --factory-startup` run imports it from a script outside `Tools/art/blender/` through `sys.path` and calls the pure helpers on an in-memory cube (srgb, tri_count, dimensions, report dict) with exit code 0, writing no FBX, no render, no file in the repo; no existing script under `Tools/art/blender/` is modified (`git diff --stat` lists only the new module); `.claude/agents/3d.md` step 4, agent-3d, blender-asset-route and art-3d-lessons tell the agent to import `rr_blender` and not redefine its helpers; lint clean; committed by path. No asset run, no render.
- gdd_check: crew tooling, no gdd node involved, no gdd edit (Planner, 2026-10-29). No task in progress (tasks-to-do-index: 308, 309, 314 todo). Found: `BLENDER_EXE` is set (User variable, `C:\Program Files\Blender Foundation\Blender 5.2\blender.exe`), so the import check can run. Helper definitions found (`^def`): `import_fbx` in drone_cut_parts.py, drone_antenna.py, meshy_cut_parts.py, turret_cut_head.py and 6 others; `tri_count` in cleanup.py, chest_cut.py, drone_cut_parts.py and 4 others; `srgb` in barrel.py, rock_slab_pieces.py, xp_nut_mesh.py and 6 others; `make_material` in asteroid.py, drone_cut_parts.py, rock_plates.py, rock_slab_pieces.py; `setup_render` in rock_plates.py, rock_slab_broken.py, rock_slab_pieces.py, shield_colour_test.py; the standard preview camera is in barrel_preview.py and blender-asset-route "Checking without a screen"; FBX export rule `FBX_SCALE_UNITS` in art-3d-lessons. `Tools/art/blender/chaser/common.py` (325 lines: setup, finish, lm_check, write_part_report) is already a shared module, but chaser-specific; it is not touched. API doc: like C++ (function detail in header comments, [[_authoring]]), the module header docstring is the API list; no new vault node, blender-asset-route gets a short "Shared module" section. Rule for later: generic helpers go in the module, task-specific geometry stays in the asset script, an existing function signature is never changed (scripts import it). A doc subtask is not mandatory (no cpp or bp subtask) but the definition and node edits go to the doc agent as in task-362.
- confirmed_by_owner: no (plan and questions handed back 2026-10-29)
- milestone: M2
- next_step: task-363-01-rr-blender-module
- started: open
- waiting_for: owner confirmation of the plan
- result: open

## Questions
Asked 2026-10-29, each with a default; open until the owner answers.
- Q1 Existing scripts. Options: (a) left as they are, the module is used by new scripts only (they are finished assets; migrating one means re-running it to prove identical output, i.e. asset runs not requested); (b) migrate them now in this task; (c) a later separate task when the owner asks. Default: (a).
- Q2 Module name and place: `Tools/art/blender/rr_blender.py`, imported with `sys.path.insert(0, <Tools/art/blender>)` then `import rr_blender as rr`; `chaser/common.py` stays as is. Default: yes.
- Q3 Helper list: `import_fbx`, `export_fbx` (centimetres, `FBX_SCALE_UNITS`, selected objects), `tri_count`, `dimensions_cm`, `srgb` (hex or 0-1 tuple to linear), `make_material` (flat Principled colour, roughness, metallic), `setup_preview` and `render_preview` (the standard camera and light of blender-asset-route, parameters overridable), `render_turntable` (N azimuths, same setup), `write_report` (tris, dimensions_cm, materials, uv_layers, warnings, plus any extra keys the script passes). `apply_pose` (6 files) is left out: its pose formats differ per robot script. Default: this list.
- Q4 Proof: one headless Blender run (`BLENDER_EXE`, Blender 5.2) that imports the module and calls the pure helpers on an in-memory cube; no FBX export, no render, no file written, no existing asset re-run. Option: no run at all (syntax check only). Default: the headless run.
- Q5 Split future heavy mesh-surgery 3d subtasks into a measure subtask (compact report) and a build subtask with a fresh context. Default: no (speculative without transcripts; not planned).

## Subtasks
1. task-363-01-rr-blender-module (vault/reference/tasks/task-363-blender-shared-module/task-363-01-rr-blender-module.md) — 3d: write `Tools/art/blender/rr_blender.py` with the decided helpers and prove it with one headless import check, no asset.
2. task-363-02-blender-module-doc (vault/reference/tasks/task-363-blender-shared-module/task-363-02-blender-module-doc.md) — doc: 3d definition, agent-3d, blender-asset-route and art-3d-lessons tell the agent to import rr_blender instead of rewriting helpers.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-363-01-rr-blender-module]] — step 1, 3d.
- [[task-363-02-blender-module-doc]] — step 2, doc.
- [[task-362-agents-path-of]] — previous change to the 3d agent definition (path-of.js).
