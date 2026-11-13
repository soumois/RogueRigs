---
id: task-517-02-dig-import
description: "Bp: owner dig file imported, manifest entry, set on the line; sys-audio line"
parent: task-517-audio-dig-sound
kind: task
status: todo
---
# Son quand on creuse: bp

- goal: owner dig file imported, manifest entry, set on the line; sys-audio line
- agent: bp
- depends_on: task-517-01-dig-event
- inputs: sys-audio (vault/reference/tech/systems/core/sys-audio.md), Art/Source/audio/sound_manifest.json
- expected_output: see done_when
- done_when: A3 holds and sys-audio updated; if the owner file is not provided, set wait-answers and ask for it; commit.
- editor_state: editor closed
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
- [[task-517-audio-dig-sound]] — parent global task.
