---
id: task-528-02-capture
description: "Capture: arrival takeoff followed by the miner"
parent: task-528-extractor-takeoff-under-floor
kind: task
status: done
---
# L'extracteur rentre dans le sol en partant: capture

- goal: arrival takeoff followed by the miner
- agent: capture
- depends_on: task-528-01-takeoff-path
- inputs: agent-capture (vault/reference/agents/agent-capture.md), extraction (vault/reference/gdd/run/extraction.md)
- expected_output: see done_when
- done_when: C1 frames over the whole takeoff, the extractor always above the floor; C2 it leaves the view climbing; frames sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-528
- usage: included in the executor run of task-528
- runs: 2
- milestone: M2
- started: 2026-11-13T14:23:33Z
- waiting_for: none
- result: 2 launches (arrival takeoff at level start). C1 seen: Saved/Captures/task528_side/side_crop.png (side view, camera pitch -8, arm 4500, yaw 90), hull above the floor on every frame, nose down 20 deg; WatchExtractors log Z 31, 226, 370, 407, 439, 495, 577, 686 uu, never decreasing. C2 seen: it leaves the view climbing (side_07 at the top-left edge; top-down zoomed-out Saved/Captures/task528_top/contact_sheet.png, frames 3-8, it rises, tilts and leaves at the screen top, hull whole). Miner following not simulated: zoomed-out camera instead. Frames reported to the owner.

## Relations
- [[task-528-extractor-takeoff-under-floor]] — parent global task.
