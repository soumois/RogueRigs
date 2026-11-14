---
id: task-545-03-capture
description: "Capture: Pause screen captures against image 10 at four resolutions"
parent: task-545-pause-screen-bolts
kind: task
status: done
---
# Pause screen: grid and bolts on the frame bars as owner image 10: capture

- goal: Pause screen captures against image 10 at four resolutions
- agent: capture
- depends_on: task-545-02-pause-layout
- inputs: umg-layout (vault/reference/tech/conventions/umg-layout.md), Saved/Scratch/owner-ui-batch-2026-11-13/10.webp
- expected_output: see done_when
- done_when: C1 vertical dividers touch the top and bottom frame bars; C2 horizontal divider spans the two weapon columns to the right frame side; C3 a bolt on each divider end and junction, centred on the bar; C4 bolts round, not stretched, at each resolution; C5 title bars as image 10; C6 stats and buttons unchanged.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-545
- usage: included in the executor run of task-545
- runs: 1
- milestone: M2
- started: 2026-11-14T03:48:00Z
- waiting_for: none
- result: 4 launches (1280x720, 1920x1200, 2560x1080 with 3 weapons; 1920x1080 with 2 weapons) plus the 1920x1080 3-weapon frame of subtask 02. Sheet Saved/Captures/t545_c_sheet.png (with target 10), bolt crops Saved/Captures/t545_c_bolt_crops.png. C1 seen: both vertical dividers end on the top and bottom frame bars. C2 seen: horizontal divider from the first vertical to the right frame side across the two weapon columns. C3 seen: bolts on the 4 vertical ends, the 2 junctions and the right frame side, centred on the bar. C4 seen: bolts round, same size, at the four resolutions. C5 seen: title bars long and thick as image 10 with the bolt next to PAUSED (tips are the current sprite, as decided). C6 seen: stats, texts and buttons unchanged; 2 weapons shows the second vertical only, no horizontal divider.

## Relations
- [[task-545-pause-screen-bolts]] — parent global task.
