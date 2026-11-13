---
id: task-518-audio-level-up-not-heard
description: "Global task: The level up sound is heard in its file but not in game: make it audible in game."
parent: tasks-done-index
kind: task
status: done
---
# Son du level up inaudible en jeu

- request: Owner 2026-11-13: « Ou est-ce que je peux avoir le son coupé, car je n'ai pas entendu le level up. » then, after listening to Art/Source/audio/SW_LevelUp.wav alone: « Level up : J'entends bien le son dans le fichier ». The task-511 log says the LevelUp event plays (bUI=True, Volume=1.0). Owner answer 2026-11-13 (relayed by the main session, verbatim): "Q8: 0.15 in et out
Q9 : oui" = the plan and the defaults of Q9 accepted.
- goal: When the player levels up in game, the level up sound is heard as clearly as when its file is played alone.
- acceptance: A1 the cause is stated in the result (for example stopped, masked, too quiet or played on a silenced path), from a -game run with Verbose LogRRAudio at a real level up; A2 after the fix the same run shows the LevelUp play starting at the level up and lasting its full length at the table volume through the Effects class, with no stop; A3 build green, audio tests pass; A4 sys-audio updated if the fix changes how the system works; the owner is told it is unheard by the agent and to check by ear.
- gdd_check: Read audio-direction and sys-audio (level up is a UI-paused sound per task-511 Q5). Defect reported by the owner: not diagnosed by the planner. No conflict, no open value. No code read.
- verification_level: light
- family: comportement
- confirmed_by_owner: yes (2026-11-13, "Q8: 0.15 in et out
Q9 : oui")
- milestone: M2
- next_step: none
- started: 2026-11-13T09:20:31Z
- waiting_for: none
- executor_tokens: input 124, cache write 141591, cache read 6002155, output 43033
- executor_usage: runs 1, turns 62, context start 17k peak 154k, resumes 0, 15 min, units 0.99 M, USD 2.77
- planning_tokens: input 10, cache write 20096, cache read 213155, output 4963
- planning_usage: aec6de57 "Plan shot and wall-impact sound changes" 1/8 (tasks 512, 513, 514, 515, 516, 517, 518, 519), units 0.07 M, USD 0.24
- result: Cause: the level-up sound played in full at a real level up (not stopped, not cut by the pause, Effects class) but reached the output at 0.35 of its file level like every centred play, and 93 % of its energy is below 200 Hz, so the shots and XP pickups of the same moment covered it above 200 Hz. Fix: LevelUp row Volume 2.8 (provisional) = the file level; after run recorded at 0.94-0.99 of the file over its 0.69 s. Added rr.Audio.Record and Verbose End lines; sys-audio updated. Not heard by the agent: owner to check by ear (clips in Saved/Scratch/task-518/). Budget: light level exceeded (62 turns, 0.99 M units) because the diagnosis needed a recording tool, 5 game runs and a signal analysis (the first runs were silent: unfocused window).

## Questions
Decided 2026-11-13 (owner "Q8: 0.15 in et out
Q9 : oui"): every default below is accepted; values provisional, owner 2026-11-13.
- Q9 Level up : l'executor cherche pourquoi le son n'est pas audible en jeu alors qu'il l'est seul (coupé, masqué, trop faible) et corrige ; tu juges à l'oreille. Default: oui.

## Subtasks
1. task-518-01-level-up-audible (vault/reference/tasks/task-518-audio-level-up-not-heard/task-518-01-level-up-audible.md) — cpp: level up sound audible in game: cause found and fixed

## Relations
- [[tasks-done-index]] — parent index.
- [[task-518-01-level-up-audible]] — step 1, cpp.
