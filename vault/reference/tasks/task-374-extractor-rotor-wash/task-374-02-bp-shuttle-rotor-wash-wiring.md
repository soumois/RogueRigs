---
id: task-374-02-bp-shuttle-rotor-wash-wiring
description: "Bp: BP_ExtractionShuttle: four wash components under the rotors, Intensity from OnRotorSpeedChanged, floor height at spawn."
parent: task-374-extractor-rotor-wash
kind: task
status: done
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
- model: sonnet
- tokens: input 72, cache write 159447, cache read 5018910, output 19038
- usage: turns 35, context start 59k peak 203k, resumes 0, 3 min, units 0.80 M
- runs: 1
- milestone: M2
- started: 2026-10-29T23:35:03Z
- waiting_for: none
- result: BP_ExtractionShuttle (178508af): WashFL/FR/RL/RR NiagaraComponents on Mesh at rotor positions, OnRotorSpeedChanged sets User.Intensity, BeginPlay sets User.FloorHeight and initial Intensity, no Tick. PIE on landed extractor: 4 components, Intensity 0.6, FloorHeight 62-75, no LogNiagara error. Unverified: arrival extractor, Intensity 1 in takeoff, visual look.

## Relations
- [[task-374-extractor-rotor-wash]] — parent global task.
