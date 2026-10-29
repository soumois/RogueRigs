---
id: task-375-02-bp-shuttle-scale-x4
description: "Bp: BP_ExtractionShuttle: Mesh scale 4 (rotors and wash follow), chevron hide distance 150 -> 600 uu."
parent: task-375-extractor-scale-x4
kind: task
status: in-progress
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-29T23:45:04Z
- waiting_for: none
- result: open

## Relations
- [[task-375-extractor-scale-x4]] — parent global task.
