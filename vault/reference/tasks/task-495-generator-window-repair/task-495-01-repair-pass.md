---
id: task-495-01-repair-pass
description: "Cpp: repair passes and seed-acceptance checks in FRRLayoutGenerator for windows, straight lanes and pocket distance, keeping validator constraints and determinism."
parent: task-495-generator-window-repair
kind: task
status: done
---
# C++ generator enforces the layout review rules: cpp

- goal: Repair passes and seed-acceptance checks in FRRLayoutGenerator for the three review rules, keeping every recipe and validator constraint and determinism. Windows: carve into dead windows, add rock into too-open ones. Straight lanes over 30 cells: break them (for example a wall outcrop) where feasible. Pocket distance over 15: acceptance check (repair if feasible). A layout still breaking a rule after repair fails the seed and the search moves on (Q2).
- agent: cpp
- depends_on: none
- inputs: sys-layout-generator (vault/reference/tech/systems/world/sys-grid/sys-layout-generator.md), layout-generator-recipe (vault/reference/tech/pipelines/layout-generator-recipe.md), layout-review-rules (vault/reference/tech/pipelines/layout-review-rules.md), level-layout-schema (vault/reference/tech/pipelines/level-layout-schema.md), Saved/GeneratedLevels/task-492/review_cpp.md
- expected_output: see done_when
- done_when: A1, A1b, A2, A3, A5 and A6 met on the 24 starts of review_cpp.md, measured by the Executor (window min and max, longest straight lane and farthest pocket per layout, validator output, two-run hash, retries and times) and written in the subtask result; carved floor joins the main network (no new isolated region) and added rock keeps passage 3; the subtask result says which rules are repaired and which only checked.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-495
- usage: included in the executor run of task-495
- runs: 1
- milestone: M2
- started: 2026-11-07T07:13:43Z
- waiting_for: none
- result: Commit c34eaacd. Repaired: windows (under 0.07 open: a filler walk from the nearest main-network cell into the deepest rock of the window; over 0.93: a noisy rock outcrop at the most open cell, else a bump on a wall of the window) and straight lanes over 30 (wall bump when the lane keeps 3 open cells, else a wall niche, at its middle or a third). Only checked: pocket distance (RefillRegions already refills regions with gap over max_gap 9, so it cannot pass 10). All three are seed-acceptance checks (fails window, lane, pocket distance); repair params in the new review section of fine_default.json. Measured on the 24 starts of review_cpp.md (brief_240): 24/24 accepted; window min 0.073 to 0.125, max 0.880 to 0.929 (all in 0.05-0.95); longest lane 14 to 29; farthest pocket 5 to 11 (limit 15); validate_layout.js OK 24/24, passage 3, coverage 52.8-58.0 %, reach 75.4-85.8 %, maxd 14-15, heal_ore 56, barrels 17-22, pockets 6-9; carved floor all joins the main network (unreachable regions identical in count to the same seed without repairs, none under 135 cells); rock additions keep passage 3, reach and maxd by construction (checked per stamp). A3: starts 1000, 2900, 6450 byte-identical on two runs. A5: 21/24 starts accept the same first seed as task-492-03; 1800 (1806 to 1818), 2400 (2403 to 2409), 2900 (2901 to 2903) moved because carving pushed reach just over 0.86; mean seeds tried 3.7 vs 2.9; accepted-seed generation 0.08-0.43 s (mean 0.16); repairs add no measurable time (seed 7000: 5.73 s vs 5.77 s without, the slow seeds come from the filler, unchanged). A6: RogueRigs.LayoutLoader 3/3 pass; Node arena_gen_fine.js still runs with the params file; committed layouts untouched. First run found two defects, both fixed before this result: a 0.958 window over the spawn square (no room for a free outcrop: wall-bump fallback added) and about 24 s seeds (filler rerun after each repair pass: removed, rock stamps now refuse new far cells). Files: Saved/GeneratedLevels/task-495/ (temporary). Figures superseded after the A4 outcrop correction of task-495-02 (commit f7cfc4b7, all checks rerun): 24/24, windows 0.073-0.929, lanes 14-29, pocket 10, 22/24 same first seed (1800 and 2900 moved, reach), mean 3.5 seeds, determinism and LayoutLoader tests pass. ALERT executor run over the standard budget at this closing (56 turns, 1.56 M units for 1.5 M): cause, reading the 1363-line generator, about 700 lines of new C++ with two builds, and two full 24-start commandlet runs (one correction round after the first).

## Relations
- [[task-495-generator-window-repair]] — parent global task.
