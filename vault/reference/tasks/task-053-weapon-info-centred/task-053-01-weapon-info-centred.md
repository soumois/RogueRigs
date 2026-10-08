---
id: task-053-01-weapon-info-centred
description: "Weapon slot info (name, ammo bar or blocks, reload bar, LOCKED) centred under each slot."
parent: task-053-weapon-info-centred
kind: task
status: in-progress
---
# Centred weapon info

- goal: Weapon slot info (name, ammo bar or blocks, reload bar, LOCKED) centred under each slot.
- agent: bp
- depends_on: none
- inputs: wbp-hud (vault/reference/tech/blueprints/wbp-hud.md), sys-hud (vault/reference/tech/systems/sys-hud.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md)
- expected_output: WBP_HUD: for each of the 4 weapon slots, the slot text, AmmoBar, ReloadBar, BlockRow and the LOCKED text are centred horizontally on the slot (same centre x); layout only, no logic change; docs wbp-hud; PIE: no log error, positions read back.
- done_when: PIE check with no error; docs updated; lint clean.
- editor_state: open
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-053-weapon-info-centred]] — parent global task.
