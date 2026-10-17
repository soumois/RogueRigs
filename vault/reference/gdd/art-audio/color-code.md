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
| Ore pickup (Duskite chunk) | glowing Duskite violet crystal cluster, like the HUD ore icon, with a violet trail (owner 2026-10-15, provisional until modelled, task-189); the owner judges it distinct enough from the lilac core of the big XP chip |
| XP pickups | glowing cyan, shaped as a hex nut (owner 2026-10-13, provisional until modelled; resource pickups other than ore keep this look); the big XP block (many pieces merged) is a data chip whose glowing core is bright magenta-pink with a glow of the same hue (owner 2026-10-13, was white), on a dark gunmetal board; implemented as MI_PickupXPBig: base (1, 0.15, 0.75), emissive (1, 0.2, 0.8) x 4, with magenta trail NS_XPTrailBig and sparkle NS_PickupSparkleBig; the big XP chip core now glows lilac-violet, GlowColor about (0.8, 0.25, 1.0), provisional, owner 2026-10-13, while NS_PickupSparkleBig, NS_XPTrailBig and MI_PickupXPBig stay magenta (a mismatch for the owner to decide) |
| Miner, turrets, drone | white + teal + rust |
| Player shots, mining laser | white-gold |
| Command console, shuttle | white + teal (player side) |
| Console marked rocks | pulsing white outline |
| Enemy shots, targeting laser | red (moving lines) |
| All robots | red eyes + yellow logo plate |
| Chaser | company yellow |
| Kamikaze | dark gunmetal + pulsing orange core |
| Chaser shield | physical riot shield: company yellow front, yellow and black diagonal stripes on the cut corners and over the top edge (front, top face and back), yellow back with a dark gunmetal grip, charcoal Kessler-Voss emblem on the front (owner 2026-10-17, provisional; was a translucent pale blue-white shell, still the one in game, [[art-prompts-shield]]) |
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
