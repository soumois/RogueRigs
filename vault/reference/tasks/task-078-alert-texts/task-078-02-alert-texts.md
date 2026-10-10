---
id: task-078-02-alert-texts
description: "Critic review of alerts.json: verdict, rule id, reason and correction per item."
parent: task-078-alert-texts
kind: task
status: todo
---
# Critic review (alert-texts)

- goal: review alerts.json against the numbered rules and return a verdict per item.
- agent: style-guide
- depends_on: task-078-01-alert-texts
- inputs: agent-style-guide, dialogue-tone, lore, crew-characters, fictional-films, company-voice (resolved paths in vault/_paths.md); the output file only, never the generator prompt or log
- expected_output: Tools/content/logs/<run-id>.review.json per the text-content-schema (item_id, verdict, rule, reason, suggestion); pure JSON; does not edit the content file. The Orchestrator then sends the fails to the text agent (loop 1, loop 2 at most); a third failure goes to the owner.
- done_when: every item has a verdict; every fail cites an existing rule id; review file kept in logs.
- editor_state: any
- token_ceiling: 20000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-078-alert-texts]] — parent global task.
