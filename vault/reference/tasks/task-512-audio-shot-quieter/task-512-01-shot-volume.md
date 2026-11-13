---
id: task-512-01-shot-volume
description: "Cpp: shot volume lowered in the event table"
parent: task-512-audio-shot-quieter
kind: task
status: in-progress
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-13T08:33:42Z
- waiting_for: none
- result: open

## Relations
- [[task-512-audio-shot-quieter]] — parent global task.
