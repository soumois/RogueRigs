---
id: task-544-04-capture
description: "Capture: Captures of the "!" and the gauge through one horde cycle, four resolutions"
parent: task-544-horde-kill-gauge
kind: task
status: done
---
# Hordes: one "!" at half of C, then a kill gauge under the alert bar triggers each horde: capture

- goal: Captures of the "!" and the gauge through one horde cycle, four resolutions
- agent: capture
- depends_on: task-544-03-gauge-widget
- inputs: umg-layout (vault/reference/tech/conventions/umg-layout.md), Saved/Scratch/owner-ui-batch-2026-11-13/4.webp
- expected_output: see done_when
- done_when: C1 the "!" at half of C at level start; C2 no gauge before the bar reaches it; C3 gauge under the bar, partly filled after kills, "!" gone; C4 gauge full and pulsing with the horde banner; C5 gauge empty and refilling after the horde; C5b gauge look close to the task-544-02 render; C6 alert bar and gauge laid out correctly at the four resolutions of umg-layout (Q10).
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-544
- usage: included in the executor run of task-544
- runs: 1
- milestone: M2
- started: 2026-11-14T00:50:09Z
- waiting_for: none
- result: Captures in Saved/Captures/: C1 seen (task544_cycle_1080/crop_top.png row 1: one '!' at half of C, no other horde mark); C2 seen (same frame, no gauge); C3 seen (crop_gauge.png row 1: gauge attached under the bar, partly filled, '!' gone); C4 seen (crop_gauge.png rows 2-3 with the HORDE INCOMING banner in g_05/g_06: full, two red shades = pulse); C5 seen (crop_gauge.png row 4: empty and refilling after the horde was killed, log Filling 0/150); C5b seen (gauge look matches render 20261114-014334: riveted frame, dark track, red fill, '!' at the left; render gauge is wider, ~55 % vs a third in the widget per brief); C6 seen at 1920x1080, 1280x720, 1920x1200, 2560x1080 (task544_cycle_1080/crop_resolutions_attached.png at top 97, and task544_att2_1920x1080/crop.png after the final 5-unit move to top 92, no gap); owner addition: slide seen (task544_slide_1080/slide_crop.png: gauge emerging from behind the bar, then settled). 2560x1080 frames after ~8 s land on the focus-loss PAUSED screen, so early shots were used there. Launches: 3 for the round before the owner addition, 6 after (slide, cycle, 3 resolutions, 1 recheck). ALERT: executor over budget (5.45 M units at 157 turns).

## Relations
- [[task-544-horde-kill-gauge]] — parent global task.
