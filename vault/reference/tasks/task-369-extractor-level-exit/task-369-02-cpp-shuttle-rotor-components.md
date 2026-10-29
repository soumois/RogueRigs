---
id: task-369-02-cpp-shuttle-rotor-components
description: "Cpp: ARRShuttle gets a body and four rotor mesh components (FL, FR, RL, RR) for the Blueprint to fill; rule unchanged."
parent: task-369-extractor-level-exit
kind: task
status: done
---
# Extractor in game: the level exit you walk up to: cpp

- goal: ARRShuttle exposes one body mesh component and four rotor mesh components attached to it, no collision, the extraction rule unchanged (Q1 A).
- agent: cpp
- depends_on: task-369-01-bp-extractor-import
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), sys-objectives (vault/reference/tech/systems/world/sys-objectives.md), sys-objectives-shuttle (vault/reference/tech/systems/world/sys-objectives/sys-objectives-shuttle.md), sys-objectives-run-flow (vault/reference/tech/systems/world/sys-objectives/sys-objectives-run-flow.md), extraction (vault/reference/gdd/run/extraction.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md)
- expected_output: Source/RogueRigs/Objectives/ ARRShuttle: Mesh kept as body, RotorFL, RotorFR, RotorRL, RotorRR UStaticMeshComponent (VisibleAnywhere, BlueprintReadOnly), NoCollision, no shadow change; a getter for the four rotors; one commit; change summary.
- done_when: build succeeds; BP_ExtractionShuttle still loads; RogueRigs.Extraction.* and RogueRigs.RunFlow.* pass; board radius still 3 cells; committed by path.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: opus
- tokens: input 28, cache write 46352, cache read 429321, output 8493
- usage: turns 14, context start 12k peak 46k, resumes 0, 6 min, units 0.14 M
- runs: 1
- milestone: M2
- started: 2026-10-29T21:07:18Z
- waiting_for: none
- result: ARRShuttle: Mesh is body, RotorFL/FR/RL/RR NoCollision attached to it, GetRotors(); commit bc6422d1; Extraction+RunFlow 8/8 and FineConsoleShuttlePlacement pass.

## Relations
- [[task-369-extractor-level-exit]] — parent global task.
