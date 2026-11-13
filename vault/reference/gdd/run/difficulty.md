---
id: difficulty
description: "Two counters per level: the visible alert bar (pauses on mandatory objectives) and hidden threat (never pauses, drives enemy strength); difficulty between levels."
parent: run-index
kind: spec
status: approved
---
# Difficulty

## Two counters per level
| Counter | Grows with | Pauses | Drives |
|---|---|---|---|
| Alert (visible bar) | time + kills | while a mandatory objective is pending | tiers: events and music |
| Threat (hidden) | time + kills | never | enemy health and attack power at spawn, spawn rate |

Threat never pauses so that a paused alert bar cannot be used to farm XP.

## Between levels
- Each level of the run raises enemy health and damage and makes waves denser.

## Inside a level
- Threat raises enemy health and attack power and the spawn rate.
- A threat increase affects only robots spawned afterwards; robots already in the arena keep their stats.

## Overtime (owner 2026-10-06 third test, provisional)
- A level must last 3–4 min. From 3:30 of level time the level enters overtime: the horde gauge of [[level-flow]] fills 2 times faster (each kill counts 2), 3 times faster from 4:30; each horde triggered from 3:30 adds 20 % of base health to robots spawned after it, additive (+20 %, +40 %, ...) on top of threat; and the wave interval halves at each overtime minute, so staying in the level soon becomes untenable (owner 2026-11-13, UI batch, provisional, "en overtime, cela se remplit 2 fois plus vite à 3:30, puis 3 fois plus vite aprés 4:30 et les pv des monstres augmentent de 20% à chaque vagues aprés 3:30"; the 30 s horde timer is removed, hordes come only from the gauge: Planner reading).

## Open values
- Alert and threat gain per second and per kill.
- Health and attack power bonus per (run level, threat) step.
- Spawn rate curve: set by the wave director ([[enemies-index]]); only the per-level variation stays open.

- Last-level profile (owner 2026-10-07): the wave director values tuned before 2026-10-07 (150 alive, waves 7.1 → 4.1 s, packs on distinct sides) are kept as the last-level profile; level 1 uses the softer values in [[enemies-index]]. Not wired yet: the game has one level.

## Relations
- [[run-index]] — parent index.
- [[level-flow]] — what alert tiers trigger.
- [[sys-difficulty]] — how it is built.
