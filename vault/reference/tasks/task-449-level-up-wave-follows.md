---
id: task-449-level-up-wave-follows
description: "Global task: The gold level-up floor wave follows the miner while it plays, instead of staying where he levelled up."
parent: tasks-done-index
kind: task
status: done
---
# Level-up floor wave follows the miner

- request: Owner 2026-11-05 (verbatim): "Prépare les tasks pour faire que la particule de level up suive le joueur et retire le text Level up dans l'ui ( en bleu ). Prépare juste les tâches". This task is the point "que la particule de level up suive le joueur". His screenshot: Art/Source/level-up-fx/20261105-owner-two-level-up-texts.webp. Owner remark on it, 2026-11-05 (verbatim, it says may be, it is no measure): "Le grand anneau jaune dans la sphère orange peut être l’onde de level up restée sur place". Owner answer 2026-11-05 (relayed by the main session, verbatim): "Ok, tout par défaut" = the plan and the defaults of Q21 accepted.
- goal: A miner who keeps running stays at the centre of his level-up wave.
- acceptance: A1 in game, while the gold ring and shards of the level-up wave play, they stay centred on the miner as he runs, flat on the floor, instead of being left behind as in Art/Source/level-up-fx/20261105-owner-two-level-up-texts.webp (the ring there is believed to be this wave, not proved from one frame; if it proves to be something else, the report says so); A2 this is read in the simulation: ring and shard positions relative to the miner during a level-up with the miner moving; a compile state or an asset read-back alone does not count; A3 size, life, colours and sprite count of the wave are unchanged; A4 the robot push is unchanged: one blow at the moment of the level-up, from where the miner stands then, not repeated while the wave follows him; A5 the gold text keeps its behaviour (Q21), the heal ring, the 0.8 s delay and the card screen are unchanged; A6 existing tests pass, only expectations this change breaks are updated; A7 No capture and no stress run in this task unless the owner asks: he validates the look in game himself; the final report lists what to look at and says plainly that the look is unverified until he has seen it. The report also gives the token cost of this task, per subtask and in total, from cost-log (units, turns, runs, resumes).
- gdd_check: Read xp-level-up, sys-level-up, bp-level-up-burst, task-441. Nothing conflicts; xp-level-up edited at confirmation. task-441 made the push a real one-shot (A3 of task-441) and the wave a one-shot, so following changes the look only and the push stays as it is. The owner names "text" and "particule au sol" apart in task-441, hence the default of Q21. The orange sphere in his screenshot is an explosion and is not part of this task. Rough size check from that one frame (miner about 130 px for 176 uu, perspective ignored): the yellow ring is about 680 uu across with a band of about 65 uu. bp-level-up-burst gives the wave at most 599 uu across with a band of 30 % then 5 % of the radius, so a band this wide means an early, smaller ring; bp-explosion-shockwave gives a constant 60 uu band growing to the radius, about 655 uu across for the kamikaze. So the frame fits the explosion ring at least as well as the wave: not settled, the Executor reports what it finds. The wave follows the miner either way. Does not touch the assets of tasks 446, 447, 448.
- confirmed_by_owner: yes (2026-11-05, "Ok, tout par défaut")
- milestone: M2
- next_step: none
- started: 2026-11-05T22:42:52Z
- waiting_for: none
- result: Done. Commit 5e984eb5. sys-level-up (burst entry, GetWaveLocation, GetFollowedWave, tick only while the wave plays, test figures) and bp-level-up-burst (purpose, verified, unverified) state that the wave follows the miner, the text stays and the push is one blow. Lint clean.

## Questions
Decided 2026-11-05 (owner "Ok, tout par défaut"): every default below is accepted; values provisional, owner 2026-11-05.
- Q21 « La particule de level up » : laquelle suit le joueur ? Default: l'onde dorée au sol et ses éclats, car tu parles toujours du texte à part ; le texte doré « LEVEL UP » reste là où il apparaît, et le joueur peut donc le dépasser en courant. Le recul des robots reste un seul coup au moment du level up, il ne se répète pas pendant que l'onde te suit. L'autre choix : le texte doré te suit aussi.

## Subtasks
1. task-449-01-wave-follows (vault/reference/tasks/task-449-level-up-wave-follows/task-449-01-wave-follows.md) — cpp: The level-up wave stays on the miner while it plays, the push staying one blow.
2. task-449-02-doc (vault/reference/tasks/task-449-level-up-wave-follows/task-449-02-doc.md) — doc: Record that the wave follows the miner.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-449-01-wave-follows]] — step 1, cpp.
- [[task-449-02-doc]] — step 2, doc.
