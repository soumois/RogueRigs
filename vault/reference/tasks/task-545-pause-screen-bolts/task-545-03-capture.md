---
id: task-545-03-capture
description: "Capture: Pause screen captures against image 10 at four resolutions"
parent: task-545-pause-screen-bolts
kind: task
status: todo
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-545-pause-screen-bolts]] — parent global task.
