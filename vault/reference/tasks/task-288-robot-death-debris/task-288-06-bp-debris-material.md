---
id: task-288-06-bp-debris-material
description: "Robot debris material and instances (bp)."
parent: task-288-robot-death-debris
kind: task
status: done
---
# Robot debris material and instances

- goal: Create M_RobotDebris (opaque, default lit, one-sided, base colour from a BaseColorTex parameter, no WPO, no emissive, bUsedWithNiagaraMeshParticles on) with instances MI_HeavyDebris, MI_ShooterDebris, MI_ChaserV2Debris, MI_KamikazeDebris on the cleaned base colour textures imported in task-283-06; M_ChaserDebris (v1) untouched.
- agent: bp
- depends_on: task-288-05-cpp-debris-groups
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), enemy-death (vault/reference/gdd/enemies/enemy-death.md), bp-robot-debris (vault/reference/tech/blueprints/fx/bp-robot-debris.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md); files: the four T_*_BaseColor textures in /Game/RogueRigs/Art/{Heavy,Shooter,ChaserV2,Kamikaze}/
- expected_output: assets in /Game/RogueRigs/Materials/Robots/; doc node bp-robot-debris (or a new sibling) updated
- done_when: instances read back, compiles without error, saved, doc updated; lint clean
- editor_state: open
- token_ceiling: 80000
- model: sonnet
- tokens: input 34, cache write 48779, cache read 526887, output 9496
- runs: 1
- milestone: M2
- result: M_RobotDebris (opaque, lit, one-sided, BaseColorTex, no WPO/emissive, bUsedWithNiagaraMeshParticles) + MI_HeavyDebris, MI_ShooterDebris, MI_ChaserV2Debris, MI_KamikazeDebris in Materials/Robots, read back; M_ChaserDebris untouched; bp-robot-debris updated.

## Relations
- [[task-288-robot-death-debris]] — parent global task.
