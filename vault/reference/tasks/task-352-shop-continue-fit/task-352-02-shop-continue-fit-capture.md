---
id: task-352-02-shop-continue-fit-capture
description: "Cpp: -game capture at 1920 x 1080 of the fix of owner point 2, compared with the look before and the mockup."
parent: task-352-shop-continue-fit
kind: task
status: done
---
# Shop screen: Continue plate cut at the bottom: capture

- goal: -game at 1920 x 1080, same procedure as task-348-08: one shop frame at 1920 x 1080 showing the whole screen, compared with srd_00 (before) and the mockup. Sent to the owner with the sheet. No code change unless a capture step needs it; then report it.
- agent: cpp
- depends_on: task-352-01-shop-continue-fit-fix
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), sys-run-shop (vault/reference/tech/systems/world/sys-run/sys-run-shop.md), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md), task-348-08-shop-screen-capture (vault/reference/tasks/task-348-shop-screen-redesign/task-348-08-shop-screen-capture.md) (capture procedure, its run.log in `Saved/Captures/ShopRedesign/run.log`), `Saved/ShopRedesign/srd_00.png` and `Saved/ShopRedesign/srd_23.png` (look before), `Art/Source/shop-screen/reference/chatgpt-shop-mockup-2026-10-28.webp` (target look)
- expected_output: frames and a comparison sheet in `Saved/ShopFix/task-352/` (not committed); report of what is seen and unseen.
- done_when: the sheet shows: Continue and Reroll plates fully visible, Continue bottom at least the decided margin above y = 1080; title still fully visible; or the report lists what differs; unseen items listed.
- editor_state: any (no build; task-348-08 ran -game with the editor open)
- token_ceiling: 1.2 M units
- model: opus
- tokens: input 32, cache write 36545, cache read 355439, output 7628
- usage: turns 15, context start 12k peak 42k, resumes 1 (message x1), 2 min, units 0.12 M
- runs: 1
- milestone: M2
- started: 2026-10-28T17:43:29Z
- waiting_for: none
- result: -game 1920x1080 capture (Saved/ShopFix/task-352/comparison_sheet.png): Continue plate y 988-1059, 20 px above bottom (>=16 ok); Reroll y 877-953 whole; title 31-78 clear; 342-344 effects play. Seen: Reroll icon and the 5 of 15 sit on the plate end-caps, CONTINUE text touches end-caps. Unseen: hover/pressed, greyed Reroll, other tiers, other resolutions

## Relations
- [[task-352-shop-continue-fit]] — parent global task.
