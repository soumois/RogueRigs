---
id: task-350-05-turret-doc
description: "Update sys-turret and bp-turret for the fixed base, yawing head and canon-tip shots; create bp-turret-materials for the turret textures, material and meshes."
parent: task-350-turret-in-game
kind: task
status: done
---
# Turret docs: system, Blueprint, materials

- goal: The vault describes the turret as built by subtasks 01 to 03: C++ head pivot and shot origins, BP_Turret components, the imported textures, material, instances and meshes.
- agent: doc
- depends_on: task-350-01-turret-head-yaw, task-350-02-turret-import, task-350-03-bp-turret-meshes, task-350-04-turret-capture
- inputs: agent-doc (vault/reference/agents/agent-doc.md); nodes to update: sys-turret (vault/reference/tech/systems/combat/sys-turret.md), bp-turret (vault/reference/tech/blueprints/player/bp-turret.md), sys-tests-weapon-types (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-weapon-types.md, only if subtask 01 or 04 changed a test); node to create: bp-turret-materials (vault/reference/tech/blueprints/player/bp-turret-materials.md, template `vault/templates/blueprint.md`, parent blueprints-player-index (vault/reference/tech/blueprints/player/blueprints-player-index.md), layout of bp-drone-materials (vault/reference/tech/blueprints/player/bp-drone-materials.md)), with its line in blueprints-player-index and relations on both ends (bp-turret, art-prompts-turret (vault/reference/gdd/art-audio/art-prompts-turret.md)); decisions: global `request` and `gdd_check` (vault/reference/tasks/task-350-turret-in-game.md); commit hashes and change summaries of subtasks 01 to 04 from the Executor (`git show <hash> -- <paths>`).
- expected_output: updated sys-turret, bp-turret (components, events, logic_in_bp now flash only, assets, verified), new bp-turret-materials, index line.
- done_when: the three nodes state the values used (HeadPivot, Muzzle, scale S, materials, slots, bounds) as read back by the agents, with what is not verified; relations on both ends; `node vault/scripts/lint.js --write` clean; committed by path as `task-350-05-turret-doc: <summary>`.
- editor_state: any
- token_ceiling: 0.3 M units
- model: sonnet
- tokens: input 18, cache write 43011, cache read 275582, output 7855
- usage: turns 9, context start 13k peak 46k, resumes 0, 1 min, units 0.12 M
- runs: 1
- milestone: M2
- started: 2026-10-28T16:29:12Z
- waiting_for: none
- result: Done: sys-turret, bp-turret, sys-tests-weapon-types updated, bp-turret-materials created, index, quirks line; lint OK. Commit 77942a95.

## Relations
- [[task-350-turret-in-game]] — parent global task.
