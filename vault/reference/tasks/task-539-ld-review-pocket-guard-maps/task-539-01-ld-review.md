---
id: task-539-01-ld-review
description: "Level-design: Before/after visual and measured review of the 7 starts whose accepted seed changed after task-537."
parent: task-539-ld-review-pocket-guard-maps
kind: task
status: done
---
# Revue visuelle Level-Design des cartes changées par la libération du garde de poche: level-design

- goal: Before/after visual and measured review of the 7 starts whose accepted seed changed after task-537.
- agent: level-design
- depends_on: none
- inputs: layout-review-rules (vault/reference/tech/pipelines/layout-review-rules.md), task-495-02-ld-review (vault/reference/tasks/task-495-generator-window-repair/task-495-02-ld-review.md), task-492-03-ld-validation (vault/reference/tasks/task-492-cpp-layout-generator/task-492-03-ld-validation.md), task-537-layout-stale-pocket-guard (vault/reference/tasks/task-537-layout-stale-pocket-guard.md), sys-layout-generator (vault/reference/tech/systems/world/sys-grid/sys-layout-generator.md), Saved/GeneratedLevels/task-537/, Tools/layout_gen/params/fine_default.json
- expected_output: Table, previews and the sheet in a Saved/GeneratedLevels/ folder named after this task (temporary, not committed); verdict in this subtask node; one line in layout-review-rules Automation.
- done_when: A1 to A4 of the global hold; the sheet path and the per-start verdict are in result; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-539
- usage: included in the executor run of task-539
- runs: 1
- milestone: M2
- started: 2026-11-13T20:18:51Z
- waiting_for: none
- result: A1: the 7 starts regenerated at HEAD give 1500, 1702, 1900, 2006, 2700, 2803, 7000 with the task-537/after cell hashes, and all 14 maps pass validate_layout.js. A2: independent scan (scan539.js) matches the generator: after windows 0.063-0.929, lanes 19-28, farthest pocket 6-10, all within the rules. A3 verdicts: 1500 ok; 2000 ok (west lane of 28 at 62-70,122-149); 1700 a surveiller (six pockets packed in the east third at 150-225,60-200, straight carved bar at 117-140,72-80); 1900 a surveiller (7 carved repairs read as straight bars); 7000 a surveiller (open mass south of the spawn at 70-140,120-200 kept under 0.95 by outcrops); 2700 defaut (merged hidden region of 1609 cells at 166-228,76-153, hidden floor 0.146 to 0.239); 2800 defaut (merged hidden region of 1225 cells at 28-86,125-209). The window rule counts hidden pocket floor as open, so it misses these areas; windows with network floor under 0.05: 2700 536 to 1099, 2800 526 to 791, 1700 526 to 940. Reported only, nothing changed. Repairs are marked on the after maps only (the before code cannot be rebuilt). Sheet (temporary): Saved/GeneratedLevels/task-539/task-539_before_after_sheet.png; table Saved/GeneratedLevels/task-539/review_task539.md.

## Verdict
Table: Saved/GeneratedLevels/task-539/review_task539.md; sheet: Saved/GeneratedLevels/task-539/task-539_before_after_sheet.png; pairs in png/ (all temporary). Scan scan539.js and extra539.js, independent of the generator code; validate_layout.js OK on all 14 maps. Places in cells (x, y), origin top-left. Repairs are marked on the after maps only: the before code cannot be rebuilt.
- A1: the 7 starts regenerated at HEAD give 1500, 1702, 1900, 2006, 2700, 2803, 7000 with the cell hashes of task-537/after; no mismatch.
- A2 (window, lane, farthest pocket, before -> after): 1500 0.073-0.927 -> 0.092-0.903, 25 -> 20, 10 -> 8; 1700 0.089-0.924 -> 0.063-0.873, 15 -> 24, 10 -> 7; 1900 0.099-0.915 -> 0.080-0.924, 24 -> 19, 9 -> 6; 2000 0.083-0.929 -> 0.137-0.920, 20 -> 28, 5 -> 8.
- 2700 0.083-0.929 -> 0.071-0.927, 24 -> 22, 8 -> 10; 2800 0.082-0.906 -> 0.076-0.925, 21 -> 23, 6 -> 9; 7000 0.080-0.880 -> 0.080-0.929, 20 -> 23, 6 -> 8. All within the rules, equal to the generator log.
- Extra measure: windows with under 0.05 of floor reachable from the spawn, typically 490 to 750 of about 6100; 1700 526 -> 940, 2700 536 -> 1099, 2800 526 -> 791. The window rule counts hidden pocket floor as open, so it misses these areas.

## A3 (previews)
- 1500 ok: balanced network, readable pockets; the carved corridor east of the spawn (145-170, 115-145) bends at sharp angles.
- 1700 a surveiller: east third (150-225, 60-200) holds six pockets side by side and the network barely enters it; straight carved bar at the top centre (117-140, 72-80) is the longest lane, 24.
- 1900 a surveiller: 7 carved repairs read as straight bars of even width: west (52-80, 144-150), east (168-195, 140-158), south (85-118, 186-202), north (135-145, 38-55).
- 2000 ok: lane of 28 on the west (62-70, 122-149), near the limit; open web in the north half, as before.
- 2700 defaut: east sector (166-228, 76-153) is one hidden branched region of 1609 cells (other pockets 135 to 600), so merged pockets read as a second hidden cave network; hidden walkable floor 0.146 -> 0.239; four outcrops ring the west spawn plaza.
- 2800 defaut: the south-west quadrant (28-86, 125-209) is one hidden branched region of 1225 cells next to three more pockets, with a thin network around it.
- 7000 a surveiller: the open mass south of the spawn (70-140, 120-200) stays under 0.95 only through outcrops; max 0.880 -> 0.929.
- Reported only (Q2): no change to the generator, params, briefs or committed levels.

## Relations
- [[task-539-ld-review-pocket-guard-maps]] — parent global task.
- [[layout-review-rules]] — the rules reviewed.
