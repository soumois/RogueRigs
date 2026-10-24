---
id: task-312-02-capture-zone-colour
description: "Capture the zone without wash (cpp): zone colour does not touch the console, step 02."
parent: task-312-zone-colour-off-console
kind: task
status: done
---
# Capture the zone without wash

- goal: Headless -game run: frames of the console hack zone (Locked and Hacking) with the miner and drone inside, compared with the washed-out frames before (task-302 captures in Saved/ConsoleStates/), side-by-side sheet for the owner.
- agent: cpp
- depends_on: task-312-01-telegraph-under-actors
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), ground-telegraph (vault/reference/gdd/art-audio/ground-telegraph.md), bp-objective-console (vault/reference/tech/blueprints/world/bp-objective-console.md); files: before: Saved/ConsoleStates/after-fix/contact_sheet_console_states_after_fix.png
- expected_output: Saved/ZoneColour/ frames and before/after sheet, report
- done_when: zone circles visible on the ground; console, miner and drone keep their colours inside the zone; sheet written; nothing else changed
- editor_state: closed
- token_ceiling: 30000
- model: opus
- tokens: input 58, cache write 60101, cache read 1077122, output 18384
- runs: 1
- milestone: M2
- result: Saved/ZoneColour/contact_sheet_zone_colour_before_after.png: Hacking (fill disc) after fix: ring and disc on the ground, console white/teal with bright lamps, drone and miner unwashed; before (task-302) console grey. Locked frame and chest zone not captured (no debug command places the miner in a Locked zone or a real chest zone); unit tests cover chest opt-out.

## Relations
- [[task-312-zone-colour-off-console]] — parent global task.
