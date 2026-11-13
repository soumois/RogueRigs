---
id: task-528-01-takeoff-path
description: "Cpp: takeoff path never below the floor per Q15"
parent: task-528-extractor-takeoff-under-floor
kind: task
status: todo
---
# L'extracteur rentre dans le sol en partant: cpp

- goal: takeoff path never below the floor per Q15
- agent: cpp
- depends_on: none
- inputs: extraction (vault/reference/gdd/run/extraction.md), bp-extraction-shuttle (vault/reference/tech/blueprints/world/bp-extraction-shuttle.md)
- expected_output: see done_when
- done_when: A1 to A4 (code part) hold; commit.
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
- [[task-528-extractor-takeoff-under-floor]] — parent global task.
