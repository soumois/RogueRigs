---
id: task-283-10-capture-after
description: "Capture the robots after integration (cpp)."
parent: task-283-robot-mesh-integration
kind: task
status: todo
---
# Capture the robots after integration

- goal: With the editor closed, build, run the automation tests touching the enemies, then rerun the exact capture command of 283-01 and compare each robot with its before shot.
- agent: cpp
- depends_on: task-283-09-bp-shooter-mesh
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md); files: Saved/RobotMeshes/before/*.png and the 283-01 command
- expected_output: Saved/RobotMeshes/after-integration/*.png and a before/after description per robot
- done_when: heavy, kamikaze and shooter show their Meshy mesh at the expected size, eyes red, no grey-box primitive left, the kamikaze core pulses; existing enemy tests pass; the Orchestrator sends both shot sets to the owner
- editor_state: closed
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-283-robot-mesh-integration]] — parent global task.
