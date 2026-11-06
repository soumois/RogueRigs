---
id: task-461-02-health-slots-panel
description: "Bp: The health bar and the four weapon slots sit inside one riveted panel."
parent: task-461-bottom-hud-panel
kind: task
status: done
---
# Bottom HUD built as one riveted panel and a framed XP bar: bp

- goal: The health bar and the four weapon slots sit inside one riveted panel.
- agent: bp
- depends_on: task-461-01-art
- inputs: wbp-weapons (vault/reference/tech/blueprints/ui/wbp-weapons.md), wbp-hud (vault/reference/tech/blueprints/ui/wbp-hud.md), task-452-health-above-weapons (vault/reference/tasks/task-452-health-above-weapons.md), task-458-weapon-slots-style (vault/reference/tasks/task-458-weapon-slots-style.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-05b.webp, Art/Source/hud-unified/reference/owner-hud-target-2026-11-05.webp
- expected_output: see done_when
- done_when: Compiled and saved with no error; one panel encloses health and slots, no per-slot frames or connectors left; health, slots and card flight still work in PIE with no runtime error.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-461
- usage: included in the executor run of task-461
- runs: 1
- milestone: M2
- started: 2026-11-06T04:32:07Z
- waiting_for: none
- result: WBP_HealthStatus HealthPanel is now the whole panel (760 x 192, anchor 0.5/1, top -42): HealthPlate = T_HUD_BottomPanel box (620 x 165, 0.095/0.28), new HealthWell (T_HUD_SlotWell) behind the bar, bar and text inset 100/26/100/142. WBP_Weapons: SlotsBase connector plate removed, Frame0..3 = T_HUD_SlotWell box (140 x 54, 0.14/0.28), turret padding 72; WBP_HUD Weapons top -78, Subtitles -290. Compiled, saved; PIE L_ArenaM1 GrantXP 3 and 40, two card picks: no Accessed None nor script error. Look not seen (A6: owner checks). Commit 22b01a16.

## Relations
- [[task-461-bottom-hud-panel]] — parent global task.
