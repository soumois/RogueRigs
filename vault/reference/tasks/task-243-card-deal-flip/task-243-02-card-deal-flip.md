---
id: task-243-02-card-deal-flip
description: "Review the card back candidate against the art rules and the colour code."
parent: task-243-card-deal-flip
kind: task
status: done
---
# 243-02 style-guide

- goal: Review the card back candidate against the art rules and the colour code.
- agent: style-guide
- depends_on: task-243-01-card-deal-flip
- inputs: art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md)
- expected_output: verdict and corrections
- done_when: verdict given per item and shown to the owner, who validates or corrects the design
- editor_state: any
- token_ceiling: 15000 (provisional)
- model: open
- tokens: open
- runs: 2
- milestone: M2
- result: style-guide: first round 3 fails A6, owner chose variant 2 with the A6 correction; second round A6 fixed, 1 fail color-code (dark gunmetal and navy field dominant, teal minor); owner keeps it on purpose (deviation voluntary, 2026-10-19).

## Relations
- [[task-243-card-deal-flip]] — parent global task.
