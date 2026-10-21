---
id: task-259-ore-dig-time-2s
description: "Global task: Ore blocks (ore, special ore and heal ore) take 2 s instead of 1.5 s to dig at mining speed 1."
parent: tasks-done-index
kind: task
status: done
---
# Ore dig time 2 s

- request: ore blocks go back to 2 s (owner, 2026-10-20)
- goal: Ore blocks (ore, special ore and heal ore) take 2 s instead of 1.5 s to dig at mining speed 1.
- acceptance: Build passes; RogueRigs.Dig, RogueRigs.Ore, RogueRigs.Integration.FineContactDigOpensPassage and RogueRigs.RunFlow pass; docs and lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-20).
- confirmed_by_owner: yes (2026-10-20)
- milestone: M1
- next_step: none
- result: Done. OreHealth, HealOreHealth and SpecialOreHealth 1.5 -> 2.0 (provisional, owner 2026-10-20); DropInterval also 1.5 -> 2.0 so non-deposit ore blocks keep one pickup at the break (beyond the brief, kept the old rule); no BP_ArenaGrid, map or table override (OreTypes.csv has no health column). Tests updated for 2 s (pops 30 frames apart, break at frame 120), HealVein test no longer miscounts a pooled pickup; 27 of 27 pass. GDD ore-blocks, heal-ore, drone and sys docs updated. Not seen in play; FineContactDigOpensPassage doc numbers older than the measured ones.

## Subtasks
1. task-259-01-ore-dig-time-2s — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-259-01-ore-dig-time-2s]] — step 1, cpp.
