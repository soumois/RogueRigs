---
id: task-462-03-doc
description: "Doc: Docs describe the new ground blast and its references."
parent: task-462-extractor-blast-cartoon
kind: task
status: done
---
# Extractor ground blast redone in cartoon style from references: doc

- goal: Docs describe the new ground blast and its references.
- agent: doc
- depends_on: task-462-02-ground-blast
- inputs: bp-extractor-rotor-wash (vault/reference/tech/blueprints/fx/bp-extractor-rotor-wash.md), bp-extraction-shuttle (vault/reference/tech/blueprints/world/bp-extraction-shuttle.md)
- expected_output: see done_when
- done_when: Nodes state the current effect; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-462
- usage: included in the executor run of task-462
- runs: 1
- milestone: M2
- started: 2026-11-06T04:58:10Z
- waiting_for: none
- result: Done (d5b11415): bp-extractor-rotor-wash states the current GroundBlast (M_ExtractorBlastWind puffs and strokes, MaterialRandom split, sizes, alpha 0.6 x Intensity x S, rate 32/s, PIE check values, unverified look) and links extractor-blast-references and task-462; bp-extraction-shuttle assets and the fx index gloss updated; lint clean.

## Relations
- [[task-462-extractor-blast-cartoon]] — parent global task.
