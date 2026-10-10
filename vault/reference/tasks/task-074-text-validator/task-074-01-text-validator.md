---
id: task-074-01-text-validator
description: "A4 validator: Tools/content/validate_text.js checks schema, word counts, numbers and log excerpts, with a test on an invalid file."
parent: task-074-text-validator
kind: task
status: in-progress
---
# A4 text validator

- goal: A4 validator: Tools/content/validate_text.js checks schema, word counts, numbers and log excerpts, with a test on an invalid file.
- agent: cpp
- depends_on: task-073-01-text-schema
- inputs: assignment-4-plan (vault/reference/course/assignments/assignment-4-plan.md), agent-text (vault/reference/agents/agent-text.md), dialogue-tone (vault/reference/gdd/art-audio/dialogue-tone.md), upgrades (vault/reference/gdd/weapons/upgrades.md)
- expected_output: Tools/content/validate_text.js (Node, like vault/scripts/lint.js) following the text-content-schema node: validates schema, bark word counts (T1), numbers unchanged against the upgrades node, every log excerpt present word for word in the cited node (ids resolved through vault/_paths.md), no failing item left; exit code non-zero on any failure, one JSON error line per problem. A test: a deliberately invalid sample file under Tools/content/tests/ that must fail, and a valid one that must pass; a command to run both.
- done_when: both test files give the expected exit codes; docs updated in the schema node; lint clean; committed.
- editor_state: any
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-074-text-validator]] — parent global task.
