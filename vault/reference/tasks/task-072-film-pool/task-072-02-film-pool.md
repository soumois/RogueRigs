---
id: task-072-02-film-pool
description: "Critic review of the film list: verdict, rule id, reason and correction per item."
parent: task-072-film-pool
kind: task
status: todo
---
# Critic review (film-pool)

- goal: review the film list against the numbered rules and return a verdict per item.
- agent: style-guide
- depends_on: task-072-01-film-pool
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
- [[task-072-film-pool]] — parent global task.
