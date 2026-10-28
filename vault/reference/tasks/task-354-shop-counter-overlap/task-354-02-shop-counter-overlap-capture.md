---
id: task-354-02-shop-counter-overlap-capture
description: "Cpp: -game capture at 1920 x 1080 of the fix of owner point 4, compared with the look before and the mockup."
parent: task-354-shop-counter-overlap
kind: task
status: cancelled
---
# Shop screen: crystal counter covers the shop panel corner: capture

- goal: -game at 1920 x 1080, same procedure as task-348-08: one shop frame at 1920 x 1080 and frames of one purchase (rr.Shop.Buy) to see the ore flight leave the moved counter, compared with srd_00 (before) and the mockup. Sent to the owner with the sheet. No code change unless a capture step needs it; then report it.
- agent: cpp
- depends_on: task-354-01-shop-counter-overlap-fix
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), sys-run-shop (vault/reference/tech/systems/world/sys-run/sys-run-shop.md), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md), task-348-08-shop-screen-capture (vault/reference/tasks/task-348-shop-screen-redesign/task-348-08-shop-screen-capture.md) (capture procedure, its run.log in `Saved/Captures/ShopRedesign/run.log`), `Saved/ShopRedesign/srd_00.png` and `Saved/ShopRedesign/srd_23.png` (look before), `Art/Source/shop-screen/reference/chatgpt-shop-mockup-2026-10-28.webp` (target look)
- expected_output: frames and a comparison sheet in `Saved/ShopFix/task-354/` (not committed); report of what is seen and unseen.
- done_when: the sheet shows: no overlap of the counter with the panel corner or the first rivet; the ore icons still leave the counter and the balance counts down; title and Continue still fully visible; or the report lists what differs; unseen items listed.
- editor_state: any (no build; task-348-08 ran -game with the editor open)
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-354-shop-counter-overlap]] — parent global task.
