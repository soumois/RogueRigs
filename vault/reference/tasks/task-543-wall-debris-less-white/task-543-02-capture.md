---
id: task-543-02-capture
description: "Capture: Before/after capture of a wall block breaking"
parent: task-543-wall-debris-less-white
kind: task
status: done
---
# Wall destruction particles: rock colours instead of near white: capture

- goal: Before/after capture of a wall block breaking
- agent: capture
- depends_on: task-543-01-debris-colour
- inputs: Saved/Scratch/owner-ui-batch-2026-11-13/3.png
- expected_output: see done_when
- done_when: C1 debris visible at the breaking block; C2 debris lilac and violet, none near white; C3 no bloom halo around them; C4 amount and size comparable to the before frame.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-543
- usage: included in the executor run of task-543
- runs: 1
- milestone: M2
- started: 2026-11-14T00:14:54Z
- waiting_for: none
- result: Done. Launches: before 1 (Saved/Captures/task543-before), after 3 (task543-after, -after2 superseded by the fixes; task543-after3 final). Sheets: Saved/Captures/task543-after3/sheet_dig.png (drone break, before vs after), sheet_bar.png (barrel explosion). C1 seen: debris thrown at the breaking block (after3 dig_01, dig_02). C2 seen: lilac, mid violet and dark violet chips, none near white (before: white flakes). C3 seen: no bloom halo on them. C4 seen: amount and size comparable to the before frame. Note, not a fail: after a barrel explosion the rock debris reads dark violet with low contrast on the dark floor (exposure drops after the flash); the glowing pink pieces there are ore debris, unchanged (A3).

## Relations
- [[task-543-wall-debris-less-white]] — parent global task.
