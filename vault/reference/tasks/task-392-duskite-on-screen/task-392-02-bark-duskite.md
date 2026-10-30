---
id: task-392-02-bark-duskite
description: "Text: barks.json: "Tip is the ore." becomes "Tip is the Duskite.""
parent: task-392-duskite-on-screen
kind: task
status: todo
---
# On-screen ore word: Duskite: text

- goal: In Content/RogueRigs/Data/Text/barks.json, the line "Taxi is here. Tip is the ore." -> "Taxi is here. Tip is the Duskite." (Q8), checked against the dialogue tone rules; nothing else changes; run the validator.
- agent: text
- depends_on: none
- inputs: agent-text (vault/reference/agents/agent-text.md), sys-text-content (vault/reference/tech/systems/ui/sys-text-content.md), dialogue-tone (vault/reference/gdd/art-audio/dialogue-tone.md), ore-blocks (vault/reference/gdd/terrain/ore-blocks.md), Content/RogueRigs/Data/Text/barks.json, global request and gdd_check: vault/reference/tasks/task-392-duskite-on-screen.md
- expected_output: barks.json updated, retrieval log; commit `task-392-02-bark-duskite: ...`.
- done_when: The bark reads "Taxi is here. Tip is the Duskite.", nothing else changed, validator passes; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-392-duskite-on-screen]] — parent global task.
