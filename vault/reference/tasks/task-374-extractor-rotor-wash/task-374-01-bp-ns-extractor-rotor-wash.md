---
id: task-374-01-bp-ns-extractor-rotor-wash
description: "Bp: create NS_ExtractorRotorWash (downward streaks, floor dust ring, Intensity and floor height parameters)."
parent: task-374-extractor-rotor-wash
kind: task
status: todo
---
# Extractor: rotor-wash effect under the four rotors: bp

- goal: NS_ExtractorRotorWash exists per Q7.
- agent: bp
- depends_on: task-373-01-cpp-extractor-rotor-spin
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), blueprints-fx-index (vault/reference/tech/blueprints/fx/blueprints-fx-index.md), bp-drone-thruster (vault/reference/tech/blueprints/fx/bp-drone-thruster.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), extraction (vault/reference/gdd/run/extraction.md)
- expected_output: /Game/RogueRigs/FX/NS_ExtractorRotorWash (and its material if needed); one commit; change summary (emitters, parameters, colours, counts).
- done_when: the system compiles without error; Intensity 0 emits nothing and 1 emits streaks and the ring; the ring is not emitted when the floor height parameter is more than 300 uu below the system; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-374-extractor-rotor-wash]] — parent global task.
