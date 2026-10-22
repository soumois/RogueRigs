---
id: task-286-01-shield-fit-bake
description: "Fit the shield on chaser v2 and bake (3d)."
parent: task-286-shield-chaser-v2
kind: task
status: in-progress
---
# Fit the shield on chaser v2 and bake

- goal: Fit the existing riot shield mesh of the shield chaser onto the left forearm part of chaser v2 (same grip as v1), keep its slots, and bake the four clips of the v1 shield contract (shield run, shield bash, plain run, plain attack; same clip ids, frames and ClipOffset 0 and 2) with the chaser v2 knee walk, as new files; v1 files untouched (owner 2026-10-22).
- agent: 3d
- depends_on: none
- inputs: art-3d-lessons (vault/reference/agents/art-3d-lessons.md), agent-3d (vault/reference/agents/agent-3d.md), robots-meshy-raw-2026-10-22 (vault/reference/tech/pipelines/robots-meshy-raw-2026-10-22.md), art-prompts-robots-meshy (vault/reference/gdd/art-audio/art-prompts-robots-meshy.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), art-prompts-shield (vault/reference/gdd/art-audio/art-prompts-shield.md), sys-enemies-shield-chaser (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-shield-chaser.md); files: Art/Meshes/chaser-v2-meshy/chaser_v2_meshy_parts.fbx and its bake config (task-284), Art/Meshes/shield-chaser/ (v1 shield source, read only), Tools/art/blender/shield_chaser_bake.py
- expected_output: Art/Meshes/shield-chaser-v2/SM_ShieldChaserV2_Meshy_Anim.fbx (slots body, shield, emblem, robot_eye), T_ShieldChaserV2Anim.exr, anim report
- done_when: shield on the left forearm part (PartId of that part), the four clips present with the v1 frame counts, foot slide under 2 cm per cycle, git diff shows no v1 file changed; the Orchestrator renders key frames and sends them to the owner; Blender processes stopped
- editor_state: closed (Blender headless only)
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-286-shield-chaser-v2]] — parent global task.
