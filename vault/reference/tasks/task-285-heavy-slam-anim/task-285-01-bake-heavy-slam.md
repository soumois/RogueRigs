---
id: task-285-01-bake-heavy-slam
description: "Bake the heavy telegraph and slam clips (3d)."
parent: task-285-heavy-slam-anim
kind: task
status: done
---
# Bake the heavy telegraph and slam clips

- goal: Add to the heavy bake, after the walk loop, a telegraph clip (fists raised upwards over the telegraph, held), a slam clip (knees bend and the body leans forward so both fists touch the ground at the contact frame) and a recover clip (back to rest over RecoverDuration 0.5 s); timing (provisional, owner 2026-10-22): telegraph 0.6 s unchanged, fists raised over the first 0.45 s, strike over the last 0.15 s, contact on the OnSlam damage frame.
- agent: 3d
- depends_on: none
- inputs: art-3d-lessons (vault/reference/agents/art-3d-lessons.md), agent-3d (vault/reference/agents/agent-3d.md), robots-meshy-raw-2026-10-22 (vault/reference/tech/pipelines/robots-meshy-raw-2026-10-22.md), art-prompts-robots-meshy (vault/reference/gdd/art-audio/art-prompts-robots-meshy.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), enemy-heavy (vault/reference/gdd/enemies/enemy-heavy.md), sys-enemies-heavy (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-heavy.md); files: Art/Meshes/heavy-meshy/heavy_meshy_parts.fbx and the heavy bake config of task-284
- expected_output: T_HeavyAnim.exr with clips walk, telegraph, slam, recover; anim_report.json with clip ids, frames, contact frame, fist lowest z per frame, feet slide
- done_when: fists at or under 2 cm above the floor at the contact frame and above the head at the end of the telegraph, feet do not move during telegraph, slam and recover, knees bend; the Orchestrator renders the key frames and sends them to the owner; Blender processes stopped
- editor_state: closed (Blender headless only)
- token_ceiling: 60000
- model: sonnet
- tokens: input 52, cache write 106320, cache read 1754940, output 39066
- runs: 1
- milestone: M2
- result: T_HeavyAnim.exr + walk rows unchanged; clip 1 telegraph raise (rows 16-30, 0.45 s, held), clip 2 strike (rows 31-46, contact row 35, play ~0.15 s only: loops), clip 3 recover (rows 47-61, ends at rest, RestClipId 3 stays); fists 0.99/1.12 cm at contact, 261/259 cm above head 246 at telegraph end, feet fixed, knees bend 60 deg; robot_slam_poses.py; renders Saved/Renders/task-285/heavy_{telegraph_end,contact,recover_mid}.png; slam bake error 0.149 cm (half float).

## Relations
- [[task-285-heavy-slam-anim]] — parent global task.
