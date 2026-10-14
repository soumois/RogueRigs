---
id: task-168-01-card-lore-lines
description: "Rewrite the 13 upgrade flavor lines as pure lore lines in Rook's voice, without describing the stat effect, now shown on its own line."
parent: task-168-upgrade-card-design
kind: task
status: done
---
# Upgrade cards: pure lore lines

- goal: Rewrite the 13 upgrade flavor lines as pure lore lines in Rook's voice, without describing the stat effect, now shown on its own line.
- agent: text
- depends_on: none
- inputs: lore (vault/reference/gdd/lore.md), crew-characters (vault/reference/gdd/crew-characters.md), fictional-films (vault/reference/gdd/fictional-films.md), company-voice (vault/reference/gdd/company-voice.md), dialogue-tone (vault/reference/gdd/art-audio/dialogue-tone.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), upgrades (vault/reference/gdd/weapons/upgrades.md), sys-text-content (vault/reference/tech/systems/ui/sys-text-content.md)
- expected_output: Content/RogueRigs/Data/Text/upgrade_texts.json rewritten (same ids and stats, flavor = pure lore line, English, at most 90 characters) and its retrieval log in Tools/content/logs/
- done_when: All 13 items rewritten without describing the stat effect; ids, stats, schema_version and run_id format unchanged; style-guide critic pass on every item (or owner ruling after 2 loops).
- editor_state: any
- token_ceiling: 30k
- model: opus+sonnet
- tokens: input 38, cache write 67022, cache read 378021, output 5460
- runs: 2
- milestone: M2
- result: upgrade_texts.json: 13 pure lore lines in Rook's voice, validator exit 0; critic loop 0: 12 pass, upg_move_speed T3 fail (Die Hard catchphrase) fixed in loop 1 with the critic's correction. Commits 092ec1a, 77c33af.

## Relations
- [[task-168-upgrade-card-design]] — parent global task.
