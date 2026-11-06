---
id: task-447-swirls-alpha-fix
description: "Global task: The rotor wash swirls of task-442 read alpha 0 in the simulation: make them actually show, look values unchanged."
parent: tasks-done-index
kind: task
status: done
---
# Fix the invisible extractor swirls

- request: Owner 2026-11-05, verbatim: "Oui, corrige". It answers the main session's question: "Je fais corriger les volutes et vérifier la particule de mort ?" Context he was given: during task-443, particles dumped in PIE read alpha 0 on 60 of 60 Swirls particles of NS_ExtractorRotorWash, although the asset read-back shows the alpha expression.
- goal: The Swirls of the rotor wash really render with the alpha decided in task-442.
- acceptance: A1 in PIE on a landed extractor, particle values read in the simulation show Swirls particles with a non-zero alpha matching the expression decided in task-442 at the current User.Intensity; a compile state or an asset read-back alone does not count as proof; A2 this is a correction: the look values of task-442 (shape, count, size, colour, intended alpha) are unchanged; A3 the GroundBlast of task-443 and its height response still read as before in the simulation; A4 No capture and no stress run in this task: the on-screen look stays the owner's to judge and the report says so. The report also gives the token cost of this task, per subtask and in total, from cost-log (units, turns, runs, resumes).
- gdd_check: Correction of task-442, no design decision and no gdd edit. The suspected cause is the trap recorded in unreal-mcp-quirks (the last stack edit stays out of the compiled script while the state says UpToDate); not diagnosed here. Nothing open.
- confirmed_by_owner: yes (2026-11-05, "Oui, corrige")
- milestone: M2
- next_step: none
- started: 2026-11-05T22:22:52Z
- waiting_for: none
- executor_tokens: input 124, cache write 139539, cache read 6160028, output 32331
- executor_usage: runs 1, turns 62, context start 17k peak 152k, resumes 0, 10 min, units 0.95 M
- planning_tokens: input 7, cache write 35353, cache read 533294, output 7554
- planning_usage: aeb73265 "Plan FX list tasks" 1/16 (tasks 435, 436, 437, 438, 439, 440, 441, 442, 443, 444, 445, 446, 447, 448, 449, 450), units 0.14 M
- result: Done, look unverified until the owner sees it in game. The Swirls of NS_ExtractorRotorWash now carry their task-442 alpha in the simulation (was 0): Particles.Color set plain then the same expression, saved, proven in PIE after an editor restart (landed Intensity 0.6: A 0.003-0.084, error 0.00000; takeoff Intensity 1: up to 0.1396). Look values unchanged; GroundBlast and its height response read as before (A 0.300 landed, 0.389 at 88.9, 0.198 at 241.98). No capture, no stress run.

## Subtasks
1. task-447-01-swirls-compiled (vault/reference/tasks/task-447-swirls-alpha-fix/task-447-01-swirls-compiled.md) — bp: The compiled Swirls emitter gives particles the alpha the asset declares.
2. task-447-02-doc (vault/reference/tasks/task-447-swirls-alpha-fix/task-447-02-doc.md) — doc: Record the corrected state and how it was proved.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-447-01-swirls-compiled]] — step 1, bp.
- [[task-447-02-doc]] — step 2, doc.
