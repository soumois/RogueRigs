---
id: task-302-01-console-state-drive
description: "Drive the console lamps, head and screen from its state (cpp): console state visuals, step 01."
parent: task-302-console-state-visuals
kind: task
status: in-progress
---
# Drive the console lamps, head and screen from its state

- goal: ARRConsole sets, on each state change, LampColor on both lamp materials (Locked red, Idle orange, Hacking green, Complete green), BlinkRate 2 Hz on MI_ConsoleLampAntenna only (0 on Complete and on the other orange), ScreenColor (black Locked and Idle, green Hacking and Complete) and the AntennaHead yaw speed (0, 10, 45, 0 deg/s); all values EditDefaultsOnly, provisional owner 2026-10-23; the chest keeps its look. GDD objective-console state visuals line; exception "console state lamps" in art-style-guide A7 and color-code; sys-objectives-console-chest doc; fix only tests the change breaks.
- agent: cpp
- depends_on: task-301-console-c2-mesh (done)
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), objective-console (vault/reference/gdd/run/objective-console.md), sys-objectives-console-chest (vault/reference/tech/systems/world/sys-objectives/sys-objectives-console-chest.md), bp-objective-console (vault/reference/tech/blueprints/world/bp-objective-console.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), sys-objectives-events (vault/reference/tech/systems/world/sys-objectives/sys-objectives-events.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md); files: Source/RogueRigs/Objectives/
- expected_output: C++ change, GDD and tech doc lines, tests green
- done_when: build green; RogueRigs.Console.* and RogueRigs.RunFlow.* pass; values and names in the docs; lint clean; committed
- editor_state: closed
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-302-console-state-visuals]] — parent global task.
