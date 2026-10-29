---
id: task-369-02-cpp-shuttle-rotor-components
description: "Cpp: ARRShuttle gets a body and four rotor mesh components (FL, FR, RL, RR) for the Blueprint to fill; rule unchanged."
parent: task-369-extractor-level-exit
kind: task
status: todo
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-369-extractor-level-exit]] — parent global task.
