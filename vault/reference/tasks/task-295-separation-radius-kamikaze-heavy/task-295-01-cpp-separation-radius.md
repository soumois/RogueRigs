---
id: task-295-01-cpp-separation-radius
description: "Add a separation radius scale, 1.3 on kamikaze and heavy, used only by the separation subsystem (cpp)."
parent: task-295-separation-radius-kamikaze-heavy
kind: task
status: in-progress
---
# Per-class separation radius

- goal: ARREnemy EditDefaultsOnly SeparationRadiusScale (default 1.0), set 1.3 in ARRKamikaze and ARRHeavy constructors (check BP_Kamikaze and BP_Heavy do not override); URRRobotSeparationSubsystem uses BodyRadius x scale for the pair contact distance and its bucket size check; GetBodyRadius and capsules unchanged. Update only the tests this breaks.
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), sys-enemies-kamikaze (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-kamikaze.md), sys-enemies-heavy (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-heavy.md), sys-tests-enemies (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-enemies.md); files: Source/RogueRigs/Enemies/RREnemy.h/.cpp, RRRobotSeparationSubsystem.h/.cpp, RRKamikaze.cpp, RRHeavy.cpp
- expected_output: code, sys-enemies-base updated (crowd line) with before and after
- done_when: build clean; enemy tests pass; separation radius values reported for the 5 robot classes (kamikaze 48.6, heavy 62.4); docs updated; lint clean
- editor_state: closed (owner closes it, or Live Coding off)
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-295-separation-radius-kamikaze-heavy]] — parent global task.
