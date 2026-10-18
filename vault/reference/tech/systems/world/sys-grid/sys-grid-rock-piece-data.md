---
id: sys-grid-rock-piece-data
description: "Exact rock piece custom data layout (damage, shake, marked, solid mask per quadrant) and the display damage rule. Open for the rock material."
parent: sys-grid-rock-damage
kind: doc
---
# Grid — rock piece custom data layout (task-230-08)

- responsibility: the contract between `ARRArenaGrid` and the rock piece material ([[task-230-09-rock-slab-material]]).
- main_classes: `ARRArenaGrid::PieceDamageCustomDataIndex`, `PieceShakeCustomDataIndex`, `PieceMarkedCustomDataIndex`, `PieceSolidMaskCustomDataIndex`, `PieceCustomDataCount`, `GetCellDisplayDamage` (`Grid/RRArenaGrid.h`).
- custom data layout (every piece HISM, intact and broken, 13 floats; constants `Piece*CustomDataIndex`, `PieceCustomDataCount`): quadrant q of the piece at corner (x, y) is cell (x − 1 + (q & 1), y − 1 + (q >> 1)), i.e. q = (+X side) + 2 · (+Y side) in grid actor axes, not the piece's rotated local axes (material: sign of world position − instance position, grid actor unrotated).
  - 0–3: display damage d of quadrant q, 0..1.
  - 4–7: shake of quadrant q: 0 still, else 1 + phase / 2π in [1, 2) (same per-cell phase as the old jitter; amplitudes and frequency are `ShakeAmplitude` 25, `ShakeVerticalAmplitude` 8, `ShakeFrequency` 8 Hz, to mirror as material parameters; offset = (H sin(a + p), H sin(1.37a + 2p), V sin(1.71a + 3p)), a = 2π f t).
  - 8–11: marked outline of quadrant q: `MarkedCustomDataValue` (1) or 0.
  - 12: solid mask of the 4 quadrants (bit q = rock piece cell), 0..15.
- damage: d = 1 − health / max health on rock, ore, heal and special ore cells (above 0 at the first hit). Empty quadrant (floor, barrel, void, outside): max d of its solid 4-neighbours, 0 without one — provisional choice (owner left it open), same rule as the task-230-06 previews. The sag inputs of `SM_RockSlabBroken.report.json` beyond the 4 quadrants (distance to any empty cell, nearest intact cell) are not provided: the material approximates them from these floats (open for [[task-230-09-rock-slab-material]]).
- gotchas: the grid actor is assumed unrotated (as `GetClearDistance`); read the floats with PerInstanceCustomData on every piece material, intact and broken.

## Relations
- [[sys-grid-rock-damage]] — parent: swap, writes and crystals.
- [[rock-render-damage]] — the design these floats feed.
