---
id: task-286-04-capture-shield-v2
description: "Capture the shield chaser v2 (cpp)."
parent: task-286-shield-chaser-v2
kind: task
status: done
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
- model: opus
- tokens: input 40, cache write 69582, cache read 740430, output 9283
- runs: 1
- milestone: M2
- result: Shots Saved/RobotMeshes/shield-v2/: shield on the left forearm while running (shieldchaserv2_c_3, _d_2, _d_3), break with shield fragments (_c_4), plain chaser v2 after the break (_c_7, _d_5, _d_7), parts joined; robot_shots.ps1 -BreakShieldsAt (rr.Debug.BreakShields). Eyes not visible in any frame (shield then miner in front).

## Relations
- [[task-286-shield-chaser-v2]] — parent global task.
