---
id: art-style-guide
description: "Numbered style rules A1–A18 for any new asset: shape language, faction contrast, effects, composition, forbidden elements, delivery formats. Open before making, briefing or reviewing art."
parent: art-audio-index
kind: note
---
# Art style guide

Derived from the four approved reference images and their prompts ([[art-prompts]]). Colors live in [[color-code]]; brightness and color-meaning rules live in [[readability-rules]]; do not restate them here. Reviews cite the rule ids below.

## Shape language
- A1. Chunky, blocky volumes with large flat facets; facets stay visible, no smoothing. Textures (owner 2026-10-17, level 3, replaces the 2026-10-11 level 1 rule, which was meant as a limit of AI generation, not a style choice): per-asset painted base colour, normal and roughness maps are allowed, as are shared tiling or world-aligned textures (rock tops) and Meshy textures; flat colour per material with thin lighter edge bands and the shared scratch mask (the barrel) stays a valid option; stickers and signs are textured decals. Textures keep the main colours of [[color-code]] and must not hide the facets or the silhouette.
- A2. Rigid mechanical parts only: plates, pistons, bolts, big gripping hands. No cloth, no organic forms.
- A3. Every enemy role reads by silhouette alone: chaser = lanky biped, arms forward; kamikaze = round body with a glowing core and four limbs; shooter = bulky, one arm is a built-in cannon, thin antenna; heavy = chaser silhouette at twice the size, hunched.
- A4. The drone is about the size of the miner's head and shoulders; the turret sits low and wide on a tripod base.

## Two factions, one contrast
- A5. Corporate robots: clean panels, one big red eye in a boxy head, a small yellow logo plate, hazard stripes on heavy armor, no wear.
- A6. The Rigs: patched, mismatched white and teal plates with rust chips, bolted-on pouches and a backpack, rivets and welded repairs; amber visor on the miner.
- A7. Anything on the player side never uses enemy colors; anything corporate is never rusty.

## Effects
- A8. Player fire: white-gold bolts, bright muzzle flash, small impact sparks on the hit target.
- A9. Enemy fire and targeting: thin red lines.
- A10. Explosions: orange core with debris; the kamikaze core must read as "about to explode".
- A11. Nobody fires over a block: shooters and targets are positioned with a clear line.

## Scene composition
- A12. Steep near-vertical camera, miner centered, strong directional light with lifted shadows.
  - Provisional: Shadows off for now (owner 2026-10-20, temporary, task-261): the shadow part of A12 is suspended, see [[lights-no-shadows]].
- A13. Clean flat floor with subtle large facets and no pebbles; the arena edge is a cliff into space with stars and a planet.

## Forbidden
- A14. UI inside reference art, small scattered rubble on the floor, grey rock walls as arena borders. Text is allowed only on logos, stickers and advertising panels, never as UI or captions in a reference image (owner 2026-10-11).
- A15. Handheld weapons on the corporate shooter (its cannon is part of the arm).

## Delivery formats
- A16. Character or robot sheet: plain light grey background, three-quarter top view.
- A17. 3D source: front, side and back T-pose on plain white, no weapon, no effects, even lighting, no shadows.
- A18. Of a 3D source sheet, front and back are used ([[asset-pipeline]]).

## Relations
- [[art-audio-index]] — parent index.
- [[art-prompts]] — the prompts and method this guide was derived from; use them to produce matching art.
- [[visual-style]] — the short look statement this guide expands.
- [[color-code]] — palette per element.
- [[readability-rules]] — rules the guide must never break.
- [[asset-pipeline]] — what happens to the images after approval.
- [[agent-style-guide]] — the critic that cites these rule ids.
- [[lights-no-shadows]] — provisional suspension of the shadow part of A12 (lights without shadows, revert steps).
