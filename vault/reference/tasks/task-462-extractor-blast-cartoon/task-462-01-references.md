---
id: task-462-01-references
description: "Art: Find and record 3 to 5 cartoon ground wind references and the one the effect follows."
parent: task-462-extractor-blast-cartoon
kind: task
status: done
---
# Extractor ground blast redone in cartoon style from references: art

- goal: Find and record 3 to 5 cartoon ground wind references and the one the effect follows.
- agent: art
- depends_on: none
- inputs: art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), extraction (vault/reference/gdd/run/extraction.md), task-443-extractor-ground-blast (vault/reference/tasks/task-443-extractor-ground-blast.md), Art/Source/extractor-fx/reference/owner-ground-blast-2026-11-05.webp
- expected_output: see done_when
- done_when: A vault node lists each reference with link and what to take, names the chosen one and why; nothing downloaded into the repo; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-462
- usage: included in the executor run of task-462
- runs: 1
- milestone: M2
- started: 2026-11-06T04:40:28Z
- waiting_for: none
- result: Done: extractor-blast-references lists 5 links (Wind Waker puffs realtimevfx t/3213, Wind Waker wakes t/30572 + ArtStation ZeWdJG, Wind Shear 2D t/17282, VFX Practice Wind t/26056, helicopter brownout video); followed: Wind Waker hard-edged puffs with a curl band + tapered curved strokes, brownout motion; nothing downloaded into the repo. Web search engines were refused by the permission classifier, so references came from the realtimevfx.com search API (9ee3959d).

## Relations
- [[task-462-extractor-blast-cartoon]] — parent global task.
