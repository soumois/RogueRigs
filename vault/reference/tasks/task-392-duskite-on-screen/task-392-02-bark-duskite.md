---
id: task-392-02-bark-duskite
description: "Text: barks.json: "Tip is the ore." becomes "Tip is the Duskite.""
parent: task-392-duskite-on-screen
kind: task
status: done
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
- model: sonnet
- tokens: input 20, cache write 21706, cache read 180106, output 4210
- usage: turns 10, context start 11k peak 26k, resumes 0, 1 min, units 0.07 M
- runs: 1
- milestone: M2
- started: 2026-10-30T21:59:06Z
- waiting_for: none
- result: barks.json rook_shuttle_landed_01 now Tip is the Duskite., validator passes, commit 874624a0

## Relations
- [[task-392-duskite-on-screen]] — parent global task.
