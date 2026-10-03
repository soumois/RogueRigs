---
id: task-012-02-progression
description: "URRProgressionComponent on the miner: XP and level, CSV curve 5 × level with carry-over, XP pickups × XPGain, events for the HUD, tests."
parent: task-012-xp-level
kind: task
status: todo
---
# XP and level progression

- goal: XP cubes fill levels.
- agent: cpp
- depends_on: task-012-01-tuning-shake
- inputs: [[xp-level-up]], [[player-stats]], [[sys-pickups]], [[sys-player]], [[sys-hud]], [[sys-gas-core]], [[sys-automation-tests]], [[cpp-standard]].
- expected_output: `Content/RogueRigs/Data/Tables/XPCurve.csv` (rows per level or a formula row; values 5 × level for levels 1 to 50, document). `URRProgressionComponent` (actor component on `ARRMiner`): `AddXP(float Raw)` multiplies by the miner's `XPGain` attribute, accumulates, levels up with carry-over (several levels at once possible); getters `GetLevel`, `GetXP`, `GetXPToNextLevel`; delegates `OnXPChanged(Current, Needed, Level)` and `OnLevelUp(NewLevel)` (once per level gained). XP pickups (`ERRPickupType::XP`) call `AddXP` with an editable value per pickup (default 1) on collection. `URRHUDWidget`: new BlueprintImplementableEvents `OnXPChanged(Current, Needed, Level)` and `OnLevelUp(NewLevel)`, bound to the miner's component, initial values pushed on bind; also guard `OnHealthChanged` so it is never called with Max 0 (fixes the HUD divide-by-zero at PIE start). New doc `sys-progression` (template `system.md`); [[sys-pickups]], [[sys-player]], [[sys-hud]] updated. Tests `RogueRigs.Progression.*`: 5 XP → level 2 with 0 carried; 17 XP from level 1 → level 3 with 2 carried, two level-up events; `XPGain` 2 doubles XP; collecting an XP pickup adds 1.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs match; lint clean.
- editor_state: closed
- token_ceiling: 80000
- milestone: M1
- result: open

## Relations
- [[task-012-xp-level]] — parent global task.
