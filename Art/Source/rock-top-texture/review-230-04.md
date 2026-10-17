# Review 230-04: rock top texture (owner pick, proposal 2)

Reviewed: `Art/Textures/T_RockTop.png` (= `20261017-153218-rock-top-texture-2-tileable.png`) and `20261017-153218-rock-top-texture-2-tiled3x3.png`.
Judged on the images alone, for the use given: world-aligned top face of breakable rock, about 2000 uu per tile plus a macro sample, game camera about 60 degrees down.

What the images show: a soft mottled stone surface, low contrast, fine grain, no cracks, no lines, no text; overall colour a clear lavender / periwinkle (blue-violet), mid-light value.

| Rule | Verdict | Reason | Correction |
|---|---|---|---|
| color-code (breakable rock: light grey tinted violet) | fail | The texture reads as a saturated lavender, i.e. violet with a little grey, not grey with a violet tint; it also moves the rock toward the Duskite violet of the ore crystals, which weakens the ore read. | In the material, desaturate the base colour by about 50-60% (lerp to its luminance, or Desaturation node at 0.5-0.6) and nudge the value slightly up, so it reads light grey with a faint violet cast; keep the tint as a material parameter so the owner can tune it. |
| A1 (level 3, painted textures allowed, facets and silhouette not hidden) | pass | Painted tiling texture with low-contrast mottling; nothing in it would hide slab facets or silhouette. | none |
| readability: cracks mean damaged | pass | No cracks, veins or line features anywhere in the tile or the 3 x 3; intact rock reads smooth. | none |
| readability: rock lighter than the floor | not judgeable | The floor is not in the images; the texture value is mid-light (roughly 60-65% brightness). | Check in-game next to the floor; if needed raise brightness in the material, not in the texture. |
| readability: ore glow is the brightest thing | pass (conditional) | The texture has no highlights or emissive-looking spots; its peak value is well below a glow. | Keep the base colour multiplier at or below 1 after the desaturation fix. |
| readability: terrain never uses enemy colours | pass | Hue is blue-violet, not acid green nor steel blue; the desaturation fix above moves it further from steel blue. | none |
| A13 / A14 (no pebbles or rubble, no text) | pass | No small rubble shapes, no text. | none |

## Seams and repetition (3 x 3)
- Seams: none visible at the tile borders.
- Repetition: when looked for, a faint periodic pattern of lighter blotches (a lighter band near the top of each tile, a darker zone left of each tile centre) is visible in the 3 x 3. No rule covers it; the planned macro sample (larger scale, multiplied at low strength) should break it. Not a fail, flagged for the in-game check.
- The fine grain is about one texel per cell at 30 cells per tile and will vanish at game distance; harmless.

## Totals
6 pass (one conditional), 1 fail (color-code), 1 not judgeable (rock vs floor brightness).
