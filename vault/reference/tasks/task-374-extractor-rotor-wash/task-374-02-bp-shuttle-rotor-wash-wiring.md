---
id: task-374-02-bp-shuttle-rotor-wash-wiring
description: "Bp: BP_ExtractionShuttle: four wash components under the rotors, Intensity from OnRotorSpeedChanged, floor height at spawn."
parent: task-374-extractor-rotor-wash
kind: task
status: in-progress
---
# Extractor: rotor-wash effect under the four rotors: bp

- goal: BP_ExtractionShuttle plays the wash under each rotor, driven by the rotor speed event.
- agent: bp
- depends_on: task-374-01-bp-ns-extractor-rotor-wash
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), bp-extraction-shuttle (vault/reference/tech/blueprints/world/bp-extraction-shuttle.md), sys-objectives-shuttle (vault/reference/tech/systems/world/sys-objectives/sys-objectives-shuttle.md)
- expected_output: BP_ExtractionShuttle.uasset: four Niagara components attached under the rotors, OnRotorSpeedChanged sets Intensity, BeginPlay sets the floor height; one commit; change summary.
- done_when: PIE with SkipToExtraction and rr.Run.NextLevel: four wash instances active on the landed and the arrival extractors, Intensity follows the speed, no Event Tick, no log error; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-29T23:35:03Z
- waiting_for: none
- result: open

## Relations
- [[task-374-extractor-rotor-wash]] — parent global task.
