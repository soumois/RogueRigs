---
id: task-082-02-shuttle-lines
description: "Critic review of the rewritten alerts.json (15 lines): verdict, rule id, reason and correction per item."
parent: task-082-shuttle-lines
kind: task
status: done
---
# Critic review (shuttle-lines)

- goal: review the rewritten alerts.json (15 lines) against the numbered rules (L1-L4, T1-T9) and return a verdict per item; T9 first: no line may order the player to stop or contradict the objective.
- agent: style-guide
- depends_on: task-082-01-shuttle-lines
- inputs: agent-style-guide, dialogue-tone, lore, crew-characters, fictional-films, company-voice (resolved paths in vault/_paths.md); the output file only, never the generator prompt or log
- expected_output: Tools/content/logs/<run-id>.review.json per text-run-log-schema; pure JSON; does not edit the content file. Then corrections by the text agent (loop 1, loop 2 at most, each re-reviewed); a third failure goes to the owner.
- done_when: every item has a verdict; every fail cites an existing rule id; review file kept in logs.
- editor_state: any
- token_ceiling: 20000
- model: opus
- tokens: input 22, cache write 73194, cache read 186876, output 10668
- runs: 3
- milestone: M2
- result: Shuttle and extraction lines rewritten under new rule T9; critic round 0: 11 pass 4 fail (3 T9, 1 T6), corrected; round 1: 15 pass. Run 20261010-112240-alerts.

## Relations
- [[task-082-shuttle-lines]] — parent global task.
