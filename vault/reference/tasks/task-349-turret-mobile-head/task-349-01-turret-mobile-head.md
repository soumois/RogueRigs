---
id: task-349-01-turret-mobile-head
description: "Cut SM_Turret into a fixed SM_TurretBase and a SM_TurretHead pivoting on the yaw axis, cap the seam, check a 360 degree sweep, render."
parent: task-349-turret-mobile-head
kind: task
status: in-progress
---
# Turret mesh: cut the rotating head from the fixed base

- goal: From `Art/Meshes/turret/SM_Turret.fbx` (task-347 output), produce a fixed base mesh and a head mesh pivoting on its rotation axis, per the global `gdd_check` Q1 to Q5 as answered by the owner.
- agent: 3d
- depends_on: task-347-01-turret-flat-surfaces
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), turret (vault/reference/gdd/weapons/turret.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), art-prompts-console (vault/reference/gdd/art-audio/art-prompts-console.md), art-prompts-turret (vault/reference/gdd/art-audio/art-prompts-turret.md), bp-turret (vault/reference/tech/blueprints/player/bp-turret.md), sys-turret (vault/reference/tech/systems/combat/sys-turret.md); precedent: task-301-02-console-c2-cut (vault/reference/tasks/task-301-console-c2-mesh/task-301-02-console-c2-cut.md); decisions: global `request` and `gdd_check` (vault/reference/tasks/task-349-turret-mobile-head.md); files: `Art/Meshes/turret/SM_Turret.fbx` with its maps, `turret_canon.report.json`, `turret_flat.report.json`, `Tools/art/blender/turret_preview.py`.
- method: one headless Blender script `Tools/art/blender/turret_cut_head.py`. (1) Find the turntable (ring or neck between base and head): its centre, radius and height; log the rule used. (2) Split the faces per Q1; list the parts on each side (render `Saved/TurretMesh/cut/parts_*.png`, base and head tinted). (3) Close the cut per Q3. (4) Pivots per Q2: head origin on the rotation axis at the cut height, base origin bottom centre, canon along +X at rest; record the head offset in the base frame (cm). (5) Sweep the head about the axis every 10 degrees over 360 and measure the minimum clearance between head and base (no face intersection). (6) Export per Q4, centimetres (`FBX_SCALE_UNITS`); reimport both in a clean scene to check dimensions, pivots, slots and the +X canon direction. (7) Renders in `Saved/TurretMesh/cut/` (not committed): assembled at 0, 90, 180, 270 degrees, head lifted, close-up of the caps, same camera and light as task-346/347. (8) Add a "Base and head" section to art-prompts-turret (files, axis, offset, parts per side, tris). (9) Stop every Blender process.
- expected_output: `Tools/art/blender/turret_cut_head.py`; `Art/Meshes/turret/SM_TurretBase.fbx`, `SM_TurretHead.fbx`; `Art/Meshes/turret/turret_cut.report.json` (turntable measurements, parts per side, axis, head offset, pivots, clearance per angle, tris per mesh and total, slots, UV check, warnings); renders in `Saved/TurretMesh/cut/`; art-prompts-turret updated.
- done_when: two FBX reimport with the expected dimensions; head origin on the measured axis (offset within 0.1 cm) and base origin bottom centre; canon along +X at rest; no open boundary at the cut on either mesh (boundary edge count at the cut = 0); no head-base intersection at any of the 36 sweep angles; body vertices away from the cut and all UVs unchanged versus SM_Turret.fbx (numbers in the report); total triangles under 16,000; slots per Q4; renders exist and their paths are reported so the Executor sends them to the owner; art-prompts-turret updated; lint clean; committed by path as `task-349-01-turret-mobile-head: <summary>`; `Get-Process blender` empty.
- editor_state: any
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-28T07:14:11Z
- waiting_for: none
- result: open

## Relations
- [[task-349-turret-mobile-head]] — parent global task.
