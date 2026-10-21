---
id: task-260-rock-dig-time-065
description: "Global task: Rock blocks take 0.65 s instead of 0.5 s to dig at mining speed 1."
parent: tasks-done-index
kind: task
status: done
---
# Rock dig time 0.65 s

- request: rock health from 0.5 to 0.65 (owner, 2026-10-20)
- goal: Rock blocks take 0.65 s instead of 0.5 s to dig at mining speed 1.
- acceptance: Build passes; RogueRigs.Dig, RogueRigs.Ore, RogueRigs.Integration.FineContactDigOpensPassage and RogueRigs.RunFlow pass; docs and lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-20).
- confirmed_by_owner: yes (2026-10-20)
- milestone: M1
- next_step: none
- result: Done. RockHealth 0.5 -> 0.65 (provisional, owner 2026-10-20); no override anywhere; explosions still destroy rock in one hit, shots do not damage rock, damage visuals use fractions; BarrelHealth kept at 0.5 (owner may want 0.65). Tests updated (rock breaks at frame 39), 27 of 27 pass; GDD drone and ore-blocks plus 6 sys docs updated. Not seen in play.

## Subtasks
1. task-260-01-rock-dig-time-065 — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-260-01-rock-dig-time-065]] — step 1, cpp.
