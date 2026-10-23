---
id: task-288-15-bp-shield-v2-death
description: "BP_ShieldChaserV2 throws chaser v2 debris (bp)."
parent: task-288-robot-death-debris
kind: task
status: in-progress
---
# BP_ShieldChaserV2 throws chaser v2 debris

- goal: On BP_ShieldChaserV2: DebrisSystem = NS_ChaserV2Debris and DebrisPieces = the chaser v2 entries (same mesh as chaser v2); the shield break keeps NS_ShieldDebris and the v1 shield shard meshes (same shield mesh, referenced, not modified); BP_ShieldChaser (v1) untouched.
- agent: bp
- depends_on: task-288-13-bp-chaser-v2-death, task-286-02-bp-shield-chaser-v2
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), enemy-death (vault/reference/gdd/enemies/enemy-death.md), bp-robot-debris (vault/reference/tech/blueprints/fx/bp-robot-debris.md), bp-shield-chaser (vault/reference/tech/blueprints/enemies/bp-shield-chaser.md), bp-shield-debris (vault/reference/tech/blueprints/fx/bp-shield-debris.md); files: BP_ChaserV2 DebrisPieces (288-13)
- expected_output: BP_ShieldChaserV2 compiled and saved; doc node bp-shield-chaser-v2 updated
- done_when: defaults read back equal to BP_ChaserV2, OnShieldBroken still spawns NS_ShieldDebris, git diff shows no v1 asset changed; lint clean
- editor_state: open
- token_ceiling: 80000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-288-robot-death-debris]] — parent global task.
