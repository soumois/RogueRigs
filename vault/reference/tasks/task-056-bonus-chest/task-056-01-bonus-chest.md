---
id: task-056-01-bonus-chest
description: "Bonus chest: one per level at 60 % alert, 8 s hack, XP, ore and a lucky upgrade choice."
parent: task-056-bonus-chest
kind: task
status: done
---
# Bonus chest

- goal: Bonus chest: one per level at 60 % alert, 8 s hack, XP, ore and a lucky upgrade choice.
- agent: cpp
- depends_on: none
- inputs: objective-bonus-chest (vault/reference/gdd/run/objective-bonus-chest.md), objective-console (vault/reference/gdd/run/objective-console.md), level-flow (vault/reference/gdd/run/level-flow.md), rarities (vault/reference/gdd/weapons/rarities.md), sys-objectives (vault/reference/tech/systems/sys-objectives.md), sys-upgrades (vault/reference/tech/systems/sys-upgrades.md), sys-pickups (vault/reference/tech/systems/sys-pickups.md), sys-hud (vault/reference/tech/systems/sys-hud.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: ARRBonusChest (console-like hack actor: 8 s, zone radius 6 cells, drains when the miner leaves, no marked rocks), dropped by the run flow once per level when the alert reaches 60 % (does not pause the alert), 30–45 cells away on a free reachable 2 × 2 footprint; on hack: XP = 50 % of the current level need, 5 ore chunks popped, one upgrade offer drawn with +3 luck for that draw only; HUD alert-bar event entry for the chest (new type, position 0.6, done when hacked); class properties editable; Blueprint hooks listed for task-056-02 (exact names); update only existing tests this breaks; docs sys-objectives, sys-upgrades, sys-hud.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 90000
- model: opus
- tokens: input 104, cache write 132430, cache read 4868945, output 41548
- runs: 1
- milestone: M2
- result: ARRBonusChest subclass of ARRConsole (8 s, zone 6 cells, drain 0.05/s, no marked rocks, 2 × 2); run flow drops one per level at alert 0.6, 30–45 cells, not after extraction is ready (shared FindObjectiveCell); reward once: 0.5 × level need XP via AddXP (XP gain stat applies), 5 ore pickups, URRLevelUpComponent::QueueBonusChoice(3) with luck +3 and no weapon card; HUD alert entry BonusChest at 0.6. HUD and pressure tests updated. Build 0, 195/195. Commits 3a45c35, 1e99320, ecf7714.

## Relations
- [[task-056-bonus-chest]] — parent global task.
