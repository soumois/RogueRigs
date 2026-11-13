---
id: task-511-04-audio-doc
description: "Doc: Document the audio system, its event table and how to add a sound."
parent: task-511-audio-system-base-sounds
kind: task
status: done
---
# Audio system and the first 8 sound effects: doc

- goal: Document the audio system, its event table and how to add a sound.
- agent: doc
- depends_on: task-511-03-sound-import
- inputs: audio-direction (vault/reference/gdd/art-audio/audio-direction.md), blueprints-index (vault/reference/tech/blueprints/blueprints-index.md), vault/reference/tech/systems/
- expected_output: see done_when
- done_when: A system node describes the audio system, the event table and the steps to add an event or a sound; audio-direction links it; A6 holds.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-511
- usage: included in the executor run of task-511
- runs: 1
- milestone: M2
- started: 2026-11-13T08:08:47Z
- waiting_for: none
- result: Done (3d19a334). sys-audio created (event table, call sites, caps, button sounds, how to add a sound, debug), linked from systems-core-index, sys-options, sys-button-hover and audio-direction. audio-direction filled with what the owner said and did on 2026-11-13: Q2 palette refused, new palette open; sources 7 ElevenLabs + 1 level-up of unknown origin, licences to confirm; kept assets SW_*. Checked only by log (owner to listen in game): each sound's timing and loudness, on-screen panning of shot/impact/death, the 4/4/3 caps under fire, hover and click on every screen, silence of disabled buttons, the level-up cut at 0.69 s. Open for the owner: ElevenLabs licence, level-up author and licence, new palette, turret and robot shots silent. A4 departs from the plan by the owner's own choice: his picks are not the CC0 candidates.

## Relations
- [[task-511-audio-system-base-sounds]] — parent global task.
