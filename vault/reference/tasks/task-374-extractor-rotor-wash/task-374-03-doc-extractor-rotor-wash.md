---
id: task-374-03-doc-extractor-rotor-wash
description: "Doc: create bp-extractor-rotor-wash, update blueprints-fx-index and bp-extraction-shuttle."
parent: task-374-extractor-rotor-wash
kind: task
status: in-progress
---
# Extractor: rotor-wash effect under the four rotors: doc

- goal: The new Niagara system has its doc node and the shuttle doc lists the wash wiring.
- agent: doc
- depends_on: task-374-01-bp-ns-extractor-rotor-wash, task-374-02-bp-shuttle-rotor-wash-wiring
- inputs: agent-doc (vault/reference/agents/agent-doc.md), blueprints-fx-index (vault/reference/tech/blueprints/fx/blueprints-fx-index.md), bp-extraction-shuttle (vault/reference/tech/blueprints/world/bp-extraction-shuttle.md), new node vault/reference/tech/blueprints/fx/bp-extractor-rotor-wash.md (template vault/templates/blueprint.md)
- expected_output: bp-extractor-rotor-wash created, blueprints-fx-index and bp-extraction-shuttle updated; one commit.
- done_when: nodes written from the change summaries with the commit hashes; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-29T23:38:15Z
- waiting_for: none
- result: open

## Relations
- [[task-374-extractor-rotor-wash]] — parent global task.
