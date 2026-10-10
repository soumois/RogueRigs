---
id: task-076-01-upgrade-texts
description: "A4 upgrade card texts: text agent writes, validator checks, critic reviews, corrections applied (2 loops at most)."
parent: task-076-upgrade-texts
kind: task
status: in-progress
---
# A4 upgrade card texts

- goal: A4 upgrade card texts: text agent writes, validator checks, critic reviews, corrections applied (2 loops at most).
- agent: text
- depends_on: task-075-01-baseline-run
- inputs: assignment-4-plan (vault/reference/course/assignments/assignment-4-plan.md), agent-text (vault/reference/agents/agent-text.md), lore (vault/reference/gdd/lore.md), crew-characters (vault/reference/gdd/crew-characters.md), fictional-films (vault/reference/gdd/fictional-films.md), company-voice (vault/reference/gdd/company-voice.md), dialogue-tone (vault/reference/gdd/art-audio/dialogue-tone.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), upgrades (vault/reference/gdd/weapons/upgrades.md), sys-upgrades (vault/reference/tech/systems/sys-upgrades.md)
- expected_output: one flavor line per upgrade stat; the stat name stays; numbers unchanged. File Content/RogueRigs/Data/Text/upgrade_texts.json. Agent output files are pure JSON or CSV: no prose, no markdown. A failing item never stays in Content/ or in a vault node; failures go to Tools/content/logs/ only.
- done_when: validate_text.js exits 0; the file is in Content/RogueRigs/Data/Text/; retrieval log in Tools/content/logs/; every item reviewed by the critic with a verdict; failures corrected within 2 loops or reported to the owner.
- editor_state: any
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-076-upgrade-texts]] — parent global task.
