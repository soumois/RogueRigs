---
id: task-404-02-doc-rifle-mesh-offset
description: "Doc: update bp-burst-rifle with the RifleMesh transform and Muzzle"
parent: task-404-rifle-mesh-owner-offset
kind: task
status: todo
---
# Burst rifle mesh at the owner position and scale 1.25: doc

- goal: Document the new RifleMesh relative transform and Muzzle position in bp-burst-rifle (owner values 2026-10-31).
- agent: doc
- depends_on: task-404-01-rifle-mesh-transform
- inputs: agent-doc (vault/reference/agents/agent-doc.md), bp-burst-rifle (vault/reference/tech/blueprints/player/bp-burst-rifle.md), Global request and gdd_check of task-404-rifle-mesh-owner-offset
- expected_output: bp-burst-rifle updated; lint clean; commit `task-404-02-doc-rifle-mesh-offset: ...`.
- done_when: bp-burst-rifle components line gives RifleMesh location (9.898091, -1.129037, -13.0), scale 1.25 (owner 2026-10-31) and the Muzzle value; lint clean; committed by path.
- editor_state: closed
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
- [[task-404-rifle-mesh-owner-offset]] — parent global task.
