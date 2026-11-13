---
id: task-528-01-takeoff-path
description: "Cpp: takeoff path never below the floor per Q15"
parent: task-528-extractor-takeoff-under-floor
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-528
- usage: included in the executor run of task-528
- runs: 1
- milestone: M2
- started: 2026-11-13T14:20:28Z
- waiting_for: none
- result: 4e144e6b: ARRShuttle::TickTakeoff moves along the horizontal heading of the nose (yaw only) plus ClimbRatio x speed upward (new UPROPERTY ClimbRatio 0.1, provisional); pitch 20 deg, rise, rotors, wash unchanged. Build green (C:\Workspace\RogueRigs\RogueRigs.uproject), RogueRigs.Extraction 7/7 pass; Takeoff test now checks the Z never decreases over 60 flight frames and climbs to 405-600 uu.

## Relations
- [[task-528-extractor-takeoff-under-floor]] — parent global task.
