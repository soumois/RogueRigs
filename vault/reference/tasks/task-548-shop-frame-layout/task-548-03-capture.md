---
id: task-548-03-capture
description: "Capture: Shop screen captures against image 8 at four resolutions"
parent: task-548-shop-frame-layout
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-548
- usage: included in the executor run of task-548
- runs: 1
- milestone: M2
- started: 2026-11-14T04:33:54Z
- waiting_for: none
- result: -game captures (not committed): 1920x1080 Saved/Captures/task548_b1080/shop_00.png (and task548_a1080 for NEED 15 MORE), 1280x720, 1920x1200, 2560x1080 in Saved/Captures/task548_c<res>/shop_00.png; before Saved/Captures/task548_before/shop_00.png; sheet with owner image 8 Saved/Captures/task548_sheet/comparison_sheet.png. C1 seen: corners, bolts, patch plates, side clamps and rivets round and unstretched at all four; only the plain bar piece stretches (longer horizontal rust streaks at 2560x1080). C2 seen: interior near black and uniform at all four. C3 seen: cards with a clear margin to the frame (about 45-60 px at 1080p), panel 46-1874 x 147-838 close to image 8 (46-1872 x 149-833); cards 278 x 400 vs about 310 x 417 in image 8 (card ratio kept), so gaps between cards wider than image 8; 2560x1080 keeps the known wide gaps. C4 seen: SOLD slot (wrench, NO REFUNDS.) and greyed NEED n MORE cards (15, 30, 75) at all four, BUY on affordable cards. C5 seen: Reroll and Continue fully on screen at all four (at 2560x1080 their plates are wider and Reroll greyed at balance 10, both unchanged by this task). 3 launches in this subtask plus 3 in task-548-02.

## Relations
- [[task-548-shop-frame-layout]] — parent global task.
