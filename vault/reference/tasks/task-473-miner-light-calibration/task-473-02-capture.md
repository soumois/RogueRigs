---
id: task-473-02-capture
description: "Capture: Final independent check of the miner light against image 2."
parent: task-473-miner-light-calibration
kind: task
status: done
---
# Miner light calibrated on the owner in-game reference: capture

- goal: Final independent check of the miner light against image 2.
- agent: capture
- depends_on: task-473-01-light-match-loop
- inputs: agent-capture (vault/reference/agents/agent-capture.md), Art/Source/cartoon-look/reference/owner-2026-11-05-ingame-light-walls.webp, Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-06.webp
- expected_output: see done_when
- done_when: C1 warm orange centre fading to pink-violet; C2 pool size and falloff close to image 2; C3 floor under the miner below about 205/255; C4 robots and pickups inside the pool not lit by it; C5 wall shadows from the miner light still present; side-by-side sheets before / after / image 2 sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-473
- usage: included in the executor run of task-473
- runs: 1
- milestone: M2
- started: 2026-11-06T18:32:36Z
- waiting_for: none
- result: 3 launches (task473-capture; Objective Console/Chest found none, so FuseBarrel 200 placed the miner by a wall). C1 seen: warm orange centre fading to red-magenta (w_00.png); the outer ring is redder and darker than the pink-violet of image 2. C2 seen: size and falloff close to image 2 (radial medians within about 10 of image 2 from 0.15h to 0.25h). C3 not seen: floor near the miner R channel p95 219, max 228 (rule about 205); image 2 itself has R p95 252 there; owner call. C4 seen: robots (task473-r3/r_01.png) keep their yellow, crystal pickups stay purple-white inside the pool (y_00.png). C5 ambiguous: castShadows true read back; x_00.png shows the pool stopping at the wall, but no floor behind a wall inside the pool radius is in view. Sheets: Saved/Captures/task473-capture/final_sheet.png and final_sheet_crop.png (before / after / image 2), round sheets in task473-r1..r3.

## Relations
- [[task-473-miner-light-calibration]] — parent global task.
