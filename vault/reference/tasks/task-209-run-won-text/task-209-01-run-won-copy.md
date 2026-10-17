---
id: task-209-01-run-won-copy
description: "text: Write the run-won end screen title"
parent: task-209-run-won-text
kind: task
status: in-progress
---
# run-won-copy

- goal: Write the run-won end screen title (and an optional one-line subtitle) in English, in the game voice, as a new entry in the text content files.
- agent: text
- depends_on: none
- inputs: agent-text (vault/reference/agents/agent-text.md), lore (vault/reference/gdd/lore.md), crew-characters (vault/reference/gdd/crew-characters.md), fictional-films (vault/reference/gdd/fictional-films.md), company-voice (vault/reference/gdd/company-voice.md), dialogue-tone (vault/reference/gdd/art-audio/dialogue-tone.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), sys-text-content (vault/reference/tech/systems/ui/sys-text-content.md), run-structure (vault/reference/gdd/run/run-structure.md), extraction (vault/reference/gdd/run/extraction.md), wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md)
- expected_output: An entry in Content/RogueRigs/Data/Text/ (file chosen per sys-text-content) and a retrieval log in Tools/content/logs/.
- done_when: text validator exits 0 if one exists; style-guide review follows; committed.
- editor_state: any
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-209-run-won-text]] — parent global task.
