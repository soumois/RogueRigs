---
id: bp-robot-readability
description: "REVERTED (task-275-02): robot readability materials back to Monday 2026-10-19; holds before/after values and the recipe to restore the readability version."
parent: blueprints-enemies-index
kind: doc
---
# Robot readability parameters: REVERTED (owner 2026-10-21, task-275-02: back to Monday lighting)

Task-266-01 and task-270-08 added a Readability group to the chaser masters and lowered two greybox colours. The owner reverted to Monday (commit 75610b21, 2026-10-19). The five `.uasset` files were restored from that commit. Eye glow (GlowAmount) and animation parameters (ClipId, AnimStart, RunRate, ClipOffset, Visible, WPO) were identical on Monday and are unchanged.

Assets: `/Game/RogueRigs/Art/Chaser/M_Chaser_Meshy_Anim` (BP_Chaser), `/Game/RogueRigs/Art/ShieldChaser/M_ShieldChaser_Anim` (BP_ShieldChaser instances), `MI_ShieldChaser_Emblem`, `MI_GB_Shooter`, `MI_GB_Heavy`.

## Before (readability) -> after (Monday)
- Readability group on both masters -> removed. Before: BodyBrightness 0.5 (0.7 before task-270-08), BodySaturation 0.85, BodyRoughness 0.75 with roughness = max(0.6, x), RimColor (0.25, 0.35, 0.8), RimAmount 0.35, RimPower 3, brightness/saturation/rim blended by 1 - red-eye mask. After: fixed roughness 0.6, no rim, base colour unmodified.
- MI_ShieldChaser_Emblem overrides (BodyBrightness 1, BodySaturation 1, RimAmount 0) -> removed.
- MI_GB_Shooter Color (0.3, 0.68, 0.035) -> (0.45, 1, 0.05).
- MI_GB_Heavy Color (0.12, 0.28, 0.56) -> (0.15, 0.35, 0.7).
- Other MIs of the two masters (MI_ShieldChaser_Body, BodyPlain, Shield, Hidden) and MI_GB_Kamikaze, Core, Eye, Chaser were not changed since Monday and not touched.

## Revert recipe
`git checkout 9be640ba -- <the five .uasset files>` restores the readability version, or re-enter the values listed under Before.

## Unverified
Editor read-back (task-275-04): M_Chaser_Meshy_Anim has scalars GlowAmount, ClipId, AnimStart, RunRate; M_ShieldChaser_Anim has GlowAmount, ClipId, AnimStart, RunRate, ClipOffset, Visible and texture BaseColorTex; no BodyBrightness, BodySaturation, BodyRoughness or Rim* parameter on either. MI_GB_Shooter Color (0.45, 1, 0.05), MI_GB_Heavy Color (0.15, 0.35, 0.7). The restore is also file-level: `git diff 75610b21 HEAD` on the five files is empty.

- actor_shadow (task-472, commits 890894ef, 0028c7a2): every robot Blueprint (BP_Chaser, BP_ShieldChaser, BP_Kamikaze, BP_Shooter, BP_Heavy and their V2) keeps the inherited C++ `BlobShadow` (light-independent purple-grey disc, [[sys-blob-shadow]]); per-type `Opacity` 0.6 and `SizeScale` 1.2 (owner 2026-11-06 Q3, provisional) are its Blueprint defaults, no value overridden in this Blueprint.

## Relations
- [[sys-blob-shadow]] — the shadow under every robot.
- [[blueprints-enemies-index]] — parent index.
- [[bp-chaser]] — uses M_Chaser_Meshy_Anim.
- [[bp-shield-chaser]] — uses the M_ShieldChaser_Anim instances.
- [[bp-shooter]] — MI_GB_Shooter colour.
- [[bp-heavy]] — MI_GB_Heavy colour.
- [[chaser-meshy-pipeline]] — the materials and their animation parameters.
- [[color-code]] — chaser = company yellow, red eyes.
