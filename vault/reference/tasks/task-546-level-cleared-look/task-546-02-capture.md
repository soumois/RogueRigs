---
id: task-546-02-capture
description: "Capture: Stats screen captures against image 6 at four resolutions"
parent: task-546-level-cleared-look
kind: task
status: done
---
# Level cleared screen: dividers and bolts as owner image 6: capture

- goal: Stats screen captures against image 6 at four resolutions
- agent: capture
- depends_on: task-546-01-stats-layout
- inputs: umg-layout (vault/reference/tech/conventions/umg-layout.md), Saved/Scratch/owner-ui-batch-2026-11-13/6.webp
- expected_output: see done_when
- done_when: C1 vertical divider top on a bolt junction mid horizontal divider; C2 its bottom bolt on the bottom frame bar; C3 bolts round and unstretched at each resolution; C4 layout proportions close to image 6; C5 texts and NEXT LEVEL button unchanged.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-546
- usage: included in the executor run of task-546
- runs: 1
- milestone: M2
- started: 2026-11-14T03:55:23Z
- waiting_for: none
- result: 5 -game launches, sheet Saved/Captures/t546_c_sheet.png (1920x1080 L1 and L4, 1280x720, 1920x1200, 2560x1080, owner image 6), crops Saved/Captures/t546_c_bolt_crops.png. C1 seen: vertical divider top bolt on the junction mid horizontal divider, all frames. C2 seen: its bottom bolt on the bottom frame bar. C3 seen: bolts round and unstretched at the four resolutions and on level 4, H end bolts on the frame side bars. C4 seen: frame, title, subtitle, Total Duskite band, columns and button at image 6 fractions; dividers and bolts are the pause pieces (bar 16, bolt 34 slate), thinner than in image 6, as Q12 asked. C5 seen: texts, lists and NEXT LEVEL / CASH OUT unchanged. Pre-existing, not in scope: on level 4 the long subtitle still crosses the two subtitle lines (open defect since task-381). Owner image 6 kept as Art/Source/level-end-screen/reference/owner-level-end-target-2026-11-13.webp (Q17).

## Relations
- [[task-546-level-cleared-look]] — parent global task.
