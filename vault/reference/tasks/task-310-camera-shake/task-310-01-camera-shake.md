---
id: task-310-01-camera-shake
description: "Shared camera shake in C++ (cpp): one call plays a light shake on the player camera, step 01."
parent: task-310-camera-shake
kind: task
status: in-progress
---
# Shared camera shake in C++

- goal: Add a camera shake class built in C++ (UCameraShakeBase with a Perlin-noise pattern, no Blueprint asset) and one static call (for example a function library PlayCameraShake(WorldContext, Scale)) that starts it on the local player camera manager; chest preset 0.2 s, about 4 uu translation, no rotation, no distance falloff (default Q20), values EditDefaultsOnly, provisional owner 2026-10-23; no time dilation; do not wire it to any gameplay event (task-309-01 calls it); new doc node sys-camera-shake under systems-core-index with a relation from sys-player; fix only tests the change breaks.
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), sys-player (vault/reference/tech/systems/core/sys-player.md), systems-core-index (vault/reference/tech/systems/core/systems-core-index.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md); files: Source/RogueRigs/ (player camera classes named in sys-player); Art/Source/chest/reference/egor-chest-juice.mp4 (the slam shake to match)
- expected_output: C++ shake class and call, doc node sys-camera-shake, tests green
- done_when: build green; existing suites pass; the call compiles from another actor (used by task-309-01); doc node and relations; lint clean; committed
- editor_state: closed
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-310-camera-shake]] — parent global task.
