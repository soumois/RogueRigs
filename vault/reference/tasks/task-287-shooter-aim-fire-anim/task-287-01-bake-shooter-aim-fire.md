---
id: task-287-01-bake-shooter-aim-fire
description: "Bake the shooter aim and fire clips (3d)."
parent: task-287-shooter-aim-fire-anim
kind: task
status: done
---
# Bake the shooter aim and fire clips

- goal: Add to the shooter bake, after the walk loop, an aim clip (cannon arm raised to point at target height over the first 0.3 s of the 0.6 s laser, then held) and a fire clip (0.2 s recoil of the cannon forearm and shoulder, back to the aim pose), and report the cannon tip of the aimed pose; feet fixed (owner 2026-10-22: walk, aim and fire clips).
- agent: 3d
- depends_on: none
- inputs: art-3d-lessons (vault/reference/agents/art-3d-lessons.md), agent-3d (vault/reference/agents/agent-3d.md), robots-meshy-raw-2026-10-22 (vault/reference/tech/pipelines/robots-meshy-raw-2026-10-22.md), art-prompts-robots-meshy (vault/reference/gdd/art-audio/art-prompts-robots-meshy.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), enemy-shooter (vault/reference/gdd/enemies/enemy-shooter.md), sys-enemies-shooter (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-shooter.md); files: Art/Meshes/shooter-meshy/shooter_meshy_parts.fbx and the shooter bake config of task-284
- expected_output: T_ShooterAnim.exr with clips walk, aim, fire; anim report with clip ids, frames, cannon tip position per frame
- done_when: cannon horizontal toward +X at the end of the aim clip, recoil returns to the aim pose, feet do not move; the Orchestrator renders key frames and sends them to the owner; Blender processes stopped
- editor_state: closed (Blender headless only)
- token_ceiling: 60000
- model: sonnet
- tokens: input 50, cache write 88313, cache read 1451422, output 28376
- runs: 1
- milestone: M2
- result: T_ShooterAnim.exr: clip 1 aim (rows 16-30, arm up by 0.3 s, cannon horizontal +X within 0.01 deg), clip 2 rest kept, clip 3 fire (recoil peak 0.067 s, back to aim at 0.2 s, error 0); feet fixed; aimed cannon tip Unreal (75.6, 44.0, 109.0) cm from floor; mapping OnAimStart 1, OnFire 3, OnAimEnd 2; renders Saved/Renders/task-287/shooter_{aim_end,recoil_peak,recoil_end}.png.

## Relations
- [[task-287-shooter-aim-fire-anim]] — parent global task.
