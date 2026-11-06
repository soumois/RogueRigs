---
id: task-458-02-slots
description: "Bp: The weapon slots take the retained frame."
parent: task-458-weapon-slots-style
kind: task
status: done
---
# Weapon slots in the unified look: bp

- goal: The weapon slots take the retained frame.
- agent: bp
- depends_on: task-458-01-art
- inputs: wbp-weapons (vault/reference/tech/blueprints/ui/wbp-weapons.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), Art/Source/hud-unified/
- expected_output: see done_when
- done_when: Compiled and saved; slots, reload bar and padlocks still driven; PIE shows no runtime error.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-458
- usage: included in the executor run of task-458
- runs: 1
- milestone: M2
- started: 2026-11-06T01:14:14Z
- waiting_for: none
- result: WBP_Weapons (commit 5c275436): Frame0..3 brush = T_LevelEnd_Frame Draw As Box (margins 0.0955/0.1878, ImageSize 188 x 92 sets the corner scale, white tint), replacing the dark rounded box with gold outline; SlotsRow wrapped in Overlay SlotsBlock whose first child SlotsBase (Image, T_Shop_ButtonReroll Box brush same as the health plate, HitTestInvisible, padding -17/-6 so it adds no layout height) is the bolted base. Icons, names, ammo/reload bars, block rows, padlocks, graphs and Frame0..3 variables (card flight targets) unchanged. Compiled, saved; PIE on L_ArenaM1 started and stopped with no error or Accessed None. Kept as is (start from the asset): root VerticalBox and the 150 x 90 Col SizeBoxes of an auto-sized sub-widget anchored in WBP_HUD; no new fixed size added. No capture (Q26): look unverified until the owner sees it in game.

## Relations
- [[task-458-weapon-slots-style]] — parent global task.
