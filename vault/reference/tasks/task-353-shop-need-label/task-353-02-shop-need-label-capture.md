---
id: task-353-02-shop-need-label-capture
description: "Cpp: -game capture at 1920 x 1080 of the fix of owner point 3, compared with the look before and the mockup."
parent: task-353-shop-need-label
kind: task
status: in-progress
---
# Shop card: NEED N MORE overflows the Buy plate: capture

- goal: -game at 1920 x 1080, same procedure as task-348-08: shop frames at 1920 x 1080 with at least one affordable and one short card (low balance through rr.Cheat.GrantOre / rr.Shop.Buy), crops of the Buy plates, compared with srd_23 (before) and the mockup. Sent to the owner with the sheet. No code change unless a capture step needs it; then report it.
- agent: cpp
- depends_on: task-353-01-shop-need-label-fix
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), sys-run-shop (vault/reference/tech/systems/world/sys-run/sys-run-shop.md), wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md), task-348-08-shop-screen-capture (vault/reference/tasks/task-348-shop-screen-redesign/task-348-08-shop-screen-capture.md) (capture procedure, its run.log in `Saved/Captures/ShopRedesign/run.log`), `Saved/ShopRedesign/srd_00.png` and `Saved/ShopRedesign/srd_23.png` (look before), `Art/Source/shop-screen/reference/chatgpt-shop-mockup-2026-10-28.webp` (target look)
- expected_output: frames and a comparison sheet in `Saved/ShopFix/task-353/` (not committed); report of what is seen and unseen.
- done_when: the sheet shows: "NEED N MORE" inside the grey plate with visible side margins, smaller than BUY; BUY unchanged; or the report lists what differs; unseen items listed.
- editor_state: any (no build; task-348-08 ran -game with the editor open)
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-28T17:50:43Z
- waiting_for: none
- result: open

## Relations
- [[task-353-shop-need-label]] — parent global task.
