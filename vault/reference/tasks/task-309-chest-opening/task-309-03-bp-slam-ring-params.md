---
id: task-309-03-bp-slam-ring-params
description: "DustColor and ChipCount on NS_HeavySlamImpact (bp): reuse the slam ring for the chest, step 03."
parent: task-309-chest-opening
kind: task
status: done
---
# DustColor and ChipCount on NS_HeavySlamImpact

- goal: Add two user parameters to NS_HeavySlamImpact: User.DustColor (LinearColor, default the current dust colour 0.18, 0.13, 0.09 alpha 0.7) driving the Dust emitter colour, and User.ChipCount (int, default 16) driving the Chips burst count; the heavy look stays exactly as accepted by the owner (2026-10-23); update bp-heavy-slam-impact.
- agent: bp
- depends_on: none
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-heavy-slam-impact (vault/reference/tech/blueprints/fx/bp-heavy-slam-impact.md), bp-heavy (vault/reference/tech/blueprints/enemies/bp-heavy.md); files: none; Art/Source/chest/reference/egor-chest-juice.mp4 (technique G)
- expected_output: NS_HeavySlamImpact with two new user parameters; doc node updated
- done_when: system compiles with 0 errors; both defaults read back equal to the old values; BP_Heavy still compiles; doc node updated; lint clean; committed
- editor_state: open
- token_ceiling: 1.0 M units
- model: sonnet
- tokens: input 30, cache write 51201, cache read 491266, output 7497
- usage: turns 15, context start 12k peak 56k, resumes 0, 1 min, units 0.15 M
- runs: 1
- milestone: M2
- started: 2026-10-29T19:02:27Z
- result: Done (fbdfbfed): NS_HeavySlamImpact User.DustColor (0.18,0.13,0.09,0.7) -> Dust Color, User.ChipCount 16 -> Chips spawn count; defaults read back, compile UpToDate, BP_Heavy compiles. Look unverified.

## Relations
- [[task-309-chest-opening]] — parent global task.
