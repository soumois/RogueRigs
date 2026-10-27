---
id: visual-style
description: "Flat-shaded low-poly look, near-vertical camera centered on the miner, low blocks, void arena edge, salvage vs corporate contrast."
parent: art-audio-index
kind: spec
status: approved
---
# Visual style

- Flat-shaded low-poly.
- Camera: steep, near-vertical perspective; the miner is always centered.
- Blocks are always low (no occlusion); the top face is the most visible face.
- Floor of cracked faceted plates, painted look, edges catching the light, no rubble (owner 2026-10-21, provisional, task-273; was "clean floor", [[art-style-guide]] A13); arena edge is a cliff into space with stars and a planet.
- Void background (owner 2026-10-27, provisional, [[task-332-space-background]]): procedural, layered, never a pasted image. Three Unlit planes under the arena at real depths, parallax from the perspective camera: stars Z -12000 (opaque, black base), nebula Z -5000 (additive), sparse dust Z -2000 (additive). Motion in the material only: slow star twinkle, very slow nebula flow; no Niagara dust or drifting debris. Always darker than the floor beside it (nebula peak at most the far floor value; stars are the only bright points); the darkening mask applies to it like the rest of the screen. No planet yet (later request); no glow at the floor-void edge. Colours in [[color-code]].
- The Rigs' side looks repaired and bolted together; corporate robots look clean.
- No textures, no UI in reference art; strong directional light with lifted shadows; floor with no pebbles or rubble so robots, pickups and blocks stand out.
- Key art: `vault/assets/keyart.jpg` (readability reference, not a fidelity promise).
- M1 is grey box: plain cubes colored by block type after [[color-code]]; final assets come later ([[sys-grid]]).

## Relations
- [[art-audio-index]] — parent index.
- [[color-code]] — colors.
- [[asset-pipeline]] — how assets are made.
- [[art-style-guide]] — detailed rules for shapes, factions and effects.
- [[art-prompts]] — exact prompts and locked phrases that produce this style.
- [[lights-no-shadows]] — history: shadow suspension of the level lights, lifted by task-275-01.
