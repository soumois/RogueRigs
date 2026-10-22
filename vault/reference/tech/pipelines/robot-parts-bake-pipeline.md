---
id: robot-parts-bake-pipeline
description: "Per-robot config of the part cut, walk bake and check (heavy first): commands, planted-foot walk, texture rows. Open before another robot."
parent: pipelines-index
kind: doc
---
# Robot parts and walk bake, per-robot config (task-284-01, heavy; provisional, owner 2026-10-22)

Generalises the chaser v1 chain of [[chaser-meshy-pipeline]]. Without `--config` the three scripts keep the chaser v1 values (v1 rerun: outputs identical to the previous script version). Same 10 parts, PartId = (index + 0.5) / 10 in UV channel 3, and texture layout (20 columns, 2 texels per part, frame = row from the top) as v1, so `MF_RobotAnimWPO` ([[bp-robot-eye-material]]) works unchanged.

## Files
- `Tools/art/blender/robot_configs/<robot>.json`: `joints`, `refs` (reference centroids that name the connected pieces), `planes` (point, normal, zone box, side, pivot), `tris_limit`, view frames, and the `bake` object (run rate, speed, knee limit, arm, lean, twist, texture name and rows, report name, reserved clips).
- `meshy_cut_parts.py --config`: all source material slots are kept on every part in order (heavy: body, robot_eye last). A zone box must contain whole large triangles: a triangle with one vertex outside the box is not cut.
- `meshy_bake_anim.py --config`: `walk_style` sine (v1) or planted; `attack` false skips the attack clip; `tex_rows` reserves rows after the baked clips (rest pose). Report adds `walk_poses`, `planted_walk`, `walk_checks`.
- `robot_walk_planted.py`: stance = thigh and shin-foot rigid, rolling over the lower hull of the sole on one ground point at a time (no slip, the pivot corner switches when the next corner reaches the floor), pelvis height solved from the stance foot; heel-strike angle solved so the thigh angle at toe off is minus the one at heel strike; swing = sole angle blended, knee flexion (at most the limit) chosen to clear the floor by 3 cm.
- `meshy_cut_check.py` reads `walk_poses` when present. `robot_walk_preview.py`: 4 frames, side and three-quarter, flat base colour, to `Saved/Renders/task-284/`.

## Heavy (values in the heavy parts report and `heavy_meshy_anim_report.json`)
- Files in `Art/Meshes/heavy-meshy/`: `heavy_meshy_parts.fbx` (+ `.report.json`), `T_HeavyAnim.exr` (20 x 62, same folder logic as `Art/Meshes/chaser-meshy/T_ChaserAnim.exr`), `heavy_meshy_anim_report.json`.
- Clip 0 = walk, rows 0..15, baked for RunRate 1.0 (cycle 0.533 s, travel 40.8 cm at 153 uu/s): the instance RunRate must be 1.0, not the 0.7 default of the chaser. Rows 16..61 hold the rest pose, for task-285-01: the shader row table is fixed (clip 1 rows 16..30, 15 frames, clamped; clip 2 rows 31..46, 16 frames, looped with RunRate; clip 3 rows 47..61, 15 frames, clamped), so telegraph, slam and recover go to clips 1 to 3 or the shader table changes.

## Shooter (task-284-02; provisional, owner 2026-10-22)
- Files in `Art/Meshes/shooter-meshy/`: `shooter_meshy_parts.fbx` (+ `.report.json`, 9341 tris, slots body and robot_eye last, PartId bins exact), `T_ShooterAnim.exr` (20 x 62), `shooter_meshy_anim_report.json`, `cut_check.report.json`; config `robot_configs/shooter.json`. Cut with `--mode plane` (auto mode left the pelvis plate fused to the left thigh).
- Clip 0 = walk, rows 0..15, speed 180 uu/s (EnemyStats.csv), baked for RunRate 1.25 (cycle 0.427 s, travel 76.8 cm, step 38.4 cm): the shooter instance RunRate must be 1.25. Rows 16..61 are the rest pose (aim and fire of task-287 go to clips 1 and 3, same fixed shader table as the heavy). Joints (left, cm): hip (2, 22, 86.5), knee (6, 25, 57), shoulder (2, 36, 125), elbow (1.5, 45, 105), waist z 97; cannon stays on `ForearmHand_R`. Knee at most 15 degrees, foot slide 0.03 cm per cycle.
- Open: `meshy_cut_check` reports 1 anomaly, 189 cap boundary edges left open where the hip cuts meet the waist cut (z 92..99, pelvis band); lowest foot vertex -0.12 cm.

## Relations
- [[pipelines-index]] — parent index.
- [[chaser-meshy-pipeline]] — the v1 chain these scripts keep.
- [[robots-meshy-raw-2026-10-22]] — the raw meshes of the four robots.
- [[enemy-shooter]] — design of the shooter.
- [[enemy-heavy]] — design, speed and clips of the heavy.
- [[bp-robot-eye-material]] — the WPO function that reads this texture.
