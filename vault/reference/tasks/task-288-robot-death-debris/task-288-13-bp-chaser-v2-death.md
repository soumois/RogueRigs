---
id: task-288-13-bp-chaser-v2-death
description: "BP_ChaserV2 throws its debris (bp)."
parent: task-288-robot-death-debris
kind: task
status: done
---
# BP_ChaserV2 throws its debris

- goal: On BP_ChaserV2: DeathPuffSystem = NS_RobotDeathPuff (pool 40), DebrisSystem = NS_ChaserV2Debris, DebrisPoolSize 80, DebrisPieces = one entry per piece of debris_pieces.json (Group, Kind, MeshIndex, Offset = centre_offset_cm_unreal), as on BP_Chaser.
- agent: bp
- depends_on: task-288-10-bp-ns-kamikaze-debris
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), enemy-death (vault/reference/gdd/enemies/enemy-death.md), bp-robot-debris (vault/reference/tech/blueprints/fx/bp-robot-debris.md), bp-chaser (vault/reference/tech/blueprints/enemies/bp-chaser.md), bp-robot-death-puff (vault/reference/tech/blueprints/fx/bp-robot-death-puff.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md); files: Art/Source/chaser-v2-meshy/debris/debris_pieces.json
- expected_output: BP_ChaserV2 compiled and saved; doc node bp-chaser-v2 updated (before, after, revert)
- done_when: defaults read back (entries, indexes, offsets), compiles without LogBlueprint error, doc updated; lint clean
- editor_state: open
- token_ceiling: 80000
- model: sonnet
- tokens: input 30, cache write 32136, cache read 336432, output 6650
- runs: 1
- milestone: M2
- result: BP_ChaserV2: NS_RobotDeathPuff (40), NS_ChaserV2Debris, pool 80, 13 DebrisPieces read back; compiles; BP_Chaser untouched.

## Relations
- [[task-288-robot-death-debris]] — parent global task.
