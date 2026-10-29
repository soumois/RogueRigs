---
id: task-374-03-doc-extractor-rotor-wash
description: "Doc: create bp-extractor-rotor-wash, update blueprints-fx-index and bp-extraction-shuttle."
parent: task-374-extractor-rotor-wash
kind: task
status: done
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
- model: sonnet
- tokens: input 12, cache write 37096, cache read 404101, output 4589
- usage: turns 6, context start 59k peak 81k, resumes 0, 1 min, units 0.11 M
- runs: 1
- milestone: M2
- started: 2026-10-29T23:38:15Z
- waiting_for: none
- result: Doc done (5bc81dcf): bp-extractor-rotor-wash created, blueprints-fx-index and bp-extraction-shuttle updated, lint clean.

## Relations
- [[task-374-extractor-rotor-wash]] — parent global task.
