---
id: task-214-01-excerpt-fix
description: "text: In Tools/content/logs/20261010-103303-barks.json, replace the two excerpts 'Jokes target the company.' with 'Jokes target the company' and r"
parent: task-214-barks-log-excerpt
kind: task
status: done
---
# excerpt-fix

- goal: In Tools/content/logs/20261010-103303-barks.json, replace the two excerpts "Jokes target the company." with "Jokes target the company" and rerun the validator on barks.json until it exits 0; change no bark text.
- agent: text
- depends_on: none
- inputs: agent-text (vault/reference/agents/agent-text.md), dialogue-tone (vault/reference/gdd/art-audio/dialogue-tone.md), text-content-validator (vault/reference/tech/pipelines/text-content-validator.md), text-run-log-schema (vault/reference/tech/pipelines/text-run-log-schema.md), Tools/content/logs/20261010-103303-barks.json, Content/RogueRigs/Data/Text/barks.json
- expected_output: The corrected log file.
- done_when: validator exits 0 on barks.json with its log; only the two excerpts changed (git diff); committed.
- editor_state: any
- token_ceiling: 15000
- model: sonnet
- tokens: input 8, cache write 11590, cache read 30758, output 76
- runs: 1
- milestone: M2
- result: Log 20261010-103303-barks.json: the two excerpts now read Jokes target the company (lines 106 and 241, 2a200272); validator exits 0 on barks.json with its log and review; no bark text changed.

## Relations
- [[task-214-barks-log-excerpt]] — parent global task.
