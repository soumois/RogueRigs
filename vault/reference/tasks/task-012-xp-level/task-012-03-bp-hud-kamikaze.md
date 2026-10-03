---
id: task-012-03-bp-hud-kamikaze
description: "Bigger kamikaze visuals, HUD XP bar with level and LEVEL UP text, HUD divide-by-zero check, PIE check."
parent: task-012-xp-level
kind: task
status: in-progress
---
# HUD XP and kamikaze visuals

- goal: XP and levels visible; the kamikaze looks as big as it is.
- agent: bp
- depends_on: task-012-02-progression
- inputs: [[sys-hud]], `sys-progression`, [[sys-enemies]], [[bp-kamikaze]], [[wbp-hud]], [[xp-level-up]], [[color-code]], [[unreal-mcp]].
- expected_output: `BP_Kamikaze`: body, core and eye scaled to the new 1.1 × capsule (read the capsule size from the C++ default). `WBP_HUD`: a cyan XP ProgressBar across the bottom of the screen, a "Lv N" TextBlock at its left, a centered "LEVEL UP" TextBlock shown for 1 s on `OnLevelUp` (a widget animation or a timer in the graph), implement `OnXPChanged` (percent = Current / Needed, guarded for Needed 0) and `OnLevelUp`. Doc nodes [[wbp-hud]], [[bp-kamikaze]] updated. Save all dirty packages; check `git status`.
- done_when: assets read back; PIE starts with no divide-by-zero or other gameplay warning in the log; kamikazes look larger than the miner; files on disk; lint clean.
- editor_state: open
- token_ceiling: 60000
- milestone: M1
- result: open

## Relations
- [[task-012-xp-level]] — parent global task.
