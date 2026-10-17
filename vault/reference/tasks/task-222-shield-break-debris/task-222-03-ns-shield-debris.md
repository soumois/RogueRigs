---
id: task-222-03-ns-shield-debris
description: "bp: NS_ShieldDebris, a one-shot GPU mesh burst of the five shield pieces built like NS_RobotDebris; doc node."
parent: task-222-shield-break-debris
kind: task
status: in-progress
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
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-222-shield-break-debris]] — parent global task.
