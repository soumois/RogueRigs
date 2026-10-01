---
id: task-004-02-gas-core
description: "Write the custom ASC, the health, movement and player attribute sets, and the runtime CSV loader for player base stats."
parent: task-004-miner-gas
kind: task
status: in-progress
---
# GAS core for the player

- goal: the GAS building blocks the miner needs, with base values read from data.
- agent: cpp
- depends_on: task-004-01-enable-gas
- inputs: [[sys-gas-core]], [[gas-engine-facts]], [[gas-api-notes]], [[player-stats]], [[naming]], [[cpp-standard]].
- expected_output: under `Source/RogueRigs/GAS/`: `URRAbilitySystemComponent`; `URRHealthAttributeSet` (`Health`, `MaxHealth`, meta `IncomingDamage`), `URRMovementAttributeSet` (`MoveSpeed`), `URRPlayerAttributeSet` (`PickupRadius`, `XPGain`, `MiningSpeed`, `Luck`), with the clamps listed in [[sys-gas-core]] gotchas; `URRGameplayAbility` base (InstancedPerActor). A row struct and loader that read `Content/RogueRigs/Data/Tables/PlayerBaseStats.csv` at runtime into a DataTable (approved approach in [[sys-gas-core]]) and apply the row as base values on an ASC. Write that CSV with the base values of [[player-stats]] (move speed 600, pickup radius 300, max health 100, XP gain 1, mining speed 1, luck 0). The CSV folder must be packaged as a non-asset directory (report; config change not allowed here). [[sys-gas-core]] updated to match.
- done_when: build exits 0 with the editor closed; no new warnings in touched files; [[sys-gas-core]] matches; lint clean.
- editor_state: closed
- token_ceiling: 80000
- milestone: M1
- result: open

## Relations
- [[task-004-miner-gas]] — parent global task.
