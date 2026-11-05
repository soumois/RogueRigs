---
id: task-441-02-text-asset
description: "Bp: The text renders as gold "LEVEL UP" with the pop and no arc."
parent: task-441-level-up-effect
kind: task
status: done
---
# Level-up effect rework: bp

- goal: The text renders as gold "LEVEL UP" with the pop and no arc.
- agent: bp
- depends_on: task-441-01-level-up-text
- inputs: bp-damage-numbers (vault/reference/tech/blueprints/fx/bp-damage-numbers.md), color-code (vault/reference/gdd/art-audio/color-code.md)
- expected_output: see done_when
- done_when: Asset compiles with no error, letters read back as available to the render path, gold value set, normal numbers still use their arc.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-441
- usage: included in the executor run of task-441
- runs: 1
- milestone: M2
- started: 2026-11-05T20:10:26Z
- waiting_for: none
- result: New one-shot NS_LevelUpText (1 CPU sprite, 640 x 80 uu, life 0.7 s, velocity (0,0,100), no gravity, pop 1.15 to 1 over 0.25 s, fade last 45 %, gold (1, 0.72, 0.15)), M_LevelUpText, T_LevelUpText 512 x 64 from Art/Source/level-up-text/T_LevelUpText.png. Compile UpToDate, 0 errors; set on BP_Miner LevelUp. NS_DamageNumbers untouched, its arc stays.

## Relations
- [[task-441-level-up-effect]] — parent global task.
