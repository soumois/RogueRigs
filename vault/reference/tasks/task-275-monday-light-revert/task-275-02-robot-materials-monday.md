---
id: task-275-02-robot-materials-monday
description: "Chaser masters, MI_ShieldChaser_Emblem, MI_GB_Shooter and MI_GB_Heavy restored from Monday commit 75610b21 (readability nodes and colour changes gone); doc updated."
parent: task-275-monday-light-revert
kind: task
status: done
---
# Robot materials back to Monday

- goal: remove the task-266-01 and task-270-08 readability changes by restoring the five assets from commit 75610b21; only those two tasks touched them since Monday, so eye glow and animation parameters are the Monday ones (unchanged).
- agent: bp
- depends_on: task-275-01-monday-light-revert
- inputs: bp-robot-readability (vault/reference/tech/blueprints/enemies/bp-robot-readability.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md); files Content/RogueRigs/Art/Chaser/M_Chaser_Meshy_Anim.uasset, Content/RogueRigs/Art/ShieldChaser/M_ShieldChaser_Anim.uasset, Content/RogueRigs/Art/ShieldChaser/MI_ShieldChaser_Emblem.uasset, Content/RogueRigs/Materials/GreyBox/MI_GB_Shooter.uasset, Content/RogueRigs/Materials/GreyBox/MI_GB_Heavy.uasset.
- expected_output: the five files restored with `git checkout 75610b21 -- <paths>`; bp-robot-readability rewritten as reverted (before -> after per parameter, revert recipe = the removed parameters and their last values); no other material touched.
- done_when: `git diff 75610b21 HEAD -- <five files>` empty after the commit; doc lists BodyBrightness 0.5, BodySaturation 0.85, BodyRoughness 0.75, rim (0.25, 0.35, 0.8) x 0.35 power 3 -> removed (Monday: fixed roughness 0.6, no rim), MI_GB_Shooter (0.3, 0.68, 0.035) -> (0.45, 1, 0.05), MI_GB_Heavy (0.12, 0.28, 0.56) -> (0.15, 0.35, 0.7); editor read-back deferred to task-275-04; lint clean; committed by path.
- editor_state: closed
- token_ceiling: 25k
- model: sonnet
- tokens: input 8, cache write 14123, cache read 44481, output 101
- runs: 1
- milestone: M1
- result: Commit d8eae36d: M_Chaser_Meshy_Anim, M_ShieldChaser_Anim, MI_ShieldChaser_Emblem, MI_GB_Shooter, MI_GB_Heavy restored from 75610b21 (Readability group removed; shooter (0.3, 0.68, 0.035) -> (0.45, 1, 0.05), heavy (0.12, 0.28, 0.56) -> (0.15, 0.35, 0.7)); bp-robot-readability rewritten as reverted; editor read-back in task-275-04.

## Relations
- [[task-275-monday-light-revert]] — parent global task.
