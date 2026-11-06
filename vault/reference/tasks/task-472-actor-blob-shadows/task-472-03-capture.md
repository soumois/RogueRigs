---
id: task-472-03-capture
description: "Capture: Check the shadows in game against the seven owner points."
parent: task-472-actor-blob-shadows
kind: task
status: done
---
# Light-independent purple-grey shadows under actors: capture

- goal: Check the shadows in game against the seven owner points.
- agent: capture
- depends_on: task-472-02-shadow-material-wiring
- inputs: agent-capture (vault/reference/agents/agent-capture.md), Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-06.webp
- expected_output: see done_when
- done_when: C1 each listed actor has one shadow directly beneath it; C2 the shadow stays under a moving robot and the drone on consecutive frames; C3 same shadow look inside and outside the miner light pool; C4 purple-grey, not black; C5 soft edge, silhouette readable, not too transparent or blurred; C6 no second (light) shadow under these actors, walls still shadowed; sheet before / after sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-472
- usage: included in the executor run of task-472
- runs: 1
- milestone: M2
- started: 2026-11-06T18:18:21Z
- waiting_for: none
- result: Done, run by the executor (4 launches: task472-shadows-a, task472-shadows-b, task472-shadows-barrel, task472-capture-move in Saved/Captures). C1 seen for miner, drone, chaser, heavy, kamikaze, shooter, shield chaser, turrets; AMBIGUOUS for the bonus chest and barrels: the disc exists (tests, frames) but at SizeScale 1.2 only a thin dark rim shows around the object (crop_chest-a0.png, crop_barrels-d0.png), owner to judge or raise their SizeScale / BarrelShadowScale. C2 seen: the drone and a walking chaser keep their disc under them over 3 frames 0.12 s apart (task472-capture-move/c2.png). C3 seen: same purple-grey disc in the light pool (miner, drone) and out of it (turrets, robots); the blend follows the floor brightness under it. C4 seen: purple-grey, not black (unblended about sRGB 35, 30, 43 after the tint correction of 02). C5 seen: soft outer edge, round readable silhouette at opacity 0.6. C6 seen: no second elongated light shadow under these actors; wall shadow flags untouched (rock blocks still cast). Before / after sheet: Saved/Captures/task472-capture-move/before_after.png. A8 frame time not measured (unverified; the robot discs existed since task-264, no new per-robot component).

## Relations
- [[task-472-actor-blob-shadows]] — parent global task.
