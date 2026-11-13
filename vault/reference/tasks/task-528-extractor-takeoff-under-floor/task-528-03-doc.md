---
id: task-528-03-doc
description: "Doc: bp-extraction-shuttle: takeoff path"
parent: task-528-extractor-takeoff-under-floor
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-528
- usage: included in the executor run of task-528
- runs: 1
- milestone: M2
- started: 2026-11-13T14:25:45Z
- waiting_for: none
- result: d0895e13: sys-objectives-shuttle (forward flight along the horizontal heading, ClimbRatio 0.1, height signal, test) and bp-extraction-shuttle (takeoff_path, verified, relations) updated; Blueprint unchanged. build-toolchain project path corrected to C:\Workspace\RogueRigs\RogueRigs.uproject (old path missing). Lint clean.

## Relations
- [[task-528-extractor-takeoff-under-floor]] — parent global task.
