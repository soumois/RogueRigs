---
id: color-code
description: "Color of every element: rock, ore, heal ore, barrel, pickups, player side, shots, each robot. Open before any asset, VFX or prompt."
parent: art-audio-index
kind: spec
status: approved
---
# Color code

| Element | Color |
|---|---|
| Breakable rock | light grey tinted violet, visible cracks |
| Ore rock (Duskite) | same base + glowing violet crystals (kept, owner 2026-10-10) |
| Barrel | orange |
| Heal ore and heal pickups | red |
| XP and resource pickups | glowing cyan; the big XP block (many cubes merged) is bright white with the cyan glow (owner 2026-10-12); implemented as MI_PickupXPBig: base white (1, 1, 1), emissive (0.75, 1, 1) x 4 |
| Miner, turrets, drone | white + teal + rust |
| Player shots, mining laser | white-gold |
| Command console, shuttle | white + teal (player side) |
| Console marked rocks | pulsing white outline |
| Enemy shots, targeting laser | red (moving lines) |
| All robots | red eyes + yellow logo plate |
| Chaser | company yellow |
| Kamikaze | dark gunmetal + pulsing orange core |
| Chaser shield | translucent pale blue-white (provisional) |
| Shooter | acid green |
| Heavy | steel blue + yellow/black hazard stripes |
| Normal / critical damage number | white / larger white-gold |

Exception (owner 2026-10-11): the explosion warning sign on the barrel is a yellow triangle with a black border and a black burst, chosen by the owner after a reference sign; the yellow and black hazard stripes stay reserved for the heavy ([[barrel-asset]]).

- Colour principle (owner 2026-10-13): an element is coded by its main colour, the colour that dominates it, not by a detail. A small detail may reuse a colour that belongs to another element (the barrel is orange with a yellow sign, and yellow stays the chaser colour). The same colour may appear as a detail on any element.
- Big XP block (owner 2026-10-13, replaces the white block of 2026-10-12): bright magenta-pink with a glow of the same hue, scale 1.8 unchanged; magenta is not the main colour of any other element.

## Relations
- [[art-audio-index]] — parent index.
- [[readability-rules]] — why these colors.
- [[barrel-asset]] — the barrel and its yellow and black warning sign.
