---
id: task-009-01-player-damage
description: "Enemy-to-player flat damage GE, 0.25 s i-frames with blink, single death with State.Dead and a death event, enemy attribute set."
parent: task-009-chaser
kind: task
status: in-progress
---
# Player damage, i-frames, death

- goal: the miner can be hurt, is briefly invulnerable after a hit, and dies exactly once.
- agent: cpp
- depends_on: none
- inputs: [[sys-damage]], [[sys-gas-core]], [[sys-player]], [[miner]], [[naming]], [[gas-api-notes]], [[gas-engine-facts]], [[sys-automation-tests]], [[cpp-standard]].
- expected_output: `URREnemyAttributeSet` (`AttackPower`, base 1). `URRGE_DamageFlat` (instant, SetByCaller tag `Data.Damage`, scaled by the source's `AttackPower` if present; refuses `State.Dead` and `State.Invulnerable`). `URRGE_IFrames` (duration from an editable value on the miner, default 0.25 s, grants `State.Invulnerable`) applied from the damage post-execute of a pawn that opts in (the miner), in the same call stack ([[sys-damage]] step 7). Single death: when `Health` goes from above 0 to 0, add `State.Dead` once and broadcast a death event (`OnDeath(Instigator)` on the health set or the owner); the miner stops input-driven movement and firing on death. Miner blink during i-frames: toggles mesh visibility at an editable rate (default 12 Hz), restored at the end. Native tags added to `RRGameplayTags`. Tests `RogueRigs.Player.*`: flat damage 10 lowers health to 90; a second hit within 0.25 s is refused, one after is applied; death fires once at 0 and further damage is refused. Docs [[sys-damage]], [[sys-gas-core]], [[sys-player]], [[sys-automation-tests]].
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs match; lint clean.
- editor_state: closed
- token_ceiling: 90000
- milestone: M1
- result: open

## Relations
- [[task-009-chaser]] — parent global task.
