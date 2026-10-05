---
id: task-022-03-tests-integration
description: "Integration tests with the final Blueprints: heavy telegraphs and slams the miner, resists the shotgun knockback, crosses a corridor, placed spawner mix and cap."
parent: task-022-heavy
kind: task
status: done
---
# Heavy integration tests

- goal: the heavy is covered with the real Blueprints.
- agent: cpp
- depends_on: task-022-02-bp-heavy
- inputs: [[sys-automation-tests]], task-022-02 result.
- expected_output: `RogueRigs.Integration.*` tests with BP_Heavy, BP_Miner and the arena: telegraph then slam timing and damage, no slam damage outside 220 uu, shotgun knockback reduced by 0.8, one-cell corridor crossing, placed L_ArenaM1 spawner effective weights 65/12/15/8 and heavy cap 5; full run.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 70000
- model: opus
- tokens: input 64, cache write 127129, cache read 2800810, output 30437
- runs: 2
- milestone: M3
- result: Integration tests with real BP_Heavy: HeavyTelegraphSlamsMiner (telegraph at 151 uu, slam 0.8 s later, 2.5 s apart, 25 per slam on a vulnerable miner 100 → 50, tilt 14°, ring rendered and scaled to 220/50), HeavySlamRadius (hit at 210 uu, none at 230), HeavyResistsShotgunKnockback (600 vs 120 uu/s), HeavyCrossesCorridor; PlacedSpawnerMix updated (saved map 65/12/15/8, heavy cap 5). First run caught a real defect: ShockRing Hidden In Game, never rendered → sent back to the bp agent (fix 5727545), rerun 122/122. Commits feb3cda, 3a94b4e.

## Relations
- [[task-022-heavy]] — parent global task.
