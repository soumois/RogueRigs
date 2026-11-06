---
id: task-462-02-ground-blast
description: "Bp: The ground blast becomes the cartoon wind of the chosen reference, height fade kept."
parent: task-462-extractor-blast-cartoon
kind: task
status: in-progress
---
# Extractor ground blast redone in cartoon style from references: bp

- goal: The ground blast becomes the cartoon wind of the chosen reference, height fade kept.
- agent: bp
- depends_on: task-462-01-references
- inputs: bp-extractor-rotor-wash (vault/reference/tech/blueprints/fx/bp-extractor-rotor-wash.md), bp-extraction-shuttle (vault/reference/tech/blueprints/world/bp-extraction-shuttle.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), Art/Source/extractor-fx/reference/owner-ground-blast-2026-11-05.webp
- expected_output: see done_when
- done_when: Compiled and saved with no error; in PIE the particles are the new shapes on the floor, fade to none at 400 uu read in the simulation; Extraction tests pass.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-06T04:43:47Z
- waiting_for: none
- result: open

## Relations
- [[task-462-extractor-blast-cartoon]] — parent global task.
