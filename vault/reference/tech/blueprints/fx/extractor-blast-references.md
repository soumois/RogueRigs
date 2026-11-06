---
id: extractor-blast-references
description: "Five web references of cartoon ground wind and rotor downwash for the extractor ground blast, what each gives, the one followed; open before changing it."
parent: blueprints-fx-index
kind: note
---
# Extractor ground blast: references

Found 2026-11-05 for [[task-462-extractor-blast-cartoon]] (Q38). Links only, no image copied into the repo. Style direction: the cartoon explosions of [[art-style-guide]] A10 (flat bands, hard edges, no blur). Problem to solve: the blurry white ovals of `Art/Source/extractor-fx/reference/owner-ground-blast-2026-11-05.webp`.

## References
1. Wind Waker puffs, https://realtimevfx.com/t/3213 (Torbach, 2017, "Zelda Wind Waker blending style"): flat cloud puffs with a spiral curl and a darker rim; overlapping puffs merge into one silhouette instead of stacking soft alpha (alpha-test edge). Take: hard edge, one opacity level, the inner curl band, union of puffs.
2. Wind Waker wakes, https://realtimevfx.com/t/30572 (2026, toon surf trails) and the Thomas Corvée study it cites, https://www.artstation.com/artwork/ZeWdJG: flat white curved trails with sharp edges and tapered ends. Take: curved strokes thick in the middle, pointed at both ends.
3. Wind Shear 2D, https://realtimevfx.com/t/17282 (Gonzila, 2021; https://www.artstation.com/artwork/kDJmNx): a flat gust shape sweeping along the ground that breaks into small crescent hooks. Take: crescent hooks; shapes break and shrink at the end of life instead of fading out.
4. VFX Practice Wind, https://realtimevfx.com/t/26056 (fxy, 2024; video https://www.youtube.com/watch?v=I2FtdoBd7js): wind wisps with an expanding ring on the ground. Take: the ring that spreads outward; reject its soft wispy alpha, the same blur as today's blast.
5. Helicopter brownout, https://youtu.be/tgoqaktpbFQ?t=127 (3DLZ flight test, cited in https://realtimevfx.com/t/21199): real downwash. Take the motion only: dust leaves the area under the rotor radially along the floor, fastest near it, curling up at the edge, stronger as the rotor gets lower.

## Followed
- chosen: 1 and 2 together, the Wind Waker look: flat, hard-edged pale puffs with a curl band, plus tapered curved wind strokes, both thrown outward along the floor with the motion of 5. Why: it is the cel look closest to the owner's cartoon explosions (flat bands and sharp edges, no gradient), it reads from a top-down camera, and it is what the blast lacks; 3 adds the break-up at end of life. Colour stays the pale grey-beige of [[extraction]].
- not followed: 4 (soft alpha) and the realistic brownout dust of 5.

## Relations
- [[blueprints-fx-index]] — parent index.
- [[bp-extractor-rotor-wash]] — the effect made after these references.
- [[task-462-extractor-blast-cartoon]] — the task that found them.
- [[art-style-guide]] — A10, the cartoon style direction.
- [[extraction]] — the design of the ground blast.
