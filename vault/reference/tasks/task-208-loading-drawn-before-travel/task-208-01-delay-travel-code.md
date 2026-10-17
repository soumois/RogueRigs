---
id: task-208-01-delay-travel-code
description: "cpp: Continue waits until the old-world loading widget has been rendered at least once"
parent: task-208-loading-drawn-before-travel
kind: task
status: todo
---
# delay-travel-code

- goal: Continue waits until the old-world loading widget has been rendered at least once (Slate post-tick or a 2-frame delay) before OpenLevel.
- agent: cpp
- depends_on: none
- inputs: intermission (vault/reference/gdd/run/intermission.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-run (vault/reference/tech/systems/world/sys-run.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: C++ in Source/RogueRigs/Run/ and the controller, sys-run-transition updated.
- done_when: build succeeds; tests pass except the 2 known failures; a rendered -game log shows the frame numbers of loading started, first loading paint and travel; doc updated; committed.
- editor_state: closed
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-208-loading-drawn-before-travel]] — parent global task.
