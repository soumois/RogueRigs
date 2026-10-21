---
id: rock-render-top-texture
description: "World-position top texture for continuous slab tops (owner addition 2026-10-17): UV, crack mask, tiling, cost, shake sliding, A1 amendment level."
parent: rock-state-render-options
kind: doc
---
# Rock render: world-position top texture

Owner addition (2026-10-17): slab tops sample one large texture by world position, UV = AbsoluteWorldPosition.xy / tile size, so neighbouring pieces stay continuous.

## Crack mask
Same UV space, same texture: R = tone mask, G = crack mask (BC5 or two channels of one BC7, estimate). Cracks therefore cross piece seams like the tone. The damage threshold of [[rock-render-damage]] reads G.

## Tile size and repetition
Arena 240 x 67 = 16 080 uu. Tile about 2 000 uu (30 cells, estimate) repeats 8 x 8; the view shows about one tile (estimate). Two scales break repetition: detail at about 2 000 uu plus a macro sample of the same R channel at about 7 000 uu, rotated, multiplied in. Seed offset per level is free (scalar parameter).

## Cost
One shared material on every piece; 2 to 3 texture samples per top pixel; no per-instance data, no per-instance cost; one 1024 or 2048 texture (about 1 to 4 MB with mips, estimate).

## Shake
Shake moves the render instance (25 uu, 8 Hz) while world UV stays fixed: the texture slides over the shaking piece. Options:
- A. Accept: the slide lasts the dig (0.65 to 2 s; 0.5 s rock before task-260-01, 1.5 s ore before task-259-01) and may read as a shimmer.
- B. Write the shake offset to custom data and subtract it from the UV: one more pair of floats.
- C. Move the shake into the material (WPO by quadrant flag and time) and sample `Absolute World Position (Excluding Material Offsets)`: no slide, no per-frame instance transform (removes the HISM tree dirtying gotcha of [[sys-grid]]); the dual grid needs a quadrant shake anyway. Recommended. Collision already never moves.

## Style constraint (A1)
[[art-style-guide]] A1 allows flat colour per material, a thin lighter band, and faint scratch lines from one small shared grayscale mask; it forbids a per-asset painted base texture. Levels:
1. Grayscale tone mask (subtle value variation, toward a lighter tone, flat-looking) plus crack mask: closest to A1; needs a small amendment (a large world-space shared mask used for tone, not only scratches). Recommended, lightest.
2. Painted colour texture: was a full amendment.
Owner decision 2026-10-17: A1 level 3 (painted colour, normal and roughness maps allowed): the top texture may be a painted colour texture with a normal map; the crack mask stays a separate channel driven by damage.

## Relations
- [[rock-state-render-options]] — parent note.
- [[art-style-guide]] — A1 rule to amend.
- [[rock-render-damage]] — crack thresholds reading this mask.
