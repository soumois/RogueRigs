---
id: task-077-01-barks
description: "A4 Rook and Vesna barks: text agent writes, validator checks, critic reviews, corrections applied (2 loops at most)."
parent: task-077-barks
kind: task
status: todo
---
# A4 Rook and Vesna barks

- goal: A4 Rook and Vesna barks: text agent writes, validator checks, critic reviews, corrections applied (2 loops at most).
- agent: text
- depends_on: task-075-01-baseline-run
- inputs: assignment-4-plan (vault/reference/course/assignments/assignment-4-plan.md), agent-text (vault/reference/agents/agent-text.md), lore (vault/reference/gdd/lore.md), crew-characters (vault/reference/gdd/crew-characters.md), fictional-films (vault/reference/gdd/fictional-films.md), company-voice (vault/reference/gdd/company-voice.md), dialogue-tone (vault/reference/gdd/art-audio/dialogue-tone.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), sys-hud (vault/reference/tech/systems/sys-hud.md), sys-objectives (vault/reference/tech/systems/sys-objectives.md)
- expected_output: Rook and Vesna barks per existing game event (trigger from the GDD events), tone from the fixed list of 5, 8 words at most. File Content/RogueRigs/Data/Text/barks.json. Agent output files are pure JSON or CSV: no prose, no markdown. A failing item never stays in Content/ or in a vault node; failures go to Tools/content/logs/ only.
- done_when: validate_text.js exits 0; the file is in Content/RogueRigs/Data/Text/; retrieval log in Tools/content/logs/; every item reviewed by the critic with a verdict; failures corrected within 2 loops or reported to the owner.
- editor_state: any
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-077-barks]] — parent global task.
