---
id: task-386-09-doc-miner-animation
description: "Doc: update bp-miner and sys-player-classes, create bp-abp-miner for ABP_Miner, BS_Miner_Locomotion and the imported miner assets"
parent: task-386-miner-mixamo-animation
kind: task
status: cancelled
---
# Miner animated with the owner Mixamo clips: doc

- goal: Write the docs of the implementing steps: bp-miner (new mesh, anim class, location, rotation, assets), a new node bp-abp-miner (ABP_Miner graph, BS_Miner_Locomotion axes and samples, SK_Miner, SKEL_Miner, material, the nine sequences), sys-player-classes (URRMinerAnimInstance and its properties) and sys-player if its summary changes.
- agent: doc
- depends_on: task-386-02-miner-anim-instance, task-386-03-miner-skeletal-import, task-386-04-miner-clips-import, task-386-05-miner-locomotion-blendspace, task-386-06-abp-miner, task-386-07-bp-miner-mesh-swap
- inputs: agent-doc (vault/reference/agents/agent-doc.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), sys-player (vault/reference/tech/systems/core/sys-player.md), blueprints-player-index (vault/reference/tech/blueprints/player/blueprints-player-index.md), vault/reference/tech/pipelines/miner-mixamo-clips.md (created by step 01), to create: `vault/reference/tech/blueprints/player/bp-abp-miner.md` (template `vault/templates/blueprint.md`, parent blueprints-player-index, Relations both ends with bp-miner and sys-player-classes), owner words, planner measures and Q answers: global `request`, `gdd_check` and `## Questions` (vault/reference/tasks/task-386-miner-mixamo-animation.md)
- expected_output: bp-miner and sys-player-classes updated, bp-abp-miner created and linked from blueprints-player-index.
- done_when: bp-miner states SK_Miner, ABP_Miner_C, the new location and yaw and no longer SKM_Quinn_Simple or ABP_Unarmed as current values; bp-abp-miner exists with parent class, skeleton, graph, blend space axes and samples and asset paths; sys-player-classes lists URRMinerAnimInstance with its properties and the Q4 rule; `node vault/scripts/lint.js --write` clean; committed by path as `task-386-09-doc-miner-animation: <summary>`.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-386-miner-mixamo-animation]] — parent global task.
