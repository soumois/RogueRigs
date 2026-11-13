---
id: task-528-03-doc
description: "Doc: bp-extraction-shuttle: takeoff path"
parent: task-528-extractor-takeoff-under-floor
kind: task
status: in-progress
---
# L'extracteur rentre dans le sol en partant: doc

- goal: bp-extraction-shuttle: takeoff path
- agent: doc
- depends_on: task-528-02-capture
- inputs: bp-extraction-shuttle (vault/reference/tech/blueprints/world/bp-extraction-shuttle.md), extraction (vault/reference/gdd/run/extraction.md)
- expected_output: see done_when
- done_when: A4 doc part holds; lint clean; commit.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-13T14:25:45Z
- waiting_for: none
- result: open

## Relations
- [[task-528-extractor-takeoff-under-floor]] — parent global task.
