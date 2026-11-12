---
id: task-511-01-audio-system
description: "Cpp: Audio system with the event table, wired to the sound classes and to the 8 events, mix rules applied."
parent: task-511-audio-system-base-sounds
kind: task
status: in-progress
---
# Audio system and the first 8 sound effects: cpp

- goal: Audio system with the event table, wired to the sound classes and to the 8 events, mix rules applied.
- agent: cpp
- depends_on: none
- inputs: audio-direction (vault/reference/gdd/art-audio/audio-direction.md), ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md), sys-button-hover (vault/reference/tech/systems/ui/sys-button-hover.md), Content/RogueRigs/Audio/, Source/RogueRigs/Settings/RROptionsSubsystem.h
- expected_output: see done_when
- done_when: A1, A3 caps and the call sites of A2 hold with temporary test sounds (any engine sound, not kept): a -game run log shows each of the 8 events firing at its moment; build green, existing tests pass.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-12T21:01:39Z
- waiting_for: none
- result: open

## Relations
- [[task-511-audio-system-base-sounds]] — parent global task.
