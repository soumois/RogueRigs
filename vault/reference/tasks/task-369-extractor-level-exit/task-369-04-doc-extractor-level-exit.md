---
id: task-369-04-doc-extractor-level-exit
description: "Doc: update bp-extraction-shuttle and sys-objectives-shuttle for the extractor meshes and rotor components."
parent: task-369-extractor-level-exit
kind: task
status: done
---
# Extractor in game: the level exit you walk up to: doc

- goal: The Blueprint and system docs describe the extractor meshes, the rotor components and the removed glow pulse.
- agent: doc
- depends_on: task-369-01-bp-extractor-import, task-369-02-cpp-shuttle-rotor-components, task-369-03-bp-shuttle-extractor-look
- inputs: agent-doc (vault/reference/agents/agent-doc.md), bp-extraction-shuttle (vault/reference/tech/blueprints/world/bp-extraction-shuttle.md), sys-objectives-shuttle (vault/reference/tech/systems/world/sys-objectives/sys-objectives-shuttle.md), blueprints-world-index (vault/reference/tech/blueprints/world/blueprints-world-index.md)
- expected_output: bp-extraction-shuttle and sys-objectives-shuttle updated; one commit.
- done_when: both nodes state the new components, meshes, material and removed pulse with the commit hashes; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: sonnet
- tokens: input 10, cache write 28941, cache read 91793, output 4468
- usage: turns 5, context start 12k peak 33k, resumes 0, 1 min, units 0.07 M
- runs: 1
- milestone: M2
- started: 2026-10-29T21:16:16Z
- waiting_for: none
- result: Doc: bp-extraction-shuttle, sys-objectives-shuttle, blueprints-world-index updated; commit 83c72430; lint OK.

## Relations
- [[task-369-extractor-level-exit]] — parent global task.
