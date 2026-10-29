---
id: task-375-02-bp-shuttle-scale-x4
description: "Bp: BP_ExtractionShuttle: Mesh scale 4 (rotors and wash follow), chevron hide distance 150 -> 600 uu."
parent: task-375-extractor-scale-x4
kind: task
status: done
---
# Extractor: four times bigger, with its level-ending zone and footprint: bp

- goal: BP_ExtractionShuttle draws the extractor 4 times bigger and keeps its chevrons out from under the hull (Q1, Q6, Q7).
- agent: bp
- depends_on: task-375-01-cpp-extractor-scale-footprint
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-extraction-shuttle (vault/reference/tech/blueprints/world/bp-extraction-shuttle.md), extraction (vault/reference/gdd/run/extraction.md), task-375-extractor-scale-x4 (vault/reference/tasks/task-375-extractor-scale-x4.md) request and Questions (decided values), /Game/RogueRigs/Blueprints/Objectives/BP_ExtractionShuttle
- expected_output: inherited Mesh relative scale (4, 4, 4), location unchanged (pivot bottom centre stays on the floor); rotors keep their relative locations on Mesh; the task-374 wash components stay under the rotors (reported, not retuned); OnCountdown chevron visibility threshold 600 uu instead of 150, chevron scale and 150-600 uu spacing unchanged; Blueprint compiled and saved; one commit; change summary.
- done_when: Blueprint compiles without error; the scale and threshold values read back from the asset; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: sonnet
- tokens: input 38, cache write 52905, cache read 1443592, output 9997
- usage: turns 18, context start 58k peak 97k, resumes 0, 1 min, units 0.26 M
- runs: 1
- milestone: M2
- started: 2026-10-29T23:45:04Z
- waiting_for: none
- result: Mesh scale 4 (rotors and wash attached to Mesh follow), chevron threshold 150 -> 600 read back; compiled, saved; commit b112688b; no PIE check

## Relations
- [[task-375-extractor-scale-x4]] — parent global task.
