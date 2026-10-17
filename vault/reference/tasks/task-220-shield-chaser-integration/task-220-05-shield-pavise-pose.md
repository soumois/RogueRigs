---
id: task-220-05-shield-pavise-pose
description: "3D agent: shield 20 percent larger, carried low like a pavise near the ground with the head above it; rebake clips 0 and 1 and the combined mesh."
parent: task-220-shield-chaser-integration
kind: task
status: todo
---
# Shield chaser: pavise carry

- request: "20 % plus grand, mais plus bas. Ça se porte comme un pavois, près du sol, mais la tête dépasse." (owner 2026-10-17, on the contact sheet of task-220-01)
- goal: the same outputs as task-220-01, overwritten in place, with the shield scaled 1.2 and carried low.
- agent: agent-3d (Blender, no Meshy)
- depends_on: task-220-02-shield-chaser-material (it imports the current files; do not overwrite them while it runs)
- inputs: `Tools/art/blender/shield_chaser_bake.py` and its outputs in `Art/Meshes/shield-chaser/` (report, contact sheets); `vault/reference/tasks/task-220-shield-chaser-integration/task-220-01-shield-clips-bake.md` (method and checks, unchanged unless stated here); `vault/reference/tech/pipelines/chaser-meshy-pipeline.md`; `vault/reference/agents/art-3d-lessons.md`.
- method: in `shield_chaser_bake.py` (parameters, not a new script): shield uniform scale 1.2 (98.4 x 51.4 cm, shield FBX unchanged); clip 0 guard: shield bottom 5 to 10 cm above the floor, plate facing +X, near vertical (tilt back 0 to 5 degrees), top edge low enough that the whole head block shows above it from the front view (report the gap between the shield top and the top of HeadTorso, at least 20 cm); left arm lower accordingly (forearm closer to vertical, grip held at the hand); clip 1 bash: the same low shield pushed forward at arm's length with the torso lean, the shield bottom never below the floor. Clips 2 and 3, slots, PartId 0.45, UV checks: unchanged. Re-run the clearance check (no overlap with the legs during the run: if the swinging legs hit the shield, move it forward rather than shrink the stride).
- done_when: the checks of task-220-01 pass again; head gap reported; the same previews and contact sheets regenerated (front three-quarter and side); the Orchestrator opens them.
- editor_state: any (no Unreal)
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-220-shield-chaser-integration]] — parent global task.
