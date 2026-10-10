---
id: task-086-01-a4-readme-update
description: "A4 README updated with the new rule T9 catch, the voice-tweak comparison and the corrected display status."
parent: task-086-a4-readme-update
kind: task
status: todo
---
# A4 README update

- goal: A4 README updated with the new rule T9 catch, the voice-tweak comparison and the corrected display status.
- agent: claude
- depends_on: task-082-02-shuttle-lines, task-083-02-voice-tweak, task-085-01-alert-line-once
- inputs: assignment-4-content-pipeline (vault/reference/course/assignments/assignment-4-content-pipeline.md), assignment-4-plan (vault/reference/course/assignments/assignment-4-plan.md), dialogue-tone (vault/reference/gdd/art-audio/dialogue-tone.md)
- expected_output: update assignments/A4-dynamic-content-pipeline/README.md and the root README section from the real new logs: (1) consistency checking: add the owner-found misreading of "Cease all extraction" and how it led to T9, the rewrite, its critic review and result; (2) voice judgment: replace the "no tweak recorded" paragraph by the real tweak of task-083 with v1 and v2 lines side by side and the critic verdicts, state honestly if v2 was not adopted; (3) the numbers table; (4) owner decision 2026-10-10: remove the "What is not verified" section from the A4 README (and its entry in the intro map); the in-game display is described in the relevant sections only with what is true (what is built, what the owner has seen on screen: alert line seen, card line after the fix only if the owner confirms), without a claim that was not checked, and the barks stated as data files loaded but not displayed. Keep the earlier honest notes; nothing not produced.
- done_when: every claim traced to a file or log; links resolve; lint clean.
- editor_state: any
- token_ceiling: 20000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-086-a4-readme-update]] — parent global task.
