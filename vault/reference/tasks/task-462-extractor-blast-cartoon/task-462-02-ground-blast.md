---
id: task-462-02-ground-blast
description: "Bp: The ground blast becomes the cartoon wind of the chosen reference, height fade kept."
parent: task-462-extractor-blast-cartoon
kind: task
status: done
---
# Extractor ground blast redone in cartoon style from references: bp

- goal: The ground blast becomes the cartoon wind of the chosen reference, height fade kept.
- agent: bp
- depends_on: task-462-01-references
- inputs: bp-extractor-rotor-wash (vault/reference/tech/blueprints/fx/bp-extractor-rotor-wash.md), bp-extraction-shuttle (vault/reference/tech/blueprints/world/bp-extraction-shuttle.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), Art/Source/extractor-fx/reference/owner-ground-blast-2026-11-05.webp
- expected_output: see done_when
- done_when: Compiled and saved with no error; in PIE the particles are the new shapes on the floor, fade to none at 400 uu read in the simulation; Extraction tests pass.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: input 10, cache write 20635, cache read 56187, output 4718
- usage: turns 4, context start 9k peak 24k, resumes 1 (message x1), 2 min, units 0.06 M
- runs: 1
- milestone: M2
- started: 2026-11-06T04:43:47Z
- waiting_for: none
- result: Done, look unverified until the owner sees it (3a4c9740, 25a842ef). GroundBlast now uses new M_ExtractorBlastWind (no texture, Custom node): per particle MaterialRandom picks a hard-edged cartoon puff with a darker spiral curl band (about 55 percent) or a tapered curved wind stroke along the outward velocity (about 45 percent); shapes break up with age (puffs open into a ring, strokes shrink to a pointed sliver), no blur. Sprite size puff BlastSize x (0.75 + 0.7 age), stroke BlastSize x (0.55, 2.6) x (0.8 + 0.6 age); alpha 0.6 x Intensity x saturate(1 - H/400), no age fade; rate 32/s x Intensity x strength. Color expression compiled stale at first (quirk), re-set and checked. PIE arrival takeoff (slomo 0.1), fx.Niagara.DumpComponents: H 68.8 to 400 on the 4 components, alpha 0.497, 0.382, 0.232, 0.113, 0.036, 0.002, 0 at 400, error vs formula 0.00000, size error under 0.002, every particle at Z 8. Extraction tests 7/7 in the editor automation toolset. style-guide: round 1 A10 fail (straight cuts in the stroke break-up), corrected, round 2 pass 6/0. Swirls, wiring, timings untouched.

## Relations
- [[task-462-extractor-blast-cartoon]] — parent global task.
