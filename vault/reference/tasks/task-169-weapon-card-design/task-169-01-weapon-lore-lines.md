---
id: task-169-01-weapon-lore-lines
description: "Write one lore line for each offerable weapon (shotgun, turret launcher) for the new-weapon cards."
parent: task-169-weapon-card-design
kind: task
status: in-progress
---
# New-weapon cards: lore lines

- goal: Write one lore line for each offerable weapon (shotgun, turret launcher) for the new-weapon cards.
- agent: text
- depends_on: none
- inputs: lore (vault/reference/gdd/lore.md), crew-characters (vault/reference/gdd/crew-characters.md), fictional-films (vault/reference/gdd/fictional-films.md), company-voice (vault/reference/gdd/company-voice.md), dialogue-tone (vault/reference/gdd/art-audio/dialogue-tone.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), shotgun (vault/reference/gdd/weapons/shotgun.md), turret (vault/reference/gdd/weapons/turret.md), sys-text-content (vault/reference/tech/systems/ui/sys-text-content.md)
- expected_output: Content/RogueRigs/Data/Text/weapon_texts.json (schema_version 1, run_id <date>-<time>-weapon_texts, items {id wpn_shotgun / wpn_turret_launcher, weapon = WeaponStats DisplayName, flavor}) and its retrieval log in Tools/content/logs/
- done_when: Two lore lines, English, at most 90 characters each, no mechanic or number changed; style-guide critic pass on both (or owner ruling after 2 loops).
- editor_state: any
- token_ceiling: 30k
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-169-weapon-card-design]] — parent global task.
