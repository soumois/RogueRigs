---
id: task-357-02-shop-indicator-capture
description: "Cpp: -game captures of the shop resource indicator at four resolutions and three balance sizes, plus one purchase flight."
parent: task-357-shop-resource-indicator
kind: task
status: done
---
# Shop screen: resource indicator centred on its font: capture

- goal: -game, same procedure as task-356-03, at 1920 x 1080, 1280 x 720, 1920 x 1200 and 2560 x 1080 (windowed): one shop frame each; at 1920 x 1080 also balances of 1, 2 and 3 digits (ore cheat of sys-cheats) and frames of one purchase (rr.Shop.Buy); look at every image; crop of the indicator per frame and a comparison sheet with the task-356 frames. Sent to the owner with the sheet. No code change unless a capture step needs it; then report it.
- agent: cpp
- depends_on: task-357-01-shop-resource-indicator
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md), task-356-03-shop-anchors-capture (vault/reference/tasks/task-356-shop-screen-anchors/task-356-03-shop-anchors-capture.md) (procedure), global task-357-shop-resource-indicator `acceptance`
- expected_output: frames, crops and sheet in `Saved/ShopFix/task-357/` (not committed); report of what is seen and unseen.
- done_when: the sheet shows icon and value centred on each other inside the panel at every resolution and digit count; the ore flight leaves the icon and the balance counts down; or the report lists what differs; unseen items listed.
- editor_state: any (no build)
- token_ceiling: 1.5 M units
- model: opus
- tokens: input 58, cache write 66803, cache read 1086668, output 21664
- usage: turns 26, context start 12k peak 72k, resumes 4 (message x4), 7 min, units 0.30 M
- runs: 1
- milestone: M2
- started: 2026-10-28T21:40:15Z
- waiting_for: none
- result: Captures at 4 resolutions and 0/83/108 balances in Saved/ShopFix/task-357 (no commit, no code change). Viewed by Executor: icon and value on one line inside the plate everywhere, icon about 1.5-3.5 px lower than digit centres and both about 1-4 px above plate centre (not visible at normal size); group not horizontally centred by design (option a). Unseen: exact ore-flight start frame (count-down seen), 4-digit balance.

## Relations
- [[task-357-shop-resource-indicator]] — parent global task.
