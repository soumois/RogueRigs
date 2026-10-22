---
id: task-286-04-capture-shield-v2
description: "Capture the shield chaser v2 (cpp)."
parent: task-286-shield-chaser-v2
kind: task
status: todo
---
# Capture the shield chaser v2

- goal: With the editor closed, capture a shield chaser v2 running with its shield and after its shield breaks, compared with the 283-01 v1 shot.
- agent: cpp
- depends_on: task-286-03-bp-spawner-shield-v2
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md); files: the 283-01 capture command and shots
- expected_output: Saved/RobotMeshes/shield-v2/*.png and a description
- done_when: shield on the left forearm while running, plain chaser v2 after the break, parts joined, eyes follow the head; the Orchestrator sends the shots to the owner
- editor_state: closed
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-286-shield-chaser-v2]] — parent global task.
