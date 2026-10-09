---
id: task-058-01-xp-trail-visible
description: "The XP cube Niagara trail is visible in game (the sparkle already is)."
parent: task-058-xp-trail-visible
kind: task
status: done
---
# Visible XP trail

- goal: The XP cube Niagara trail is visible in game (the sparkle already is).
- agent: bp
- depends_on: none
- inputs: bp-pickup-xp (vault/reference/tech/blueprints/bp-pickup-xp.md), sys-pickups (vault/reference/tech/systems/sys-pickups.md), color-code (vault/reference/gdd/art-audio/color-code.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md)
- expected_output: find why NS_XPTrail (TrailFX on BP_PickupXP, activated on OnMagnetStart) is not visible in PIE while NS_PickupSparkle is: check the emitter (LocationBasedRibbon: local vs world space, ribbon width and facing for a top-down camera, spawn rate, lifetime, material, colour and alpha), the component (attachment, relative scale, auto-activate, activation actually reached), and fix it; the trail must read as a cyan streak behind each cube flying to the miner; docs bp-pickup-xp; PIE: no log error, TrailFX active and particle count above 0 on a magnetised cube (read through MCP).
- done_when: PIE check with no error; docs updated; lint clean.
- editor_state: open
- token_ceiling: 60000
- model: sonnet
- tokens: input 50, cache write 46776, cache read 727749, output 11292
- runs: 1
- milestone: M2
- result: Cause: the NS_XPTrail ribbon emitter had no spawn module, so it spawned no particles (TrailFX active but empty). Fix: engine SpawnRate module at 60 per second; world space, Screen facing, cyan, width 10, lifetime 0.35 s kept. Compiled clean; PIE no Niagara error; particle count not readable through MCP, visual check left to the owner. Commits b90ab87, 244b84f.

## Relations
- [[task-058-xp-trail-visible]] — parent global task.
