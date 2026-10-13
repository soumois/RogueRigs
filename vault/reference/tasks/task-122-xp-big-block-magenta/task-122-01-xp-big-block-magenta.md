---
id: task-122-01-xp-big-block-magenta
description: "The big XP block is bright magenta-pink with a glow of the same hue instead of white."
parent: task-122-xp-big-block-magenta
kind: task
status: in-progress
---
# Big XP block colour

- goal: The big XP block is bright magenta-pink with a glow of the same hue instead of white.
- agent: bp
- depends_on: none
- inputs: color-code (vault/reference/gdd/art-audio/color-code.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), sys-pickups-xp (vault/reference/tech/systems/world/sys-pickups/sys-pickups-xp.md), xp-level-up (vault/reference/gdd/run/xp-level-up.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: MI_PickupXPBig base and emissive magenta-pink (about (1, 0.15, 0.75) base, emissive of the same hue), glow and trail follow; bp-pickup-xp and color-code updated.
- done_when: material instance parameters read back; bp-pickup-xp doc matches; no scale or XP value changed; lint clean
- editor_state: editor open (Unreal MCP); no screenshots by the agent
- token_ceiling: 40 turns, commit after each group
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-122-xp-big-block-magenta]] — parent global task.
