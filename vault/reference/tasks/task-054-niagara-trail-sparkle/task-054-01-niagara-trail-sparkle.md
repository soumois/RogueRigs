---
id: task-054-01-niagara-trail-sparkle
description: "XP cube trail and pickup sparkle rebuilt as Niagara systems."
parent: task-054-niagara-trail-sparkle
kind: task
status: done
---
# Niagara trail and sparkle

- goal: XP cube trail and pickup sparkle rebuilt as Niagara systems.
- agent: bp
- depends_on: none
- inputs: bp-pickup-xp (vault/reference/tech/blueprints/bp-pickup-xp.md), bp-pickup-sparkle (vault/reference/tech/blueprints/bp-pickup-sparkle.md), sys-pickups (vault/reference/tech/systems/sys-pickups.md), color-code (vault/reference/gdd/art-audio/color-code.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md)
- expected_output: NS_XPTrail (cyan ribbon or sprite trail following the magnetised XP cube, started on OnMagnetStart) and NS_PickupSparkle (short cyan burst at the collect point, auto-destroy) created as Niagara systems (from engine templates if possible); BP_PickupXP uses NS_XPTrail instead of the stretched cube Trail; the sparkle spawned on collect is NS_PickupSparkle instead of the BP_PickupSparkle mesh (say whether BP_PickupSparkle is kept or removed); if the MCP cannot author Niagara systems, stop after the first failed attempts and report exactly what is missing; docs bp-pickup-xp, bp-pickup-sparkle (or new nodes for the NS assets); PIE: no log error, the components exist.
- done_when: PIE check with no error; docs updated; lint clean.
- editor_state: open
- token_ceiling: 80000
- model: sonnet
- tokens: input 88, cache write 78228, cache read 2163267, output 23155
- runs: 2
- milestone: M2
- result: NS_XPTrail (LocationBasedRibbon, cyan, width 10) as TrailFX on BP_PickupXP, activated on OnMagnetStart (old cube Trail removed); NS_PickupSparkle (RadialBurst, 18 cyan sprites, 0.18–0.28 s) spawned on OnCollected; BP_PickupSparkle kept unused. Not seen running; PIE log not readable. Juice test update moved to task-054-02. Commit 11e36d3.

## Relations
- [[task-054-niagara-trail-sparkle]] — parent global task.
