---
id: task-243-01-card-deal-flip
description: "Design one card back (star emblem, same style as the card frames) as a 1000 x 1440 candidate, prompt saved in the vault."
parent: task-243-card-deal-flip
kind: task
status: in-progress
---
# 243-01 art

- goal: Design one card back (star emblem, same style as the card frames) as a 1000 x 1440 candidate, prompt saved in the vault.
- agent: art
- depends_on: none
- inputs: art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), rarities (vault/reference/gdd/weapons/rarities.md), color-code (vault/reference/gdd/art-audio/color-code.md)
- expected_output: prompt node and candidate PNG under Art/Source, sent to the owner
- done_when: a candidate image exists, is sent to the owner and the prompt is in a vault node
- editor_state: any
- token_ceiling: 30000 (provisional)
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-243-card-deal-flip]] — parent global task.
