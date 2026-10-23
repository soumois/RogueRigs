---
id: task-309-03-capture-chest-opening
description: "Capture the chest opening (cpp): bonus chest opening: latch, lid, particle and xp burst, step 03."
parent: task-309-chest-opening
kind: task
status: todo
---
# Capture the chest opening

- goal: Headless -game run with cheats: frames of the latch swinging, the lid opening, the particle and the three chips flying; contact sheet for the owner.
- agent: cpp
- depends_on: task-309-02-bp-chest-open-fx
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), bp-bonus-chest (vault/reference/tech/blueprints/world/bp-bonus-chest.md), objective-bonus-chest (vault/reference/gdd/run/objective-bonus-chest.md); files: none
- expected_output: Saved/ChestOpening/ frames and contact sheet, report
- done_when: latch, lid, particle and chips visible in order; sheet sent to the owner; nothing else changed
- editor_state: closed
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-309-chest-opening]] — parent global task.
