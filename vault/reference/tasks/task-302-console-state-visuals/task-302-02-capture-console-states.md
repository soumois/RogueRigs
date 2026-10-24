---
id: task-302-02-capture-console-states
description: "Capture the console states in game (cpp): console state visuals, step 02."
parent: task-302-console-state-visuals
kind: task
status: in-progress
---
# Capture the console states in game

- goal: Headless -game run with cheats: frames of the console in Locked, Idle and Hacking (and Complete), lamps, blink, head turn and screen visible; contact sheet for the owner.
- agent: cpp
- depends_on: task-302-01-console-state-drive
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), objective-console (vault/reference/gdd/run/objective-console.md), bp-objective-console (vault/reference/tech/blueprints/world/bp-objective-console.md); files: none
- expected_output: Saved/ConsoleStates/ frames and contact sheet, report
- done_when: each state seen with the right colour and screen; sheet sent to the owner; nothing else changed
- editor_state: closed
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-302-console-state-visuals]] — parent global task.
