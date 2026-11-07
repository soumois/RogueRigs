---
id: task-495-02-ld-review
description: "Level-design: dev-time re-review of the 24 seed starts of task-492-03 against all layout-review-rules, compared with review_cpp.md."
parent: task-495-generator-window-repair
kind: task
status: in-progress
---
# C++ generator enforces the layout review rules: level-design

- goal: Dev-time level-design re-review of the 24 seed starts of task-492-03 against all layout-review-rules, compared with review_cpp.md; an independent check of the generator, not part of the game.
- agent: level-design
- depends_on: task-495-01-repair-pass
- inputs: layout-review-rules (vault/reference/tech/pipelines/layout-review-rules.md), task-492-03-ld-validation (vault/reference/tasks/task-492-cpp-layout-generator/task-492-03-ld-validation.md), Saved/GeneratedLevels/task-492/review_cpp.md
- expected_output: see done_when
- done_when: A new table next to review_cpp.md gives per start: accepted seed, validator result, min and max window, straight lane flag, pocket distance, verdict; 24/24 meet the window rule, the straight-lane rule (30 cells, ±1 tolerance) and the pocket distance rule (15 cells), measured independently of the generator report, and pass the validator; A4 judged on the downscaled previews with any unnatural repaired area named.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-07T07:52:59Z
- waiting_for: none
- result: open

## Verdict
Table: Saved/GeneratedLevels/task-492/review_task495.md (next to review_cpp.md, temporary). Measured by Saved/GeneratedLevels/task-495/review495.js, a Node scan independent of the generator (same scan as review_cpp.md, thresholds of [[layout-review-rules]]), plus validate_layout.js with brief_240.
- Per start (accepted seed, window min-max, longest lane, farthest pocket), all validator OK and kept: 1000 (1004) 0.099-0.884, 22, 9; 1100 (1100) 0.075-0.910, 18, 10; 1200 (1200) 0.095-0.927, 29, 7; 1300 (1303) 0.094-0.925, 18, 7; 1400 (1401) 0.085-0.918, 18, 8; 1500 (1502) 0.073-0.927, 25, 10.
- 1600 (1600) 0.109-0.901, 19, 5; 1700 (1705) 0.089-0.924, 15, 10; 1800 (1818) 0.085-0.925, 23, 10; 1900 (1901) 0.099-0.915, 24, 9; 2000 (2000) 0.083-0.929, 20, 5; 2100 (2101) 0.075-0.920, 17, 10.
- 2200 (2200) 0.075-0.922, 21, 7; 2300 (2300) 0.076-0.913, 14, 7; 2400 (2403) 0.073-0.915, 29, 8; 2500 (2505) 0.075-0.929, 22, 6; 2600 (2600) 0.125-0.913, 29, 5; 2700 (2702) 0.083-0.929, 24, 8.
- 2800 (2806) 0.082-0.906, 21, 6; 2900 (2903) 0.075-0.905, 24, 9; 6450 (6453) 0.115-0.906, 17, 8; 7000 (7002) 0.080-0.880, 20, 6; 7900 (7900) 0.094-0.917, 19, 7; 7950 (7950) 0.073-0.918, 24, 6.
- Kept 24 of 24 (review_cpp.md: 4 of 24); mean window min 0.087 (0.026), mean max 0.915 (0.956); lanes over 30: 0 (3); farthest pocket 10 (11). Values equal to the generator report.

## A4 (previews)
Previews in Saved/GeneratedLevels/task-495/png/ (cyan carved, magenta added rock against the same seed without repairs; _plain without marks). Carved areas read as filler corridors and side stubs; lane fixes are small wall bumps. First review: the too-open window outcrops were identical round pillars, lined up along the central plaza of 2505: fixed (commit f7cfc4b7, 1 to 3 overlapping discs, random direction and centre), rechecked: they now read as the irregular rock islands of the plazas. Least natural area left: 2505, four rounded outcrops in a loose column along its narrow central plaza. No straight cut, no square block.

## Relations
- [[task-495-generator-window-repair]] — parent global task.
- [[layout-review-rules]] — the rules reviewed.
