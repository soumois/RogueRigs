---
id: task-288-05-cpp-debris-groups
description: "Debris launch for robots with fewer parts (cpp)."
parent: task-288-robot-death-debris
kind: task
status: in-progress
---
# Debris launch for robots with fewer parts

- goal: Check that LaunchDebris handles a DebrisPieces list without arms or without Upper and Lower kinds (kamikaze: Torso, LegL, LegR, Whole only) and caps the thrown count at the pieces available; fix it if not, death rules unchanged (enemy-death).
- agent: cpp
- depends_on: task-288-04-pieces-kamikaze
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), enemy-death (vault/reference/gdd/enemies/enemy-death.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), sys-enemies-kamikaze (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-kamikaze.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md); files: Source/RogueRigs/Enemies/ (ARREnemy LaunchDebris), Art/Source/kamikaze-meshy/debris/debris_pieces.json
- expected_output: C++ change or a report saying none is needed; a test for the 3-piece list; sys-enemies-base doc updated if changed
- done_when: build succeeds, all automation tests pass, the new test throws 2 or 3 kamikaze pieces and never a missing group
- editor_state: closed
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-288-robot-death-debris]] — parent global task.
