---
id: task-083-02-voice-tweak
description: "Critic review of the v2 upgrade_texts.json (13 lines): verdict, rule id, reason and correction per item."
parent: task-083-voice-tweak
kind: task
status: done
---
# Critic review (voice-tweak)

- goal: review the v2 upgrade_texts.json (13 lines) against the numbered rules (L1-L4, T1-T9) and return a verdict per item; also compare nothing with v1: judge v2 alone.
- agent: style-guide
- depends_on: task-083-01-voice-tweak
- inputs: agent-style-guide, dialogue-tone, lore, crew-characters, fictional-films, company-voice (resolved paths in vault/_paths.md); the output file only, never the generator prompt or log
- expected_output: Tools/content/logs/<run-id>.review.json per text-run-log-schema; pure JSON; does not edit the content file. Then corrections by the text agent (loop 1, loop 2 at most, each re-reviewed); a third failure goes to the owner.
- done_when: every item has a verdict; every fail cites an existing rule id; review file kept in logs.
- editor_state: any
- token_ceiling: 20000
- model: opus
- tokens: input 20, cache write 69021, cache read 153152, output 10642
- runs: 3
- milestone: M2
- result: v2 upgrade lines (per-stat retrieval): critic round 0: 10 pass 3 fail (T3 x2, T2), corrected; round 1: 13 pass; adopted in Content/, v1 kept in Tools/content/logs/v1. Run 20261010-112603-upgrade_texts.

## Relations
- [[task-083-voice-tweak]] — parent global task.
