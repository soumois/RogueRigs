---
id: task-505-01-settings-sound-graphics
description: "Cpp: Sound and Graphics settings of options-menu stored, applied at once and at launch, exposed to the widget."
parent: task-505-options-screen-sound-graphics
kind: task
status: in-progress
---
# Options screen with Sound and Graphics, opened from the main menu: cpp

- goal: Sound and Graphics settings of options-menu stored, applied at once and at launch, exposed to the widget.
- agent: cpp
- depends_on: none
- inputs: options-menu (vault/reference/gdd/run/options-menu.md), audio-direction (vault/reference/gdd/art-audio/audio-direction.md), Source/RogueRigs/Settings/RRGameUserSettings.h
- expected_output: see done_when
- done_when: A3 and A4 of the global hold in a -game run: each graphics change is read back from the engine, volume values reach their groups, values reloaded after a restart; build green, existing tests pass.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-11T21:59:16Z
- waiting_for: none
- result: open

## Relations
- [[task-505-options-screen-sound-graphics]] — parent global task.
