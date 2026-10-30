---
id: task-393-01-miner-material-scale
description: "Bp: read M_Miner, add MI_Miner with BaseColorScale 0.6 and assign it to the miner"
parent: task-393-miner-over-lit
kind: task
status: done
---
# Miner too bright in the arena: bp

- goal: Read M_Miner (/Game/RogueRigs/Characters/Miner/M_Miner: nodes, roughness, specular, metallic, whether T_Miner_Normal is wired) and the mean value of T_Miner_BaseColor, and report them. Add a scalar parameter BaseColorScale (default 1) multiplying the base colour in M_Miner, create MI_Miner (child) with BaseColorScale 0.6 (Q10, provisional), assign MI_Miner to the SK_Miner slot(s) (or the BP_Miner mesh override), compile and save. Change nothing else (no light, exposure, bloom, roughness or normal map change).
- agent: bp
- depends_on: none
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md), task-386-miner-mixamo-animation (vault/reference/tasks/task-386-miner-mixamo-animation.md), global request and gdd_check: vault/reference/tasks/task-393-miner-over-lit.md
- expected_output: M_Miner and new MI_Miner saved, SK_Miner or BP_Miner updated; commit `task-393-01-miner-material-scale: ...`; change summary with the readings and the assets changed, for the doc subtask.
- done_when: MI_Miner exists with BaseColorScale 0.6 read back, assigned to every miner material slot (read back), M_Miner compiles with no error, a PIE start logs no material error; readings reported; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: sonnet
- tokens: input 40, cache write 56879, cache read 856669, output 10579
- usage: turns 20, context start 12k peak 62k, resumes 0, 1 min, units 0.21 M
- runs: 1
- milestone: M2
- started: 2026-10-30T22:02:49Z
- waiting_for: none
- result: MI_Miner BaseColorScale 0.6 on SK_Miner slot, commit 6a8b51fe; normal map wired; roughness/spec/metallic unwired defaults; albedo mean ~0.35 sRGB; PIE/render unverified

## Relations
- [[task-393-miner-over-lit]] — parent global task.
