---
id: task-492-03-ld-validation
description: "Level-design: Level-Design agent generates 20 passing seeds with the C++ generator and reviews them with layout-review-rules."
parent: task-492-cpp-layout-generator
kind: task
status: done
---
# C++ layout generator writing the grid JSON, validated by the Level-Design agent: level-design

- goal: Level-Design agent generates 20 passing seeds with the C++ generator and reviews them with layout-review-rules.
- agent: level-design
- depends_on: task-492-02-gen-place-json
- inputs: agent-level-design (vault/reference/agents/agent-level-design.md), layout-review-rules (vault/reference/tech/pipelines/layout-review-rules.md), layout-generator-recipe (vault/reference/tech/pipelines/layout-generator-recipe.md), level-layout-schema (vault/reference/tech/pipelines/level-layout-schema.md)
- expected_output: Generated files and the review table go to Saved/GeneratedLevels/task-492/ (temporary, not committed); the verdict (per-seed window min and max, flags, kept seeds, comparison with the Node generator) is written in this subtask node.
- done_when: A4 of the global holds: 20 seeds validated by validate_layout.js, verdict per seed written, defects found fixed in the generator and re-checked, remaining deviations listed for the owner; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-492
- usage: included in the executor run of task-492
- runs: 1
- milestone: M2
- started: 2026-11-07T06:27:26Z
- waiting_for: none
- result: 24 passing C++ seeds (first 20 starts 1000-2900 step 100, plus starts 6450, 7000, 7900, 7950), all validate_layout.js OK with the 240 brief (passage 3). Review per layout-review-rules: 4 kept (6453, 7002, 7900, 7950: every window in 0.05-0.95), 20 rejected for dead or too-open windows, 3 lanes over 30 flagged, no far pocket. Comparable to the Node generator: the Node script fed FRandomStream's LCG reproduces the C++ cells exactly; over 60 starts each, yield 4 vs 0 kept. No generator defect found. Deviations for the owner: low keep yield (unchanged from Node), some seeds about 3 s. Files and table in Saved/GeneratedLevels/task-492/ (temporary).

## Verdict
24 passing seeds of the C++ generator (brief 240 × 240, coverage 0.52–0.62, heal_ore 56, barrel 17–22, reach ≥ 0.6, passage 3), all `validate_layout.js` OK, minimum passage 3. Scan of [[layout-review-rules]]: 24 × 24 void-free windows, stride 2; dead < 0.04, too open > 0.95, lane > 30 cells (±1 wall tolerance), pocket gap > 15. Per seed: window min–max, flags, verdict (keep = every window 0.05–0.95, no far pocket).
- 1004: 0.000–0.884, dead, lane 31, reject. 1100: 0.019–0.939, dead, reject. 1200: 0.031–1.000, dead, open, reject. 1303: 0.042–0.988, open, reject. 1401: 0.009–1.000, dead, open, reject. 1502: 0.014–0.983, dead, open, reject.
- 1600: 0.000–0.958, dead, open, reject. 1705: 0.031–0.977, dead, open, lane 31, reject. 1806: 0.000–1.000, dead, open, reject. 1901: 0.033–0.976, dead, open, reject. 2000: 0.014–0.929, dead, reject. 2101: 0.021–1.000, dead, open, reject.
- 2200: 0.000–0.969, dead, open, reject. 2300: 0.016–0.932, dead, reject. 2403: 0.000–0.995, dead, open, reject. 2505: 0.000–1.000, dead, open, reject. 2600: 0.031–0.913, dead, reject. 2702: 0.042–0.913, no flag, reject (min under 0.05).
- 2806: 0.038–0.938, dead, reject. 2901: 0.000–1.000, dead, open, lane 36, reject. 6453: 0.115–0.906, keep. 7002: 0.050–0.880, keep. 7900: 0.063–0.946, keep. 7950: 0.050–0.918, keep.
- Pockets: 6 to 9 per seed, largest gap 11 cells: no far pocket.
- Kept 4 of 24 (the first 20 starts gave 0; four more starts added). Yield matches the rules (arena_l4: 1 of 30).

## Comparison with the Node generator
- Same 20 starts with `arena_gen_fine.js`: 2 of 20 kept, mean window min 0.041 (C++ 0.017), 0 lanes over 30 (C++ 3).
- Cause: the random stream alone. The Node script with `FRandomStream`'s LCG and the full rock-island flood picks the same 24 seeds and writes the same cells as the C++ build (checked on 4 seeds). On 60 starts each, the Node LCG kept 0 (mean min 0.031) and the `FRandomStream` variant 4 (0.026). The two generators are comparable; no generator defect found.

## Deviations for the owner
- Most seeds still show a dead or too-open window, as before (Node yield 0–2 of 20): only about 1 seed in 6–10 keeps every window in 0.05–0.95. Picking a kept seed is not automated (Q3 default).
- Some seeds take about 3 s each (start 6450: 9.5 s for 4 seeds, start 7000: 12.5 s for 3), most 0.1–0.6 s.

## Relations
- [[task-492-cpp-layout-generator]] — parent global task.
- [[layout-review-rules]] — the rules applied.
