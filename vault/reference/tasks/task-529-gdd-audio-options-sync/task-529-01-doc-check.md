---
id: task-529-01-doc-check
description: "Doc: audio-direction and options-screen checked against the sys nodes after the confirmation edits"
parent: task-529-gdd-audio-options-sync
kind: task
status: todo
---
# GDD audio-direction et options-screen alignés sur ce qui est construit: doc

- goal: audio-direction and options-screen checked against the sys nodes after the confirmation edits
- agent: doc
- depends_on: none
- inputs: audio-direction (vault/reference/gdd/art-audio/audio-direction.md), options-screen (vault/reference/gdd/run/options-screen.md), sys-options (vault/reference/tech/systems/ui/sys-options.md), sys-audio (vault/reference/tech/systems/core/sys-audio.md), sys-audio-distance-sounds (vault/reference/tech/systems/core/sys-audio-distance-sounds.md), task-520-audio-effects-slider-preview (vault/reference/tasks/task-520-audio-effects-slider-preview.md), task-521-options-tab-button-shrinks (vault/reference/tasks/task-521-options-tab-button-shrinks.md), task-522-audio-distance-sound-rotors (vault/reference/tasks/task-522-audio-distance-sound-rotors.md), task-523-audio-impact-near-quieter (vault/reference/tasks/task-523-audio-impact-near-quieter.md), task-527-audio-zone-objective-complete (vault/reference/tasks/task-527-audio-zone-objective-complete.md)
- expected_output: see done_when
- done_when: A1 to A4 of the global hold on the two gdd nodes as committed at confirmation; every value of the lines of task-520, 522, 523, 527 equals its sys node; any mismatch or stale sentence left is reported in the result with the node and line, not fixed in a gdd node; no game run, no capture; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-529-gdd-audio-options-sync]] — parent global task.
