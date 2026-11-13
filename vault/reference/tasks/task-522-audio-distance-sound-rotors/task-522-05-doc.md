---
id: task-522-05-doc
description: "Doc: sys-audio: distance sound classes, rotor loop, how to add a distance sound"
parent: task-522-audio-distance-sound-rotors
kind: task
status: done
---
# Son des hélices de l'extracteur et code générique de son à distance: doc

- goal: sys-audio: distance sound classes, rotor loop, how to add a distance sound
- agent: doc
- depends_on: task-522-04-rotor-loop
- inputs: sys-audio (vault/reference/tech/systems/core/sys-audio.md), audio-direction (vault/reference/gdd/art-audio/audio-direction.md), bp-extraction-shuttle (vault/reference/tech/blueprints/world/bp-extraction-shuttle.md)
- expected_output: see done_when
- done_when: A4 doc part holds; lint clean; commit.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-522
- usage: included in the executor run of task-522
- runs: 1
- milestone: M2
- started: 2026-11-13T14:36:33Z
- waiting_for: none
- result: Done (02021de6). New node sys-audio-distance-sounds (classes, fade and loop rules, users, rotor loop row and checks, how to add a distance sound, debug lines, LogRRAudio header); sys-audio points to it (entry points, call sites, loops, distance_fade, assets); sys-objectives-shuttle rotor sound bullet; bp-extraction-shuttle exposed default RotorSoundEvent (Blueprint not edited); systems-core-index child. Lint clean. Budget ALERT at the 04 closing (81 turns, 1.60 M units for 90 turns, 1.5 M): two resumes of the run, the build collision fix asked by the coordinator, a pre-existing Cast warning traced through a stash build, two -game checks.

## Relations
- [[task-522-audio-distance-sound-rotors]] — parent global task.
