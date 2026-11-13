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
- result: Done. A1 cause (-game runs, Verbose LogRRAudio, game mix recorded with the new rr.Audio.Record): the LevelUp play was neither stopped, cut by the pause nor on a silenced path; it played in full in the output (correlation 0.91 with the file) but at 0.35 of the file level, like every centred play, and 93 % of SW_LevelUp is below 200 Hz (79 % below 100 Hz): above 200 Hz it sat 2 to 20 dB under the shots and the XP pickups that start in the same frame, so it was masked and too quiet. Fix: LevelUp row Volume 1.0 -> 2.8 = the level of the file played alone (provisional, by ear). A2 after run: Play LevelUp at a real level up, volume 2.800, class SC_Effects, no cap stop; recorded at gain 0.94 to 0.99 of the file over its full 0.69 s (+8.5 dB in the mix); a play without pause logs End after 1.70 s (sound 0.69 s); the real level-up play logs no End line because the choice pause starts before the engine reports it (known UI-row gotcha), the recording shows it whole. 6 clipped samples in a busy moment. A3 build green; no audio test family exists, build only. A4 sys-audio updated (levels, rr.Audio.Record, End lines, unfocused window silent). Unheard by the agent: owner to check by ear. Listening clips (temporary): Saved/Scratch/task-518/. Commit a2e61012.

## Questions
Decided 2026-11-13 (owner "Q8: 0.15 in et out
Q9 : oui"): every default below is accepted; values provisional, owner 2026-11-13.
- Q9 Level up : l'executor cherche pourquoi le son n'est pas audible en jeu alors qu'il l'est seul (coupé, masqué, trop faible) et corrige ; tu juges à l'oreille. Default: oui.

## Subtasks
1. task-518-01-level-up-audible (vault/reference/tasks/task-518-audio-level-up-not-heard/task-518-01-level-up-audible.md) — cpp: level up sound audible in game: cause found and fixed

## Relations
- [[tasks-done-index]] — parent index.
- [[task-518-01-level-up-audible]] — step 1, cpp.
