---
id: task-386-20-run-clips-arm-check
description: "3d: compare arm bone motion of the run clips; the blend space shows T-pose arms while the legs run"
parent: task-386-miner-mixamo-animation
kind: task
status: in-progress
---
# Miner animated with the owner Mixamo clips: run clips arm check

- goal: Find out whether A_Miner_RunBwd, RunLeft and RunRight (Art/Meshes/Miner/Anims/*.fbx) keep their arm animation, against RunFwd, whose arms hold the rifle in -game (task386_14). Capture task386_18 shows T-pose arms and running legs when the 4-sample blend space plays RunBwd (MoveX -1), also RunLeft.
- agent: 3d
- depends_on: task-386-18-capture-simple-blendspace
- inputs: Art/Meshes/Miner/Anims/ (A_Miner_RunFwd.fbx, RunBwd, RunLeft, RunRight), source downloads Art/Source/miner-mixamo/ (Run Backwards.fbx, Run Left.fbx, Run Right.fbx, Rifle Run.fbx), Tools/art/blender/miner_clips.py (the in-place export script), Saved/Captures/task386_18/crops.png, headless Blender route of the 3d agent
- expected_output: per clip, per arm and hand bone (shoulder, arm, forearm, hand) the rotation range over the cycle in the source FBX and in the exported FBX; the cause if the in-place script drops or flattens arm tracks; the fix (script change, re-export of the clip FBX) committed if clear
- done_when: the table of arm bone motion is reported for the 4 clips in source and export; either the cause and a fixed re-exported FBX (committed by path as task-386-20-run-clips-arm-check: <summary>) or a statement that the FBX arms are fine (the defect is then in Unreal)
- editor_state: closed
- token_ceiling: 0.5 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-30T17:01:13Z
- waiting_for: none
- result: open

## Relations
- [[task-386-miner-mixamo-animation]] — parent global task.