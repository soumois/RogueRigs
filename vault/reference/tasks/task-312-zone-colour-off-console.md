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
- executor_tokens: input 96, cache write 625597, cache read 2995659, output 24268
- executor_usage: runs 2, turns 44, context start 51k peak 113k, resumes 6, 128 min, units 1.20 M
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Cause: the deferred zone decal painted every decal receiver in its box (console, bonus chest, drone meshes). Fix: telegraph owners and the drone opt out of decals (8a276ed5). Hacking capture before/after in Saved/ZoneColour/ shows no wash. Not captured: Locked state with miner inside, chest zone (no debug command reaches them); chest covered by unit test only. Owner 2026-10-24: closed as is ("Oui, clos la task-312 telle quelle"), no debug-command follow-up.

## Subtasks
1. task-312-01-telegraph-under-actors — cpp: Ground telegraph under actors
2. task-312-02-capture-zone-colour — cpp: Capture the zone without wash

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-312-01-telegraph-under-actors]] — step 01, cpp.
- [[task-312-02-capture-zone-colour]] — step 02, cpp.
