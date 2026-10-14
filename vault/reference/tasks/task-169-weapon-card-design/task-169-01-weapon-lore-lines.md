---
id: task-169-01-weapon-lore-lines
description: "Write one lore line for each offerable weapon (shotgun, turret launcher) for the new-weapon cards."
parent: task-169-weapon-card-design
kind: task
status: done
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
- model: opus+sonnet
- tokens: input 40, cache write 61534, cache read 358656, output 3866
- runs: 3
- milestone: M2
- result: weapon_texts.json: shotgun 'Cut from a dead security bay; corporate can't have it back.', turret launcher 'Cobbled from spare drill parts; outworks every corporate robot here.' (critic loop 0: shotgun pass, turret L1 fail fixed with the critic's correction in loop 1). Validator does not accept weapon_texts yet: extension in task-169-03. Commits d054024, 67eebf7, 2300e8e.

## Relations
- [[task-169-weapon-card-design]] — parent global task.
