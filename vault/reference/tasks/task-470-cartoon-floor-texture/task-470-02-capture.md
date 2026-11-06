---
id: task-470-02-capture
description: "Capture: Check the new floor in game against image 2 and the look before."
parent: task-470-cartoon-floor-texture
kind: task
status: done
---
# Floor uses the owner cartoon flagstone texture: capture

- goal: Check the new floor in game against image 2 and the look before.
- agent: capture
- depends_on: task-470-01-floor-material
- inputs: agent-capture (vault/reference/agents/agent-capture.md), Art/Source/floor-texture/owner-2026-11-05-floor-cartoon.webp, Art/Source/cartoon-look/reference/owner-2026-11-05-ingame-light-walls.webp, Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-06.webp
- expected_output: see done_when
- done_when: C1 floor shows the image 1 flagstones everywhere with their colours and pink crack rims; C2 flagstone size about that of image 2; C3 no seam or obvious grid repetition; C4 robots, pickups, miner and drone stand out from the floor; C5 ground circles and floor effects still show; side-by-side sheet before / after / image 2 sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-470
- usage: included in the executor run of task-470
- runs: 1
- milestone: M2
- started: 2026-11-06T10:39:04Z
- waiting_for: none
- result: Done, 2 launches. C1 seen: image 1 flagstones over the whole floor (wide frame cw_00), pink crack rims visible in the light pool (crop_rims.png); measured mean colour of the lit floor sRGB (108,29,62) vs image 2 (116,34,67), hue ratio B/R 0.72 vs image 1 0.75; far floor away from the light is darker than image 2 (39,6,28 vs 68,14,57), a light matter left to task-473. C2 seen: a stone about 250-300 px, 3.5 to 4 miner widths, as image 2. C3 seen: no seam, no obvious grid at normal or wide view. C4 seen: chasers, XP, ore and heal pickups, miner and drone stand out (c_01). C5 seen: level-up wave and its ground ring drawn on the floor (task470-floor-capture-fx lv_00..02); heal ring not caught in hr frames. Frames: Saved/Captures/task470-floor-capture/ (c_00, c_01, cw_00, compare_before_after_ref.png, crop_rims.png) and Saved/Captures/task470-floor-capture-fx/contact_sheet.png. No material error in run logs.

## Relations
- [[task-470-cartoon-floor-texture]] — parent global task.
