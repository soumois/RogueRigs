---
id: task-466-03-doc
description: "Doc: Docs describe the new ground wind and the owner references it follows."
parent: task-466-extractor-ground-wind-refs
kind: task
status: cancelled
---
# Extractor ground wind redone from the owner references: doc

- goal: Docs describe the new ground wind and the owner references it follows.
- agent: doc
- depends_on: task-466-02-ground-wind
- inputs: bp-extractor-rotor-wash (vault/reference/tech/blueprints/fx/bp-extractor-rotor-wash.md), extractor-blast-references (vault/reference/tech/blueprints/fx/extractor-blast-references.md), Art/Source/extractor-fx/reference/owner-ground-wind-sheet-2026-11-06.webp, Art/Source/extractor-fx/reference/owner-ground-wind-mockup-2026-11-06.webp
- expected_output: see done_when
- done_when: bp-extractor-rotor-wash describes the new ground blast; extractor-blast-references names the two owner images as the followed references, superseding the web choice of task-462; lint clean; committed.
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
- [[task-466-extractor-ground-wind-refs]] — parent global task.
