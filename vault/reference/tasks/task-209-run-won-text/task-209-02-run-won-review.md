---
id: task-209-02-run-won-review
description: "style-guide: Review the run-won text of task-209-01 against L1, L2, L4 and T1 to T9."
parent: task-209-run-won-text
kind: task
status: todo
---
# run-won-review

- goal: Review the run-won text of task-209-01 against L1, L2, L4 and T1 to T9.
- agent: style-guide
- depends_on: task-209-01-run-won-copy
- inputs: agent-style-guide (vault/reference/agents/agent-style-guide.md), dialogue-tone (vault/reference/gdd/art-audio/dialogue-tone.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), lore (vault/reference/gdd/lore.md), Content/RogueRigs/Data/Text/
- expected_output: Verdict per item with rule and correction.
- done_when: verdict pass, or the owner decides each fail (correction or kept deviation, recorded in result).
- editor_state: any
- token_ceiling: 15000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-209-run-won-text]] — parent global task.
