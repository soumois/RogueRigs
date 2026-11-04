---
id: task-425-04-light-doc
description: "Doc: document the light and the grid lighting channels"
parent: task-425-miner-floor-wall-light
kind: task
status: done
---
# Miner light that lights only the floor and the walls: doc

- goal: Update bp-miner (FloorWallLight values, revert), sys-grid (lighting channels on floor and block render chunks) and lights-dark-ambient (new miner light entry) from the change summaries and commits.
- agent: doc
- depends_on: task-425-01-grid-lighting-channels, task-425-02-miner-point-light
- inputs: bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), sys-grid (vault/reference/tech/systems/world/sys-grid.md), lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), agent-doc (vault/reference/agents/agent-doc.md)
- expected_output: The three doc nodes updated, lint clean, commit.
- done_when: bp-miner, sys-grid and lights-dark-ambient state the light values, the channels and the revert; lint clean; committed.
- editor_state: none
- token_ceiling: 0.3 M units
- model: sonnet
- tokens: input 82, cache write 140453, cache read 983489, output 18081
- usage: turns 39, context start 13k peak 43k, resumes 2 (message x2), 8 min, units 0.36 M
- runs: 1
- milestone: M2
- started: 2026-11-04T17:00:36Z
- waiting_for: none
- result: Done: bp-miner, sys-grid, lights-dark-ambient updated; commit 77b77872. ALERT units 1.2x ceiling and 2 resumes: the doc agent was resumed by relayed messages during the run; no turn limit. Capture 03 waived on the owner's word, visual check after task-426.

## Relations
- [[task-425-miner-floor-wall-light]] — parent global task.
