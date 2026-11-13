---
id: task-516-02-explosion-import
description: "Bp: owner explosion file imported, manifest updated (placeholder marks)"
parent: task-516-audio-explosion-edge-distance
kind: task
status: todo
---
# Son des explosions selon la distance au bord de l'explosion: bp

- goal: owner explosion file imported, manifest updated (placeholder marks)
- agent: bp
- depends_on: task-516-01-explosion-event
- inputs: sys-audio (vault/reference/tech/systems/core/sys-audio.md), Art/Source/audio/sound_manifest.json
- expected_output: see done_when
- done_when: A3 holds; if the owner file is not provided, set wait-answers and ask for it; commit.
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
- [[task-516-audio-explosion-edge-distance]] — parent global task.
