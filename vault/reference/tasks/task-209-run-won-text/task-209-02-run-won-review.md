---
id: task-209-02-run-won-review
description: "style-guide: Review the run-won text of task-209-01 against L1, L2, L4 and T1 to T9."
parent: task-209-run-won-text
kind: task
status: done
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
- model: opus
- tokens: input 10, cache write 27997, cache read 54308, output 1175
- runs: 1
- milestone: M2
- result: 3 pass, 1 fail: CLAIM STRIPPED pass, subtitle pass, ROCK EMPTIED, RIGS GONE pass (may read as crew wiped out), FIRST HERE, FIRST OUT fail T3 (paraphrased stock phrase first in, first out; fix LANDED FIRST, LEFT LOADED). Owner 2026-10-17 kept CLAIM STRIPPED with its subtitle, option A for the validator.

## Relations
- [[task-209-run-won-text]] — parent global task.
