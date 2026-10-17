---
id: task-210-01-transition-copy
description: "text: Write the English texts of the intermission"
parent: task-210-transition-screens-english
kind: task
status: done
---
# transition-copy

- goal: Write the English texts of the intermission (title with level number, next level line, ore this level, run ore, shop placeholder label, Continue button) and of the loading screen (title, level line, the three stage lines) in the game voice, as entries in the text content files.
- agent: text
- depends_on: none
- inputs: agent-text (vault/reference/agents/agent-text.md), lore (vault/reference/gdd/lore.md), crew-characters (vault/reference/gdd/crew-characters.md), fictional-films (vault/reference/gdd/fictional-films.md), company-voice (vault/reference/gdd/company-voice.md), dialogue-tone (vault/reference/gdd/art-audio/dialogue-tone.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), sys-text-content (vault/reference/tech/systems/ui/sys-text-content.md), intermission (vault/reference/gdd/run/intermission.md), run-shop (vault/reference/gdd/run/run-shop.md), wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md), wbp-loading (vault/reference/tech/blueprints/ui/wbp-loading.md)
- expected_output: Entries in Content/RogueRigs/Data/Text/ and a retrieval log in Tools/content/logs/.
- done_when: text validator exits 0 if one exists; style-guide review follows; committed.
- editor_state: any
- token_ceiling: 30000
- model: sonnet
- tokens: input 12, cache write 31234, cache read 108920, output 456
- runs: 1
- milestone: M2
- result: screen_texts.json items intermission_recap (LEVEL {N} CLEARED; Next claim: level {NEXT} of {TOTAL}; Ore secured this level: {ORE}; Ore secured this run: {ORE}; labels Shop opens later, Continue) and loading_level (DROPPING IN; Surveying the terrain, Loading the map, Prepping the level; label Level {N}) (8a4076e0); placeholders replaced by the widget; validator exit 0.

## Relations
- [[task-210-transition-screens-english]] — parent global task.
