---
id: task-494-04-doc
description: "Doc: Document the test mode, the command and the test map."
parent: task-494-generator-test-map
kind: task
status: done
---
# Enemy-free test map with a command to generate and reload a new level: doc

- goal: Document the test mode, the command and the test map.
- agent: doc
- depends_on: task-494-03-capture
- inputs: sys-grid (vault/reference/tech/systems/world/sys-grid.md)
- expected_output: see done_when
- done_when: A sys node (or sys-grid) describes the test mode and rr.Level.New; the map has its bp node; lint clean; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-494
- usage: included in the executor run of task-494
- runs: 1
- milestone: M2
- started: 2026-11-07T08:47:14Z
- waiting_for: none
- result: Done (commit cd2682bd). New sys-gen-test-mode (under sys-grid): classes, rr.Level.New, in-place reload flow, seed line, logs, measures, gotchas. sys-layout-loader: StartLoadIntoGrid entry point and link; sys-grid children link; bp-gen-test-game-mode (written in 02) linked to it. Lint clean (65 pre-existing warnings). Budget ALERT continues from subtasks 01 and 03 (minimap fix round).

## Relations
- [[task-494-generator-test-map]] — parent global task.
