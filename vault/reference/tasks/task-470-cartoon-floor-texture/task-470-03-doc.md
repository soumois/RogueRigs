---
id: task-470-03-doc
description: "Doc: Docs describe the new floor material and texture."
parent: task-470-cartoon-floor-texture
kind: task
status: todo
---
# Floor uses the owner cartoon flagstone texture: doc

- goal: Docs describe the new floor material and texture.
- agent: doc
- depends_on: task-470-02-capture
- inputs: bp-floor-plates-material (vault/reference/tech/blueprints/world/bp-floor-plates-material.md), visual-style (vault/reference/gdd/art-audio/visual-style.md)
- expected_output: see done_when
- done_when: bp-floor-plates-material describes the new texture, its size and what was removed; lint clean; committed.
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
