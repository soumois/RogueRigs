---
id: task-075-01-baseline-run
description: "A4 baseline: a small sample of each content type generated without reading the lore, kept as the comparison point."
parent: task-075-baseline-run
kind: task
status: in-progress
---
# A4 baseline run without lore

- goal: A4 baseline: a small sample of each content type generated without reading the lore, kept as the comparison point.
- agent: text
- depends_on: task-074-01-text-validator
- inputs: assignment-4-plan (vault/reference/course/assignments/assignment-4-plan.md), assignment-4-open-decisions (vault/reference/course/assignments/assignment-4-open-decisions.md), agent-text (vault/reference/agents/agent-text.md), upgrades (vault/reference/gdd/weapons/upgrades.md)
- expected_output: a small sample (about 6 items per type) generated without reading lore, crew-characters, fictional-films, company-voice or dialogue-tone, only the schema and the mechanic node. Saved in Tools/content/logs/ as baseline-<run-id>.json, never in Content/ and never loaded in game. Not reviewed by the critic: it only serves as the voice-judgment comparison.
- done_when: baseline file exists in logs only; nothing added to Content/; committed.
- editor_state: any
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-075-baseline-run]] — parent global task.
