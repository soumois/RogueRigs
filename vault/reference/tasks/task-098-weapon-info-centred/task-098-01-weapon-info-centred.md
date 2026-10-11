---
id: task-098-01-weapon-info-centred
description: "The weapon slot info (name, ammo, LOCKED) is really centred under each slot, not on the left."
parent: task-098-weapon-info-centred
kind: task
status: todo
---
# Weapon info centred

- goal: The weapon slot info (name, ammo, LOCKED) is really centred under each slot, not on the left.
- agent: bp
- depends_on: task-099-01-company-subtitles
- inputs: wbp-hud (vault/reference/tech/blueprints/wbp-hud.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md)
- expected_output: owner playtest: the weapon names, ammo and LOCKED texts stay on the left under each slot (see the capture: "Burst rifle", "Turret launcher", "LOCKED" left aligned) although task-053 had centred them. Find why (text justification of the TextBlocks, auto size with alignment, anchors, a later function resetting positions, ammo bar offsets) and fix so that for each slot the slot text, AmmoBar, ReloadBar and BlockRow share the same centre x and the text is centred (text justification centre where needed). Read back from the graph/asset; update wbp-hud; no screenshot calls (nobody sees the screen: the owner will judge). Remember the quirks: dirty-asset trick before saving, real newline in pins.
- done_when: layout values read back from the widget; compile 0 errors; asset saved; wbp-hud updated; lint clean; committed.
- editor_state: open
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-098-weapon-info-centred]] — parent global task.
