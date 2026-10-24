---
id: task-311-03-capture-console-bigger
description: "Capture the bigger console (cpp): console 50 % bigger, step 03."
parent: task-311-console-bigger-50
kind: task
status: done
---
# Capture the bigger console

- goal: Headless -game run: frames of the console after the change at the play zoom and closer, compared with the frames before (task-302 captures in Saved/ConsoleStates/, same zoom), side-by-side sheet for the owner; check the mesh does not overlap rock and the zone circle still fits.
- agent: cpp
- depends_on: task-311-02-bp-console-scale
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), objective-console (vault/reference/gdd/run/objective-console.md), bp-objective-console (vault/reference/tech/blueprints/world/bp-objective-console.md); files: before: Saved/ConsoleStates/blink-fix/contact_sheet_antenna_blink.png, Saved/ConsoleStates/after-fix/contact_sheet_console_states_after_fix.png
- expected_output: Saved/ConsoleBigger/ frames and before/after sheet, report
- done_when: console about 3 cells wide in game, on free floor, head and lamps in place; before/after sheet written; nothing else changed
- editor_state: closed
- token_ceiling: 30000
- model: opus
- tokens: input 58, cache write 113597, cache read 1352232, output 17333
- runs: 1
- milestone: M2
- result: Saved/ConsoleBigger/contact_sheet_console_before_after.png (6 before/after pairs, same cameras; frames cb_* in Saved/ConsoleBigger/, log run1.log): console about 1.5 x bigger (about 200-220 uu, pixel estimate), on open floor, head on the mast and turning in Idle, lamps and screen in place, zone circle fits with margin; no defect from the change; still washed out inside the light hack disc (task-312); no repo change

## Relations
- [[task-311-console-bigger-50]] — parent global task.
