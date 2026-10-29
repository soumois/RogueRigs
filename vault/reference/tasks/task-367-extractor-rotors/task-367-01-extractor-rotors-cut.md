---
id: task-367-01-extractor-rotors-cut
description: "3d: cut the four rotors (blades and hub) from SM_Extractor into their own FBX with origins on their spin axes, cap the cuts, sweep 360 degrees, render."
parent: task-367-extractor-rotors
kind: task
status: in-progress
---
# Extractor mesh: extract the four rotors so they can spin: 3d

- goal: From `Art/Meshes/extractor/SM_Extractor.fbx` as re-baked by task-368 (task-366 geometry, new UV0 and maps), a fixed body and four rotor meshes pivoting on their spin axes, per the Q2, Q5 and Q6 answers in task-366 and Q3 of task-368.
- agent: 3d
- depends_on: task-366-02-extractor-flatten, task-368-01-extractor-rebake-maps
- restart_note: a first run (2026-10-29) was stopped before the re-bake; it left untracked `Tools/art/blender/extractor_cut_rotors.py` (kept as the starting point: its selection rule, scan and sweep stand) and untracked outputs in `Art/Meshes/extractor/` (five `SM_Extractor_Body|Rotor_*.fbx`, their `.fbm` folders, `extractor_rotors.report.json`): delete those outputs first, then rerun on the re-baked mesh; the cap texel is picked again on the new base colour map; UVs are compared with the re-baked SM_Extractor.fbx. That run measured 48,154 triangles total, 0 boundary edges, sweep intersections FL 51, FR 49, RL 5, RR 439 (report them again, do not reshape).
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), extraction (vault/reference/gdd/run/extraction.md), task-349-01-turret-mobile-head (vault/reference/tasks/task-349-turret-mobile-head/task-349-01-turret-mobile-head.md) (precedent: method, done_when, result), decisions: `request` and `gdd_check` (vault/reference/tasks/task-367-extractor-rotors.md); `## Questions` Q2, Q5, Q6 with the owner answers (vault/reference/tasks/task-366-extractor-flat-faces.md), files: `vault/reference/gdd/art-audio/art-prompts-extractor.md`, `Art/Meshes/extractor/SM_Extractor.fbx` with its maps (re-baked by task-368), `extractor_flat.report.json`, `extractor_rebake.report.json`, task-368 `## Questions` Q3 (vault/reference/tasks/task-368-extractor-rebake.md), `Tools/art/blender/extractor_cut_rotors.py` (untracked, first run), `Tools/art/blender/turret_cut_head.py`, `Tools/art/blender/drone_cut_parts.py`, `Tools/art/blender/turret_preview.py`, `Tools/art/blender/rr_blender.py` (header docstring only)
- expected_output: `Tools/art/blender/extractor_cut_rotors.py` (imports rr_blender); `Art/Meshes/extractor/SM_Extractor_Body.fbx`, `SM_Extractor_Rotor_FL.fbx`, `_FR`, `_RL`, `_RR` (centimetres, `FBX_SCALE_UNITS`); `Art/Meshes/extractor/extractor_rotors.report.json` (selection rule per rotor, spin axis direction and point, blade-plane height, rotor radius, offset of each rotor origin in the body frame in cm, caps, clearance per angle, triangles per mesh and total, slots, UV check, warnings); renders in `Saved/ExtractorMesh/rotors/` (not committed): assembled, rotors tinted, rotors lifted, rotors turned 45 degrees, close-up of a cap; "Body and rotors" section added to art-prompts-extractor.
- done_when: five FBX reimport in a clean scene with the reported bounds; each rotor origin on its measured axis within 0.1 cm and the body origin bottom centre; boundary edge count at every cut = 0; 36 sweep angles per rotor with no rotor-body face intersection, or each collision reported with angle, place and depth and nothing reshaped; total triangles at most 50,000; body vertices away from the cuts and all UVs unchanged versus SM_Extractor.fbx (numbers in the report); renders exist and their paths are reported so the Executor sends them to the owner; art-prompts-extractor updated; lint clean; committed by path as `task-367-01-extractor-rotors-cut: <summary>`; `Get-Process blender` empty.
- editor_state: any
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-29T20:49:01Z
- waiting_for: none
- result: open

## Relations
- [[task-367-extractor-rotors]] — parent global task.
