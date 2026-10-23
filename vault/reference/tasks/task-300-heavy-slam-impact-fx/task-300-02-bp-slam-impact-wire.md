---
id: task-300-02-bp-slam-impact-wire
description: "BP_Heavy OnSlam spawns NS_HeavySlamImpact at Location with User.Radius = Radius (bp)."
parent: task-300-heavy-slam-impact-fx
kind: task
status: done
---
# BP_Heavy plays the slam impact

- goal: BP_Heavy OnSlam(Location, Radius): SpawnSystemAtLocation NS_HeavySlamImpact at Location, then set User.Radius = Radius; nothing else changed.
- agent: bp
- depends_on: task-300-01-bp-slam-impact-ns
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-heavy (vault/reference/tech/blueprints/enemies/bp-heavy.md), sys-enemies-heavy (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-heavy.md); files: report of 300-01
- expected_output: BP_Heavy updated; bp-heavy updated (before, after, revert)
- done_when: OnSlam graph read back; compiled and saved; doc updated; lint clean
- editor_state: open (owner editor, MCP route, do not close it)
- token_ceiling: 25000
- model: sonnet
- tokens: input 30, cache write 31634, cache read 345217, output 5768
- runs: 1
- milestone: M2
- result: BP_Heavy OnSlam (commit 9a6de5ff): SpawnSystemAtLocation NS_HeavySlamImpact at Location, then SetNiagaraVariable(Float) User.Radius = Radius on the returned component; graph read back, compiled, saved; bp-heavy doc updated (before, after, revert). Not tested in PIE.

## Relations
- [[task-300-heavy-slam-impact-fx]] — parent global task.
