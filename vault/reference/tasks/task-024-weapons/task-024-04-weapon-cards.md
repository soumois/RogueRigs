---
id: task-024-04-weapon-cards
description: "Start with the rifle only; new-weapon cards join the level-up offer while a slot is free (4 slots); picking equips it; tests."
parent: task-024-weapons
kind: task
status: done
---
# Weapon cards

- goal: weapons are acquired as level-up choices.
- agent: cpp
- depends_on: task-024-03-turret
- inputs: [[upgrades]], [[miner]], [[sys-upgrades]], [[sys-level-up]], [[sys-weapons]], [[cpp-standard]], [[sys-automation-tests]], [[drgs-analysis]]
- expected_output: miner starts with the rifle only (GuaranteedWeaponClasses emptied; BP_Miner StartingWeapons [BP_BurstRifle] set by the bp agent: say so); weapon card type in FRRUpgradeCard (French text "Nouvelle arme", weapon name and short description; rarity shown as a dedicated style, not a tier); offer rule: while owned weapons < 4 and unowned weapons exist, each card slot has a 30 % chance to be a new-weapon card, at most one per offer; picking equips the weapon (replay of global upgrades) and adds its targeted cards; weapon list from data (rifle, shotgun, turret launcher); docs sys-upgrades, sys-weapons. Tests `RogueRigs.Upgrades.WeaponCards*`.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 110000
- model: opus
- tokens: input 94, cache write 151886, cache read 4094408, output 67840
- runs: 1
- milestone: M2
- result: FRRUpgradeCard card type (stat or new weapon) with weapon class and description; offer rule 30 % per slot, at most one per offer, only while a catalogue weapon can be acquired (no RNG draw otherwise, seeded offers unchanged); reroll never repeats the weapon; pick calls ARRMiner::AddWeapon (global replay), refused if owned or 4 owned. ARRMiner WeaponCatalogue, MaxWeapons 4, GuaranteedWeaponClasses empty; NewWeaponCardChance 0.3, MaxNewWeaponCardsPerOffer 1 (provisional). Test harness M1TestLoadout adds the shotgun explicitly for old tests. 4 WeaponCards tests; build 0, 137/137. Docs sys-upgrades, sys-weapons, sys-player, sys-automation-tests. Commits 0a06a3e, 705afe8.

## Relations
- [[task-024-weapons]] — parent global task.
