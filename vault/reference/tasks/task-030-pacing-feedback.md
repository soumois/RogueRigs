---
id: task-030-pacing-feedback
description: "Global task: first play test feedback: enemies from the first second, slower ramp, shooters blended into chaser packs, robots 20 % slower, XP retuned."
parent: tasks-index
kind: task
status: in-progress
---
# Pacing after the first play test

- request: "Un peu trop calme au début. Mais montée en puissance un trop rapide. Aussi, j'ai un groupe de tireur qui est arrivé grouper. Ils doivent se fondre dans la masse lorsqu'ils arrivent, mais pas en paté de juste des tireurs, aussi je pense que c'était trop de tireur. Les monstres sont trop rapide, ralentit leur vitesse de 20%. Tu as trés bien compris le principe de spawn pour que le joueur se fasse entouré." Then: "Tu peux même faire que dés la premiére seconde tu as un enemi qui es spawn." (2026-10-06)
- goal: the level starts at once, ramps up more slowly, shooters arrive blended in chaser packs and are fewer, robots move 20 % slower, level-up pacing still hits its targets.
- acceptance: (1) build exits 0; all tests pass; (2) director: first robot at about 1 s, lone chaser every 2 s, first pack of 6–8 chasers at 15 s, 1–2 packs per wave until 1:30 then 2–3, interval 6 → 3.5 s over 210 s, no shooter-only pack, 1–2 shooters in some chaser packs not in front, shooters about 10 % of robots (tests); (3) speeds chaser 288, shield chaser 288, kamikaze 240, shooter 240, heavy 160 uu/s; (4) bot pacing medians within ±20 % of L2 0:30, L5 1:40, L10 3:10; (5) docs; lint clean; committed.
- gdd_check: Read [[enemies-index]], [[enemy-chaser]], [[enemy-kamikaze]], [[enemy-shooter]], [[enemy-heavy]], [[xp-level-up]]. GDD updated by the Orchestrator with the owner's values (2026-10-06); hordes and surround spawning unchanged; no open value.
- confirmed_by_owner: yes (2026-10-06, plan accepted with the first robot at 1 s)
- milestone: M2
- next_step: task-030-02-pacing-retune
- result: open

## Subtasks
1. task-030-01-director-speeds — cpp: director timings, pack mixes, shooters blended, speeds −20 %, tests
2. task-030-02-pacing-retune — cpp: bot pacing over 3 seeds, XP curve retune, pack arrival shots

## Relations
- [[tasks-index]] — parent index.
- [[task-030-01-director-speeds]] — step 1, cpp.
- [[task-030-02-pacing-retune]] — step 2, cpp.
