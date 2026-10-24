---
id: art-prompts-chest
description: "Bonus chest supply locker: owner-validated turnaround sheet (front, side, back) and its verbatim prompt. Open for chest art or its 3D mesh."
parent: art-prompts
kind: note
---
# Bonus chest — supply locker turnaround

Owner validated 2026-10-23. Look of the chest of [[objective-bonus-chest]]: a corporate (Kessler-Voss) supply locker, so the corporate side of [[color-code]] applies.

## Files
- Validated sheet (front, side, back), generated in ChatGPT by the owner: `vault/assets/chest-turnaround.webp`.
- Prompt as a text file: `Art/Source/chest/prompts/chest-turnaround.txt`.
- Meshy mesh by the owner, textured, optimised in Meshy (2026-10-23): `Art/Source/chest/meshy/` (copied from the owner's Downloads). Defect reported by the owner: the faces are puffed, not flat.
- Game-feel reference video given by the owner 2026-10-23 (third-party "How to make chests juicy", 29 s, seven techniques: wobble and squash, stretch, lid slam with freeze and camera shake, inner light with turning rays, ground ring shockwave, popping stars, floating sparks): `Art/Source/chest/reference/egor-chest-juice.mp4`.
- Lock latch crop (front lock block with the red light, the part that pivots at opening): `vault/assets/chest-lock-latch.png`.

## Sheet prompt (verbatim)
The `[OBJECT]` placeholder was left as is by the owner; the `Design:` paragraph defines the object.
```
Turnaround sheet for 3D modeling of [OBJECT]: front view, side view (exact 90 degrees) and back view, all three at the same size, same angle and same framing, subject centred with a margin, no ground.
Style: stylized flat-shaded low-poly game asset, chunky blocky volumes with large flat facets, facets clearly visible, no smoothing, no textures, clean hard-surface look. Rigid parts only: plates, pistons, bolts, no cloth, no organic forms. The silhouette must read clearly from a steep top-down camera.
Background and light: plain white background, even neutral lighting, no shadows, no effects, no text, no UI.

Design: a clean industrial corporate supply locker, wide and low, dark gunmetal armoured panels with sharp bevelled edges, a heavy lid with two hinge blocks, a reinforced lock block on the front with a small red lock light, and a small yellow logo plate on the lid. Spotless, no wear, no rust, no hazard stripes.
```

## What the sheet shows
- Wide, low gunmetal box, bevelled corners, chamfered feet with slots.
- Lid: two hinge blocks at the back, yellow logo plate (simplified, not the [[kessler-voss-logo]] artwork).
- Front: lock block with a small red light. Side handles on both ends.
- The side view is not a clean 90-degree view (lid and hinges read at an angle).

## Next steps (open)
- 3D mesh route: Blender cleanup of the Meshy mesh (task-306). Size about 130 x 70 x 66 uu, the Meshy proportions (owner 2026-10-24); pivots (task-306-02, relative to the body bottom-centre origin): lid on the hinge axis (0, 31.14, 41.25) cm, latch on its top edge (-0.48, -24.15, 51.49) cm; report `Art/Source/chest/clean/chest_cut.report.json`. The lid origin sits on the body back top edge, not on the hinge pins: task-309-07 moves it to the pins and fixes the open pose (owner 2026-10-24).
- Triangle budget: 3864 tris for body, lid and latch, accepted by the owner 2026-10-24 (default Q2 of task-306 was 3000); task-314 adds at most 150 on the lid.
- Lid logo missing on the flattened mesh: separate task (owner 2026-10-24).
- Logo plate: keep the simplified mark or apply the real logo as a decal: open.

## Relations
- [[art-prompts]] — parent: method and locked phrases.
- [[objective-bonus-chest]] — the objective this chest belongs to.
- [[color-code]] — corporate colours.
- [[kessler-voss-logo]] — the company logo the yellow plate stands for.
