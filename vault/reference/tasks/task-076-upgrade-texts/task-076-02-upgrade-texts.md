---
id: task-076-02-upgrade-texts
description: "Critic review of upgrade_texts.json: verdict, rule id, reason and correction per item."
parent: task-076-upgrade-texts
kind: task
status: done
---
# Critic review (upgrade-texts)

- goal: review upgrade_texts.json against the numbered rules and return a verdict per item.
- agent: style-guide
- depends_on: task-076-01-upgrade-texts
- inputs: agent-style-guide, dialogue-tone, lore, crew-characters, fictional-films, company-voice (resolved paths in vault/_paths.md); the output file only, never the generator prompt or log
- expected_output: Tools/content/logs/<run-id>.review.json per the text-content-schema (item_id, verdict, rule, reason, suggestion); pure JSON; does not edit the content file. The Orchestrator then sends the fails to the text agent (loop 1, loop 2 at most); a third failure goes to the owner.
- done_when: every item has a verdict; every fail cites an existing rule id; review file kept in logs.
- editor_state: any
- token_ceiling: 20000
- model: opus
- tokens: input 20, cache write 68352, cache read 149956, output 8851
- runs: 3
- milestone: M2
- result: Upgrade texts: 13 lines; critic round 0: 12 pass 1 fail (L3, magazine size), corrected; round 1: 13 pass. Run 20261010-103239-upgrade_texts.

## Relations
- [[task-076-upgrade-texts]] — parent global task.
