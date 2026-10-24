---
id: task-303-03-capture-hack-reward
description: "Capture the console hack reward (cpp): console hack reward: xp burst and big particle, step 03."
parent: task-303-console-hack-reward
kind: task
status: done
---
# Capture the console hack reward

- goal: Headless -game run with cheats: frames of the hack completing, the three chips flying and the ring flash; contact sheet for the owner.
- agent: cpp
- depends_on: task-303-02-bp-hack-complete-fx
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), objective-console (vault/reference/gdd/run/objective-console.md), bp-objective-console (vault/reference/tech/blueprints/world/bp-objective-console.md); files: none
- expected_output: Saved/ConsoleReward/ frames and contact sheet, report
- done_when: chips and flash visible; sheet sent to the owner; nothing else changed
- editor_state: closed
- token_ceiling: 30000
- model: opus
- tokens: input 50, cache write 76668, cache read 1028877, output 13798
- runs: 1
- milestone: M2
- result: Capture Saved/ConsoleReward/ (contact_sheet_console_reward.png + 12 frames, slomo 0.1): green ring ~200 uu at 0.27 s, ~290 uu at 0.42 s, faded by 0.57 s; white-gold sparks faint, seen at 0.87-1.17 s only; 3 chips fly out, the miner (150 uu away) collected one, 2 rest NE ~290 uu and SE ~210-260 uu; no change, no commit.

## Relations
- [[task-303-console-hack-reward]] — parent global task.
