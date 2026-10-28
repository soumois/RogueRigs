---
id: task-351-02-shop-title-clip-capture
description: "Cpp: -game capture at 1920 x 1080 of the fix of owner point 1, compared with the look before and the mockup."
parent: task-351-shop-title-clip
kind: task
status: in-progress
---
# Shop screen: title clipped at the top: capture

- goal: -game at 1920 x 1080, same procedure as task-348-08: one shop frame at 1920 x 1080 showing the title row and subtitle, compared side by side with srd_00 (before) and the mockup. Sent to the owner with the sheet. No code change unless a capture step needs it; then report it.
- agent: cpp
- depends_on: task-351-01-shop-title-clip-fix
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), sys-run-shop (vault/reference/tech/systems/world/sys-run/sys-run-shop.md), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md), task-348-08-shop-screen-capture (vault/reference/tasks/task-348-shop-screen-redesign/task-348-08-shop-screen-capture.md) (capture procedure, its run.log in `Saved/Captures/ShopRedesign/run.log`), `Saved/ShopRedesign/srd_00.png` and `Saved/ShopRedesign/srd_23.png` (look before), `Art/Source/shop-screen/reference/chatgpt-shop-mockup-2026-10-28.webp` (target look)
- expected_output: frames and a comparison sheet in `Saved/ShopFix/task-351/` (not committed); report of what is seen and unseen.
- done_when: the sheet shows: title bars, title and subtitle fully visible with the decided top margin; nothing else moved off screen (Continue state reported as seen); or the report lists what differs; unseen items listed.
- editor_state: any (no build; task-348-08 ran -game with the editor open)
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-28T17:37:28Z
- waiting_for: none
- result: open

## Relations
- [[task-351-shop-title-clip]] — parent global task.
