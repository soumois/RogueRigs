---
id: task-512-01-shot-volume
description: "Cpp: shot volume lowered in the event table"
parent: task-512-audio-shot-quieter
kind: task
status: done
---
# Tir du joueur plus discret: cpp

- goal: shot volume lowered in the event table
- agent: cpp
- depends_on: none
- inputs: sys-audio (vault/reference/tech/systems/core/sys-audio.md), audio-direction (vault/reference/gdd/art-audio/audio-direction.md), Config/DefaultGame.ini
- expected_output: see done_when
- done_when: A1 to A4 of the global hold; commit of the table change; sys-audio needs no change unless a value is quoted there.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-512
- usage: included in the executor run of task-512
- runs: 1
- milestone: M2
- started: 2026-11-13T08:33:42Z
- waiting_for: none
- result: Config/DefaultGame.ini [/Script/RogueRigs.RRAudioSettings]: the one player shot row Shot (every weapon falls back to it as Shot.<weapon>; no Shot.<weapon> row exists) Volume 1.0 -> 0.5, nothing else changed (commit 020a2331). Build RogueRigsEditor exit 0. -game run on L_ArenaM1 (-RenderOffScreen): rr.Audio.Play Shot and Shot.BurstRifle plays logged on row Shot, 0 LogRRAudio warnings or errors. No RogueRigs audio automation test exists, so none to run. sys-audio quotes no volume, unchanged; audio-direction already holds 0.5. Owner retouch: the Volume= of the Shot line in Config/DefaultGame.ini, by ear.

## Relations
- [[task-512-audio-shot-quieter]] — parent global task.
