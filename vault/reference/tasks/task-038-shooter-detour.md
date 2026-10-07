---
id: task-038-shooter-detour
description: "Global task: Shooter without line of sight walks around rock like the other robots until it can fire, never moving away from the miner."
parent: tasks-index
kind: task
status: done
---
# Shooter detour without line of sight

- request: "Le tireur est bloqué, car je suis à portée, mais pas de ligne de vue, les autres font bien le tour." (owner, 2026-10-07)
- goal: Shooter without line of sight walks around rock like the other robots until it can fire, never moving away from the miner.
- acceptance: build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: none
- result: Done. Shooter Reposition now follows the flow field like chasers until line of sight, then holds; steps within 960 uu filtered so it never moves away (URREnemyMovementComponent::SetDirectionFilter); old cell search removed. RepositionsForSight updated (0.00 uu away). Build 0, 195/195. Commit ad02319.

## Subtasks
1. task-038-01-shooter-detour — cpp

## Relations
- [[tasks-index]] — parent index.
- [[task-038-01-shooter-detour]] — step 1, cpp.
