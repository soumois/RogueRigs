---
id: task-308-03-capture-chest-lock-light
description: "Capture the chest lock light states (cpp): bonus chest lock light states, step 03."
parent: task-308-chest-lock-light
kind: task
status: done
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
- model: opus+sonnet
- tokens: input 72, cache write 79789, cache read 1349904, output 17664
- usage: turns 33, context start 12k peak 68k, resumes 3 (message x3), 16 min, units 0.32 M
- runs: 3
- milestone: M2
- started: 2026-10-29T17:49:14Z
- waiting_for: none
- result: Not verified by capture: 5 capture launches never framed the latch (default camera too far; C1 not seen, C2-C4 ambiguous/not seen); owner declined an extra cpp rr.Debug framing subtask and checked the chest himself: 'Je valide' (validated visually by the owner, 2026-10-29). Note for the owner: orange blink rate is stepped 2/3/4/6 Hz per hack quarter, not linear. bp-bonus-chest lock_light now notes the C++ runtime override. Cost: capture ~59.9k + ~69.7k, refused cpp attempt ~18k.

## Relations
- [[task-308-chest-lock-light]] — parent global task.
