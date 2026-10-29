---
id: task-369-04-doc-extractor-level-exit
description: "Doc: update bp-extraction-shuttle and sys-objectives-shuttle for the extractor meshes and rotor components."
parent: task-369-extractor-level-exit
kind: task
status: todo
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-369-extractor-level-exit]] — parent global task.
