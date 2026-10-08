---
id: task-053-01-weapon-info-centred
description: "Weapon slot info (name, ammo bar or blocks, reload bar, LOCKED) centred under each slot."
parent: task-053-weapon-info-centred
kind: task
status: done
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
- model: sonnet
- tokens: input 68, cache write 56129, cache read 1184229, output 15703
- runs: 2
- milestone: M2
- result: WBP_HUD: per slot the text (LOCKED included), AmmoBar, ReloadBar and BlockRow share one centre x (76, 246, 416, 586; canvas alignment 0.5); unused blocks collapsed, blocks auto-sized 12 × 10 with 1 px padding, rows auto-sized so visible blocks are centred. PIE: no log error; block rows not seen on screen. Doc wbp-hud. Commits 838e1f2, 01baf1d.

## Relations
- [[task-053-weapon-info-centred]] — parent global task.
