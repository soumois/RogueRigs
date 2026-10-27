---
id: task-308-03-capture-chest-lock-light
description: "Capture the chest lock light states (cpp): bonus chest lock light states, step 03."
parent: task-308-chest-lock-light
kind: task
status: todo
---
# Capture the chest lock light states

- goal: Headless -game run with cheats: frames of the chest light red (waiting), orange (miner in the zone, blink faster near the end of the hack than at its start) and green (opened), with the blink visible across frames; contact sheet for the owner.
- agent: cpp
- depends_on: task-308-02-chest-lock-light-drive
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), bp-bonus-chest (vault/reference/tech/blueprints/world/bp-bonus-chest.md), objective-bonus-chest (vault/reference/gdd/run/objective-bonus-chest.md); files: Art/Source/chest/reference/egor-chest-juice.mp4 (owner reference)
- expected_output: Saved/ChestLight/ frames and contact sheet, report
- done_when: each state seen with its colour and blink; orange blink visibly faster late in the hack; sheet sent to the owner; nothing else changed
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-308-chest-lock-light]] — parent global task.
