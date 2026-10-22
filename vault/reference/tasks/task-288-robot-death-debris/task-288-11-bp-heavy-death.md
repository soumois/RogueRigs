---
id: task-288-11-bp-heavy-death
description: "BP_Heavy throws its debris (bp)."
parent: task-288-robot-death-debris
kind: task
status: todo
---
# BP_Heavy throws its debris

- goal: On BP_Heavy: DeathPuffSystem = NS_RobotDeathPuff (pool 40), DebrisSystem = NS_HeavyDebris, DebrisPoolSize 80, DebrisPieces = one entry per piece of debris_pieces.json (Group, Kind, MeshIndex, Offset = centre_offset_cm_unreal), as on BP_Chaser.
- agent: bp
- depends_on: task-288-10-bp-ns-kamikaze-debris
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), enemy-death (vault/reference/gdd/enemies/enemy-death.md), bp-robot-debris (vault/reference/tech/blueprints/fx/bp-robot-debris.md), bp-heavy (vault/reference/tech/blueprints/enemies/bp-heavy.md), bp-robot-death-puff (vault/reference/tech/blueprints/fx/bp-robot-death-puff.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md); files: Art/Source/heavy-meshy/debris/debris_pieces.json
- expected_output: BP_Heavy compiled and saved; doc node bp-heavy updated (before, after, revert)
- done_when: defaults read back (entries, indexes, offsets), compiles without LogBlueprint error, doc updated; lint clean
- editor_state: open
- token_ceiling: 80000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-288-robot-death-debris]] — parent global task.
