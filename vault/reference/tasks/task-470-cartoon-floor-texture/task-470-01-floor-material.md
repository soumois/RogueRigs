---
id: task-470-01-floor-material
description: "Bp: The floor material shows image 1 as its painted colour at the flagstone size of image 2, no added relief."
parent: task-470-cartoon-floor-texture
kind: task
status: todo
---
# Floor uses the owner cartoon flagstone texture: bp

- goal: The floor material shows image 1 as its painted colour at the flagstone size of image 2, no added relief.
- agent: bp
- depends_on: none
- inputs: bp-floor-plates-material (vault/reference/tech/blueprints/world/bp-floor-plates-material.md), visual-style (vault/reference/gdd/art-audio/visual-style.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), Art/Source/floor-texture/owner-2026-11-05-floor-cartoon.webp, Art/Source/cartoon-look/reference/owner-2026-11-05-ingame-light-walls.webp
- expected_output: see done_when
- done_when: A1 to A5 and A7 hold in the editor and in a -game frame; the material change and the imported texture are committed.
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
- [[task-470-cartoon-floor-texture]] — parent global task.
