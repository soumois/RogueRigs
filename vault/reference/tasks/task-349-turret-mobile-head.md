---
id: task-349-turret-mobile-head
description: "Global task: Cut the turret game mesh into a fixed base and a head that turns about a vertical axis, as for the console C2."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Turret mesh: separate the rotating head from the fixed base

- request: Owner message 2026-10-28 (relayed by the main session, English translation of his French message): "You must also separate the mobile part of the turret -- rotation." This is the cut into a fixed base and a rotating head deferred as Q5 in [[task-345-turret-meshy-files]], [[task-346-turret-canon]] and [[task-347-turret-flat-surfaces]] ("a later request once the mesh is validated"). Input: `Art/Meshes/turret/SM_Turret.fbx` as left by task-347 (15,748 tris, budget 16,000, slots `turret_body` and `turret_canon`, canon with 15 shroud holes). Owner answer 2026-10-28 (relayed by the main session): "ok" = every default of Q1 to Q5 accepted; budget stays under 16,000 triangles.
- goal: The turret exists as two meshes, a fixed base and a head carrying the canon, whose pivot lies on the head's rotation axis, so the head can turn toward a target without gaps or clipping.
- acceptance: `Art/Meshes/turret/SM_TurretBase.fbx` and `SM_TurretHead.fbx` per Q1 to Q5; head pivot on the rotation axis and head offset on the base given in the report; head turned 0, 90, 180 and 270 degrees on the base shows no open hole at the cut and no part passing through the base (clearance numbers in the report); body unchanged otherwise (vertices, UVs, flat faces of task-347); total triangles under 16,000; renders sent to the owner by the Executor; art-prompts-turret updated; lint clean; committed.
- gdd_check: read 2026-10-28: [[turret]] (approved: locks the nearest enemy and fires until the target dies, leaves range or loses sight; no word on pitch or on a moving part), [[sys-turret]] (shots leave from `ShotHeight` above the turret straight toward the target; enemies stand on the same floor), [[bp-turret]] (grey-box today: EventTick yaws the head toward the target with FindLookAtRotation, barrel moved around the head; yaw only, no pitch), [[art-prompts-console]] (precedent: owner defined the mobile part, "it turns continuously about the vertical mast axis"; task-301-02 output two FBX, head origin on the mast axis, offset on the base in the report), [[art-prompts-turret]] (current mesh, 183 x 188 x 156 cm, canon axis parallel to Blender Y), [[asset-pipeline]] (pivot bottom centre, budget 16,000). No conflict; no GDD value is open for the cut. Unreal import is not required by the GDD (BP_Turret is the grey box; Q5 of tasks 345-347 kept the import as a separate request). Questions asked 2026-10-28, all DECIDED by the owner "ok" the same day (values provisional, owner 2026-10-28):
  - Q1 which parts turn: default and recommendation: everything above the turntable (the ring or neck between the base and the head) turns: head, canon, ammo box, sensors and any plate fixed on the head; legs, base and the turntable itself stay fixed. A part that straddles the cut (an ammo belt, a cable) goes with the head. The agent lists the parts on each side with a render before cutting further. DECIDED: default.
  - Q2 axis and pivot: default and recommendation: yaw only, about the vertical axis through the centre of the turntable (measured on the mesh), no canon pitch: the targets stand on the same floor, the camera is top-down and BP_Turret only yaws today, so pitch would cost a third part for an effect almost invisible. Head pivot on that axis at the cut height; base pivot bottom centre; the canon points along +X (Unreal forward) at rest in both files. DECIDED: default.
  - Q3 cut seam: default: the cut is closed by flat caps on both sides (top of the base, underside of the head), so no hole shows when the head turns; caps in the `turret_body` slot mapped to a dark metal area of the existing atlas, no new texture. DECIDED: default.
  - Q4 output format and materials: default and recommendation (console precedent): two FBX files `SM_TurretBase.fbx` (slot `turret_body`) and `SM_TurretHead.fbx` (slots `turret_body` and `turret_canon`), sharing the existing maps, no new material; `SM_Turret.fbx` stays as the assembled reference. DECIDED: default. Total triangles stay under 16,000.
  - Q5 no Unreal import in this task — default ok (BP_Turret stays the grey box; import is a separate request). DECIDED: default.
- confirmed_by_owner: yes (2026-10-28, "ok" to the plan and every default of Q1-Q5, relayed by the main session)
- milestone: M2
- next_step: task-349-01-turret-mobile-head
- started: 2026-10-28T07:14:11Z
- waiting_for: none
- result: open

## Subtasks
1. task-349-01-turret-mobile-head (vault/reference/tasks/task-349-turret-mobile-head/task-349-01-turret-mobile-head.md) — 3d: cut SM_Turret into SM_TurretBase and SM_TurretHead (pivot on the yaw axis), cap the seam, check a 360 degree sweep, render.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-349-01-turret-mobile-head]] — step 1, 3d.
- [[task-347-turret-flat-surfaces]] — must be done first: its SM_Turret.fbx is this task's input.
- [[task-346-turret-canon]] — built the canon the head carries.
