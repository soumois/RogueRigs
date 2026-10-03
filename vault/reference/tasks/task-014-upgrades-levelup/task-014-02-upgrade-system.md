---
id: task-014-02-upgrade-system
description: "Upgrade value CSV, luck-weighted rarity draw, 3-card offer limited to owned weapons, GAS application, one reroll per screen, tests."
parent: task-014-upgrades-levelup
kind: task
status: todo
---
# Upgrade system

- goal: the game can build a 3-card offer and apply the picked upgrade.
- agent: cpp
- depends_on: task-014-01-spawn-packs
- inputs: [[upgrades]], [[rarities]], [[player-stats]], [[burst-rifle]], [[sys-upgrades]], [[sys-gas-core]], [[sys-weapons]], [[sys-auto-fire]], [[gas-api-notes]], [[sys-automation-tests]], [[cpp-standard]].
- expected_output: `Content/RogueRigs/Data/Tables/UpgradeValues.csv` (stat, tier, global value, operation; targeted = 1.5 × global, values from [[upgrades]]); rarity weights and k as data. Offer builder (e.g. `URRUpgradeSubsystem` or a component on the miner): `BuildOffer(3, Luck)` draws distinct (stat, target, tier) cards from player stats (max health, move speed, pickup radius, XP gain, mining speed, luck) and weapon stats (damage, fire rate, reload speed, crit chance, crit damage) with targets "all weapons" or one owned weapon only; no magazine, no effect cards. `ApplyUpgrade(Card)` through SetByCaller GEs on the player ASC or weapon ASCs (global broadcast and replay on acquisition per [[sys-weapons]]); picked list kept. `Reroll()` once per offer, returns 3 cards different from the current ones. The card struct exposes display name (French UI text), value text and tier for the UI. [[sys-upgrades]] filled, [[sys-gas-core]] if attributes added. Tests `RogueRigs.Upgrades.*`: value lookup per tier and targeted ×1.5; rarity distribution near the weights for luck 0 and shifted up for luck 10 (seeded); never an unowned weapon or a magazine card; damage +12 % applies to the rifle; max health +20 raises max and current health; percentage stacking adds; reroll once only and gives different cards.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 120000
- milestone: M1
- result: open

## Relations
- [[task-014-upgrades-levelup]] — parent global task.
