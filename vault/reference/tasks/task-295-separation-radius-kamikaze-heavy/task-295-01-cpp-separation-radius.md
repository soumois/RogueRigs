---
id: task-295-01-cpp-separation-radius
description: "Add a separation radius scale, 1.3 on kamikaze and heavy, used only by the separation subsystem (cpp)."
parent: task-295-separation-radius-kamikaze-heavy
kind: task
status: done
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
- model: opus
- tokens: input 34, cache write 36133, cache read 456633, output 8554
- runs: 1
- milestone: M2
- result: SeparationRadiusScale on ARREnemy (default 1.0), 1.3 on kamikaze and heavy; separation radius kamikaze 37.4 -> 48.6 uu, heavy 48 -> 62.4 uu, chaser 24, shield chaser 24, shooter 34 unchanged; capsules and GetBodyRadius unchanged; build clean, 38 enemy tests pass, no test changed; commits 7569c5a5, 63f60eff

## Relations
- [[task-295-separation-radius-kamikaze-heavy]] — parent global task.
