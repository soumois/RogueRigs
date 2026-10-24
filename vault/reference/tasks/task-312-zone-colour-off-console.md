---
id: task-312-zone-colour-off-console
description: "Global task: The hack zone colour stays on the ground: the console, miner and drone inside the zone are no longer washed out."
parent: tasks-done-index
kind: task
status: done
---
# Zone colour does not touch the console

- request: "La console ne doit pas être influencé par la couleur de la zone." (owner, 2026-10-24)
- goal: The hack zone colour stays on the ground: the console, miner and drone inside the zone are no longer washed out.
- acceptance: the ground disc and circles stay on the ground, drawn under actors; inside the zone the console, miner and drone keep their own colours (no wash, lamps and screen readable at play zoom); shared fix in the ground telegraph, so the bonus chest zone gets it too; -game capture before/after for the owner; committed.
- gdd_check: ground-telegraph and objective-console read (2026-10-24): rendering only, the rules are unchanged; no conflict, no open value. Decision 12 (owner 2026-10-24). task-302-02 saw the console washed out inside the light hack disc.
- confirmed_by_owner: yes (2026-10-24, "ok" to the plan and every default; planned in one planning pass, each task run later by its own orchestrator)
- milestone: M2
- next_step: none
- result: Done. Saved/ZoneColour/contact_sheet_zone_colour_before_after.png: Hacking (fill disc) after fix: ring and disc on the ground, console white/teal with bright lamps, drone and miner unwashed; before (task-302) console grey. Locked frame and chest zone not captured (no debug command places the miner in a Locked zone or a real chest zone); unit tests cover chest opt-out.

## Subtasks
1. task-312-01-telegraph-under-actors — cpp: Ground telegraph under actors
2. task-312-02-capture-zone-colour — cpp: Capture the zone without wash

## Relations
- [[tasks-done-index]] — parent index.
- [[task-312-01-telegraph-under-actors]] — step 01, cpp.
- [[task-312-02-capture-zone-colour]] — step 02, cpp.
