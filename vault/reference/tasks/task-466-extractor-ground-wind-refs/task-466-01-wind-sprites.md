---
id: task-466-01-wind-sprites
description: "Bp: Material set for the ground wind shapes of the owner sheet: swirl ring, curved streaks, dust puffs with debris, dust disc."
parent: task-466-extractor-ground-wind-refs
kind: task
status: done
---
# Extractor ground wind redone from the owner references: bp

- goal: Material set for the ground wind shapes of the owner sheet: swirl ring, curved streaks, dust puffs with debris, dust disc.
- agent: bp
- depends_on: none
- inputs: extraction (vault/reference/gdd/run/extraction.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), extractor-blast-references (vault/reference/tech/blueprints/fx/extractor-blast-references.md), bp-extractor-rotor-wash (vault/reference/tech/blueprints/fx/bp-extractor-rotor-wash.md), Art/Source/extractor-fx/reference/owner-ground-wind-sheet-2026-11-06.webp, Art/Source/extractor-fx/reference/owner-ground-wind-mockup-2026-11-06.webp
- expected_output: see done_when
- done_when: The four shape families of the owner sheet exist as translucent whitish-beige materials or textures usable on the floor (real alpha, not the painted checker): flattened swirl ring, long and short curved streaks, puff with debris dots, round dust disc; rings and disc soft-edged, puffs and streaks crisp; style-guide pass on A10 and the two owner references; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: input 12, cache write 37837, cache read 91312, output 6020
- usage: turns 5, context start 9k peak 38k, resumes 1 (message x1), 2 min, units 0.09 M
- runs: 1
- milestone: M2
- started: 2026-11-06T05:27:11Z
- waiting_for: none
- result: Done (f0381dfa). Four sprite families redrawn by Tools/art/make_ground_wind_sprites.js (not cut from the painted checker), real alpha, warm whitish-beige, kept in Art/Source/extractor-fx/ground-wind/: T_GroundWind_Ring (open swirl arcs, tails curling inward, soft painted edges), T_GroundWind_Disc (soft radial), T_GroundWind_Streaks (2x2 atlas of hooked swooshes, crisp), T_GroundWind_Puffs (2x2 atlas of three-tone cartoon puffs with debris dots, crisp). Imported to /Game/RogueRigs/FX/GroundWind (Effects group); M_ExtractorGroundWind (translucent unlit two-sided, Niagara sprites; Shape texture x ParticleColor, atlas cell picked by Particle Random, params AtlasCells, FlipV) and MI_GroundWind_Ring|Disc|Streaks|Puffs. style-guide: round 1 fail ring (closed circle reads as progress ring) and streaks (thick crescents), both corrected; round 2 pass all four (ALERT resume x1: the round 2 review was a SendMessage resume of the same style-guide agent). Preview, temporary, not a game frame: Saved/Scratch/extractor-fx/20261106-ground-wind-sprites-preview-2-not-a-game-frame.png.

## Relations
- [[task-466-extractor-ground-wind-refs]] — parent global task.
