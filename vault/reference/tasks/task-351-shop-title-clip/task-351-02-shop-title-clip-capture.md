---
id: task-351-02-shop-title-clip-capture
description: "Cpp: -game capture at 1920 x 1080 of the fix of owner point 1, compared with the look before and the mockup."
parent: task-351-shop-title-clip
kind: task
status: done
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
- model: opus
- tokens: input 32, cache write 33157, cache read 274554, output 6875
- usage: turns 13, context start 12k peak 33k, resumes 3 (message x3), 2 min, units 0.10 M
- runs: 1
- milestone: M2
- started: 2026-10-28T17:37:28Z
- waiting_for: none
- result: Capture 1920x1080 -game, frames in Saved/ShopFix/task-351/ (comparison_sheet.png). Seen (checked by Executor): title bars, SPARE PARTS and subtitle fully visible, first lit row y=31; column moved down 34 px; purchase fx 342-344 play (buy 0, balance 45). Continue plate now cut more at bottom (starts y=1025, ~30 px cut; was fully on screen 991-1077): task 352 defect, reported as seen. Counter unmoved, still touches panel corner (task 355). Unseen: epic/legendary tabs, hover, reroll disabled, other resolutions. ALERT resumed x3: no SendMessage was sent, the agent ran once; likely a usage-counting artefact.

## Relations
- [[task-351-shop-title-clip]] — parent global task.
