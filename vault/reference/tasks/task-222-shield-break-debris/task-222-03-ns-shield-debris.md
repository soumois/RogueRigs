---
id: task-222-03-ns-shield-debris
description: "bp: NS_ShieldDebris, a one-shot GPU mesh burst of the five shield pieces built like NS_RobotDebris; doc node."
parent: task-222-shield-break-debris
kind: task
status: done
---
# NS_ShieldDebris

- goal: `/Game/RogueRigs/FX/NS_ShieldDebris` throwing the five shield pieces from where the shield stood.
- agent: agent-bp
- depends_on: task-222-02-shield-debris-import
- inputs: `vault/reference/tech/blueprints/fx/bp-robot-debris.md` (the system to copy: emitter, modules, offsets table, collision, sink, GPU settings, tools that worked and failed), `Art/Source/shield-chaser/debris/shield_debris_pieces.json`, `vault/reference/agents/bp-mcp-lessons.md`, `vault/templates/blueprint.md`.
- method: duplicate NS_RobotDebris (never edit it). Mesh renderer = the five SM_ShieldDebris_* in json order; burst count 5, every piece once (mesh index = exec index, no user array needed); offsets table keyed by mesh index from the json, rotated by the owner rotation as in NS_RobotDebris; initial velocity mostly along the owner forward (+X of the component) with an upward part and a random spread (no HitDirection user parameter); keep life, gravity, drag, spin, collision, sink from NS_RobotDebris. New doc node `vault/reference/tech/blueprints/fx/bp-shield-debris.md` from the template, listed in its index.
- done_when: system compiles UpToDate with no stack error; saved; doc node and index updated, lint clean.
- editor_state: open (launched by the Orchestrator)
- token_ceiling: open
- model: sonnet
- tokens: input 64, cache write 68226, cache read 1343716, output 1072
- runs: 1
- milestone: M1
- result: Commit c4a62d86: /Game/RogueRigs/FX/NS_ShieldDebris from NS_RobotDebris (GPU, life 4.9 s, gravity, drag, spin, collision, sink kept); mesh renderer = the 5 pieces in json order, burst 5, Mesh Index = ExecIndex(), 5-entry offsets table, velocity along the owner forward +/-15 degrees with upward part, ThrowScale 1.3-2.0; no user parameters; compiles UpToDate, 0 stack errors; doc node bp-shield-debris. Unverified: ExecIndex mesh indices and the throw in game.

## Relations
- [[task-222-shield-break-debris]] — parent global task.
