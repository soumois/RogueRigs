---
id: task-288-12-bp-shooter-death
description: "BP_Shooter throws its debris (bp)."
parent: task-288-robot-death-debris
kind: task
status: done
---
# BP_Shooter throws its debris

- goal: On BP_Shooter: DeathPuffSystem = NS_RobotDeathPuff (pool 40), DebrisSystem = NS_ShooterDebris, DebrisPoolSize 80, DebrisPieces = one entry per piece of debris_pieces.json (Group, Kind, MeshIndex, Offset = centre_offset_cm_unreal), as on BP_Chaser.
- agent: bp
- depends_on: task-288-10-bp-ns-kamikaze-debris
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), enemy-death (vault/reference/gdd/enemies/enemy-death.md), bp-robot-debris (vault/reference/tech/blueprints/fx/bp-robot-debris.md), bp-shooter (vault/reference/tech/blueprints/enemies/bp-shooter.md), bp-robot-death-puff (vault/reference/tech/blueprints/fx/bp-robot-death-puff.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md); files: Art/Source/shooter-meshy/debris/debris_pieces.json
- expected_output: BP_Shooter compiled and saved; doc node bp-shooter updated (before, after, revert)
- done_when: defaults read back (entries, indexes, offsets), compiles without LogBlueprint error, doc updated; lint clean
- editor_state: open
- token_ceiling: 80000
- model: sonnet
- tokens: input 30, cache write 31820, cache read 343396, output 7028
- runs: 1
- milestone: M2
- result: BP_Shooter: NS_RobotDeathPuff (40), NS_ShooterDebris, pool 80, 13 DebrisPieces read back; compiles.

## Relations
- [[task-288-robot-death-debris]] — parent global task.
