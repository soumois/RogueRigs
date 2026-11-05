---
id: task-448-death-puffs-sim-check
description: "Global task: Read the robot and heavy death puffs in the simulation on real kills, and fix them only if the compiled values are wrong."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Check the robot death puffs in the simulation

- request: Owner 2026-11-05, verbatim: "Oui, corrige". It answers the main session's question: "Je fais corriger les volutes et vérifier la particule de mort ?" Context he was given: NS_RobotDeathPuff and NS_HeavyDeathPuff of task-436 were only checked by read-back and no robot was killed in that check, so they may carry the same compiled-script defect as the swirls.
- goal: It is known, from the simulation, that both death puffs play the values decided in task-436; if they do not, they are fixed.
- acceptance: A1 in PIE, a real robot kill and a real heavy kill each have their puff particles read in the simulation: every emitter spawns, with non-zero alpha where visible and colours, sizes and lifetimes matching the asset; a compile state or an asset read-back alone does not count as proof; A2 an asset is changed only if its compiled values are wrong; if they are right, no asset changes and the result says so; A3 this is a correction: the look values of task-436 are unchanged; A4 NS_KamikazeDeathPuff, a plain copy of the old puff, is not part of this task; A5 No capture and no stress run in this task: the on-screen look stays the owner's to judge and the report says so. The report also gives the token cost of this task, per subtask and in total, from cost-log (units, turns, runs, resumes).
- gdd_check: Correction check of task-436, no design decision and no gdd edit. Suspected trap in unreal-mcp-quirks; not diagnosed here. Runs before task-446, which reuses this look. Nothing open.
- confirmed_by_owner: yes (2026-11-05, "Oui, corrige")
- milestone: M2
- next_step: task-448-03-doc
- started: 2026-11-05T22:33:16Z
- waiting_for: none
- result: open

## Subtasks
1. task-448-01-robot-puff-check (vault/reference/tasks/task-448-death-puffs-sim-check/task-448-01-robot-puff-check.md) — bp: The robot death puff is read in the simulation on a real kill, and fixed only if its compiled values are wrong.
2. task-448-02-heavy-puff-check (vault/reference/tasks/task-448-death-puffs-sim-check/task-448-02-heavy-puff-check.md) — bp: The heavy death puff is read in the simulation on a real kill, and fixed only if its compiled values are wrong.
3. task-448-03-doc (vault/reference/tasks/task-448-death-puffs-sim-check/task-448-03-doc.md) — doc: Record what the simulation showed for both puffs.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-448-01-robot-puff-check]] — step 1, bp.
- [[task-448-02-heavy-puff-check]] — step 2, bp.
- [[task-448-03-doc]] — step 3, doc.
