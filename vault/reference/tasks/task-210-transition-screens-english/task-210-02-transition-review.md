---
id: task-210-02-transition-review
description: "style-guide: Review the texts of task-210-01 against L1, L2, L4 and T1 to T9."
parent: task-210-transition-screens-english
kind: task
status: done
---
# transition-review

- goal: Review the texts of task-210-01 against L1, L2, L4 and T1 to T9.
- agent: style-guide
- depends_on: task-210-01-transition-copy
- inputs: agent-style-guide (vault/reference/agents/agent-style-guide.md), dialogue-tone (vault/reference/gdd/art-audio/dialogue-tone.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), lore (vault/reference/gdd/lore.md), Content/RogueRigs/Data/Text/
- expected_output: Verdict per item with rule and correction.
- done_when: verdict pass, or the owner decides each fail (recorded in result).
- editor_state: any
- token_ceiling: 15000
- model: opus
- tokens: input 14, cache write 19003, cache read 81286, output 2582
- runs: 1
- milestone: M2
- result: 11 slots pass, 0 fail (review file 20261017-150000-screen_texts.review.json, also covers end_run_won as pass). Notes: both recap lines use {ORE}, each line is replaced separately by the widget; Shop opens later may read as later in the run.

## Relations
- [[task-210-transition-screens-english]] — parent global task.
