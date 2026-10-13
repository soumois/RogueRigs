---
id: task-123-01-heavy-remove-shockring
description: "BP_Heavy no longer shows its ShockRing mesh: the slam zone is drawn only by the ground telegraph."
parent: task-123-heavy-remove-shockring
kind: task
status: in-progress
---
# Remove the heavy ShockRing

- goal: BP_Heavy no longer shows its ShockRing mesh: the slam zone is drawn only by the ground telegraph.
- agent: bp
- depends_on: none
- inputs: bp-heavy (vault/reference/tech/blueprints/enemies/bp-heavy.md), enemy-heavy (vault/reference/gdd/enemies/enemy-heavy.md), ground-telegraph (vault/reference/gdd/art-audio/ground-telegraph.md), sys-enemies-heavy (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-heavy.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: ShockRing no longer shown in OnTelegraphStart nor OnSlam (component removed if nothing else uses it); bp-heavy doc updated; glow and tilt of the telegraph unchanged.
- done_when: Blueprint compiles and is saved; read back shows no ShockRing visibility call left; bp-heavy doc matches; lint clean
- editor_state: editor open (Unreal MCP); no screenshots by the agent
- token_ceiling: 40 turns, commit after each group
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-123-heavy-remove-shockring]] — parent global task.
