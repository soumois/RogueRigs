---
id: task-077-02-barks
description: "Critic review of barks.json: verdict, rule id, reason and correction per item."
parent: task-077-barks
kind: task
status: done
---
# Critic review (barks)

- goal: review barks.json against the numbered rules and return a verdict per item.
- agent: style-guide
- depends_on: task-077-01-barks
- inputs: agent-style-guide, dialogue-tone, lore, crew-characters, fictional-films, company-voice (resolved paths in vault/_paths.md); the output file only, never the generator prompt or log
- expected_output: Tools/content/logs/<run-id>.review.json per the text-content-schema (item_id, verdict, rule, reason, suggestion); pure JSON; does not edit the content file. The Orchestrator then sends the fails to the text agent (loop 1, loop 2 at most); a third failure goes to the owner.
- done_when: every item has a verdict; every fail cites an existing rule id; review file kept in logs.
- editor_state: any
- token_ceiling: 20000
- model: opus
- tokens: input 24, cache write 78211, cache read 242234, output 14199
- runs: 3
- milestone: M2
- result: Barks: 34; critic round 0: 32 pass 2 fail (T3, T4) corrected; round 1: 34 pass. Run 20261010-103303-barks.

## Relations
- [[task-077-barks]] — parent global task.
