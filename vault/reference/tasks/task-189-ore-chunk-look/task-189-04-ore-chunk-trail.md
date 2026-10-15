---
id: task-189-04-ore-chunk-trail
description: "BP: NS_OreTrail, violet Duskite copy of the XP trail Niagara system for the ore chunk pickup."
parent: task-189-ore-chunk-look
kind: task
status: done
---
# Ore chunk: violet trail

- goal: create NS_OreTrail as a copy of NS_XPTrail (ribbon in world space, same width, lifetime and spawn rate) recoloured to the Duskite violet chosen in task-189-03 (owner 2026-10-15: the ore chunk gets a trail; this overrides the no-trail note of ore-in-rock-plan for the 30 cm chunk).
- agent: bp
- depends_on: task-189-03-ore-chunk-materials
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), color-code (vault/reference/gdd/art-audio/color-code.md)
- expected_output: /Game/RogueRigs/FX/NS_OreTrail saved by explicit path; values in bp-pickup-ore.
- done_when: system compiles; colour read back; doc updated; lint clean; committed by path.
- editor_state: open
- token_ceiling: open
- model: sonnet
- tokens: input 46, cache write 43606, cache read 680361, output 892
- runs: 1
- milestone: M1
- result: NS_OreTrail duplicated from NS_XPTrail, particle Color (0.55, 0.1, 1.0) read back, SpawnRate 60, compile UpToDate; NS_XPTrail untouched; not seen on screen; commit 82b26eee

## Relations
- [[task-189-ore-chunk-look]] — parent global task.
