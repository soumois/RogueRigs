---
id: rock-state-render-options
description: "Design note (no code): rendering the owner-chosen slab rock look on the grid HISMs; options, costs, risks, recommendation. Open before rock look work."
parent: sys-grid
kind: doc
---
# Rock state render options (task-219-03, design only)

Target: owner image C (`Art/Source/rock-look/20261017-131704-rock-look-2.png`): continuous slab tops, rounded organic outline, lighter rim, faceted low-poly sides; intact rock smooth, cracks then tilted plates mean damage ([[color-code]], [[readability-rules]]); ore deposits 2 x 2 as four crystal clusters. Near-vertical camera: top and outline dominate, sides are a thin strip ([[visual-style]]). Nothing here is implemented.

## Recommendation
1. Dual-grid (marching squares) slab pieces at cell corners replace the approved per-cell 6 shapes by open sides ([[rock-render-piece-set]]); this is a change to an approved [[sys-grid]] rule, owner to confirm.
2. Floor slab under every non-void cell, rock pieces only above the floor top, so outline warp and rounding never reveal the void.
3. One shared top material: grayscale tone mask and crack mask in one world-UV texture, damage per quadrant in custom data ([[rock-render-top-texture]], [[rock-render-damage]]).
4. Broken variants of the 5 pieces, swapped in around damaged cells and sagged by the material toward the empty side, minimum height 60 uu (owner 2026-10-18, [[rock-render-damage]]); shake moved into the material (WPO) for slabs.
5. Ore crystals as one instanced cluster per ore cell in their own chunk HISM.

## Owner decisions (2026-10-17)
- Nanite: assumed for the grid pieces (owner); contra-indications to check at integration: WPO cost (outline warp, shake), dynamic instance add/remove, very low-poly pieces.
- Variants: one per piece shape at first; the system supports several (variant index per instance, chosen by cell hash). Since task-480: two per shape with a side, one for full.
- A1: relaxed to level 3 (painted colour, normal and roughness maps allowed; [[art-style-guide]]).
- Cracks: shown as soon as a block is below 100 % health (owner).
- Dual grid: approved for the display (owner); triangle budget of [[rock-render-counts]]: provisional, to check by a game measure at integration; rim: vertex colour in the one material (owner).
- Weak rock: maximum share of the rock (value open), separate task.

## Risks
- Render and collision indices stop being in lockstep: render pieces are per corner, collision stays per cell. The index maps and the `RogueRigs.Dig.*` instance-map tests must be rewritten (estimate 2-3 days for the whole change, not measured).
- Up to 4 chunks dirtied per broken cell at chunk borders; proxy recreation per touched chunk per frame ([[rock-render-rebuild]]).
- Visual outline differs from collision by the warp amplitude (up to about 15 uu, estimate); bullets or units may seem to touch rock that is not drawn there.
- More HISMs per chunk (one per piece mesh and variant): draw calls rise; Nanite use is open.
- A large world-UV top texture: allowed since the A1 amendment ([[rock-render-top-texture]]).

## Children
- [[rock-render-piece-set]] — dual grid versus per-cell shapes, outline warp, rim, sides, floor.
- [[rock-render-counts]] — instance and triangle counts at 240 x 240 and the proposed budget.
- [[rock-render-rebuild]] — what changes per broken cell, HISM updates, dirtying.
- [[rock-render-damage]] — cracks, tilted plates, thresholds, ore crystals, future weak rock.
- [[rock-render-top-texture]] — world-position top texture, tiling, shake, A1 style constraint.

## Relations
- [[sys-grid]] — parent: the grid system this note would change.
- [[task-219-03-rock-render-note]] — the task that produced this note.
- [[rock-look-reference]] — owner reference captures behind the look.
- [[art-style-guide]] — A1 rule touched by the top texture.
- [[perf-stress-grid-cells]] — measured grid costs the estimates build on.
