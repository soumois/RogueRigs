---
id: task-054-01-niagara-trail-sparkle
description: "XP cube trail and pickup sparkle rebuilt as Niagara systems."
parent: task-054-niagara-trail-sparkle
kind: task
status: in-progress
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
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-054-niagara-trail-sparkle]] — parent global task.
