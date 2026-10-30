---
id: task-392-01-screen-texts-duskite
description: "Text: screen_texts.json: intermission_recap "Total Ore:" becomes "Total Duskite:""
parent: task-392-duskite-on-screen
kind: task
status: in-progress
---
# On-screen ore word: Duskite: text

- goal: In Content/RogueRigs/Data/Text/screen_texts.json, item intermission_recap, lines[0] "Total Ore:" -> "Total Duskite:" (Q8); nothing else changes; run the validator.
- agent: text
- depends_on: none
- inputs: agent-text (vault/reference/agents/agent-text.md), sys-text-content (vault/reference/tech/systems/ui/sys-text-content.md), ore-blocks (vault/reference/gdd/terrain/ore-blocks.md), Content/RogueRigs/Data/Text/screen_texts.json, global request and gdd_check: vault/reference/tasks/task-392-duskite-on-screen.md
- expected_output: screen_texts.json updated, retrieval log in Tools/content/logs/, its own node updates as the text agent does; commit `task-392-01-screen-texts-duskite: ...`.
- done_when: lines[0] reads "Total Duskite:", every other value unchanged (diff of one string plus run_id), validator passes; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-30T21:58:25Z
- waiting_for: none
- result: open

## Relations
- [[task-392-duskite-on-screen]] — parent global task.
