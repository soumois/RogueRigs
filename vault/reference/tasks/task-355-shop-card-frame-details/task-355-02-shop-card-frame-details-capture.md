---
id: task-355-02-shop-card-frame-details-capture
description: "Cpp: -game captures at four resolutions of the fix of owner point 5, compared with the look before and the mockup."
parent: task-355-shop-card-frame-details
kind: task
status: in-progress
---
# Shop card: second frame line, cramped tier tab, sold panel fit: capture

- goal: -game, same procedure as task-356-03: shop frames for this fix at 1920 x 1080, 1280 x 720, 1920 x 1200 and 2560 x 1080 (windowed), plus the states not yet seen (Q6): uncommon, epic and legendary tabs (rerolls with granted ore, at most 30), hover and pressed tints on Buy, Reroll and Continue (OS cursor moved by a PowerShell SetCursorPos / mouse_event script during the -game run; pressed = button held, cursor moved off before release so nothing is clicked), Reroll greyed with a balance below the reroll cost, and the burn of one purchase at 0.05 s steps; compared with the task-357 frames (before) and the mockup. Sent to the owner with the sheet. No code change unless a capture step needs it; then report it.
- agent: cpp
- depends_on: task-355-01-shop-card-frame-details-fix
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), sys-run-shop (vault/reference/tech/systems/world/sys-run/sys-run-shop.md), wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md), task-348-08-shop-screen-capture (vault/reference/tasks/task-348-shop-screen-redesign/task-348-08-shop-screen-capture.md) (capture procedure, its run.log in `Saved/Captures/ShopRedesign/run.log`), `Saved/ShopFix/task-357/` frames (look before), task-356-03-shop-anchors-capture (vault/reference/tasks/task-356-shop-screen-anchors/task-356-03-shop-anchors-capture.md) (multi-resolution procedure), `Art/Source/shop-screen/reference/chatgpt-shop-mockup-2026-10-28.webp` (target look), m-card-burn (vault/reference/tech/blueprints/ui/m-card-burn.md)
- expected_output: frames and a comparison sheet in `Saved/ShopFix/task-355/` (not committed); report of what is seen and unseen.
- done_when: the sheet shows: no second frame line beside slot 0; tab texts with margins; sold panel inside the frame opening at every resolution; every state of Q6 shown or listed as unseen with the reason; or the report lists what differs; unseen items listed.
- editor_state: any (no build; task-348-08 ran -game with the editor open)
- token_ceiling: 1.5 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-28T21:51:38Z
- waiting_for: none
- result: open

## Relations
- [[task-355-shop-card-frame-details]] — parent global task.
