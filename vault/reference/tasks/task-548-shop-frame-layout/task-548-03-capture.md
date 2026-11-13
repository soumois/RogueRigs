---
id: task-548-03-capture
description: "Capture: Shop screen captures against image 8 at four resolutions"
parent: task-548-shop-frame-layout
kind: task
status: todo
---
# Spare Parts shop: own panel frame, layout as owner image 8: capture

- goal: Shop screen captures against image 8 at four resolutions
- agent: capture
- depends_on: task-548-02-shop-layout
- inputs: umg-layout (vault/reference/tech/conventions/umg-layout.md), Saved/Scratch/owner-ui-batch-2026-11-13/8.webp
- expected_output: see done_when
- done_when: C1 panel frame corners and side pieces not stretched at each resolution; C2 inner background dark and uniform; C3 cards with margin to the frame, proportions close to image 8; C4 a greyed card and a SOLD slot still correct; C5 REROLL and CONTINUE inside the screen.
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
- [[task-548-shop-frame-layout]] — parent global task.
