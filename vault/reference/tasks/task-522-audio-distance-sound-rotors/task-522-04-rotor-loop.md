---
id: task-522-04-rotor-loop
description: "Cpp: extractor plays one rotor loop with distance per Q4, Q5"
parent: task-522-audio-distance-sound-rotors
kind: task
status: in-progress
---
# Son des hélices de l'extracteur et code générique de son à distance: cpp

- goal: extractor plays one rotor loop with distance per Q4, Q5
- agent: cpp
- depends_on: task-522-03-sound-import
- inputs: sys-audio (vault/reference/tech/systems/core/sys-audio.md), extraction (vault/reference/gdd/run/extraction.md), bp-extraction-shuttle (vault/reference/tech/blueprints/world/bp-extraction-shuttle.md), Config/DefaultGame.ini
- expected_output: see done_when
- done_when: A3, A4 (code part) hold; commit.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-13T14:28:44Z
- waiting_for: none
- result: open

## Relations
- [[task-522-audio-distance-sound-rotors]] — parent global task.
