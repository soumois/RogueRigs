---
id: validation-checklist-2026-10-12
description: "What the owner must look at in game to validate the work of 2026-10-07 to 10-12: per area, expected result, known risk."
parent: production-index
kind: note
---
# Validation checklist (2026-10-12)

Everything below is built, compiled, read back and, since 2026-10-13, validated by the owner (see Owner status). Task ids point to the nodes with the details.

| Area | Look for | Known risk |
|---|---|---|
| Weapon row (106, 109) | four framed slots 150 x 90, row centred at the bottom; an owned slot shows a big icon, no name; a locked slot shows the padlock; turret line above the row | icon sizes are arithmetic, not measured; the turret SizeBox may have the override-flag defect; icons are TEMPORARY ([[placeholder-weapon-icons]]) |
| Subtitles (099) | company line at the bottom centre above the weapon row, one at a time, speaker prefix | `extraction_ready` replaced at once by `shuttle_countdown` |
| Cards (080, 084, 094) | Rook's flavor line under each stat card, not clipped; clicks and reroll work | flavor wraps on 3 lines |
| Alerts (085, 094) | shuttle line once for 8 s; alert bar now below the cards (z 8 against 10); end screen | end screen sits inside the alert panel |
| Vignette (094-03, 104) | first hit flashes, holds 0.15 s, fades in about 0.4 s | fix of the zero time step unverified in game |
| Minimap, arrow (094-08, 103) | minimap follows the miner, arrow shows off screen and pulses, nothing runs when idle | texture event, pulse timer |
| XP bar (094-04) | bar, level text, LEVEL UP flash 1 s | |
| Pools (090 to 092, 095, 111) | reused XP cubes same size as new ones; robots appear within 20 frames; first mass kill about 60 ms hitch | pop-in at the screen edge |
| Big XP block (110, 122) | 8 cubes become one block, 1.8 times bigger, magenta-pink with a glow of the same hue (white until 2026-10-13), same total XP | glow, trail and sparkle copies (NS_XPTrailBig, NS_PickupSparkleBig) |
| Kamikaze (096) | killed by a shot: flares 0.65 s then explodes, XP only at the explosion; killed by an explosion: instant | auto-aim may waste shots on it |
| Explosions (097) | visible 1.05 times the radius, held 0.1 s, 0.6 s in all | |
| Barrel (114, 121) | radius 700, 1.5 s fuse after 0 health, orange danger zone and a growing filled disc; a chain adds 1.5 s per barrel; blocks break nearest first over several frames (2 ms budget per frame) | miner takes 40 twice in a chain; hitch not re-measured |
| Capture zones (113, 118, 120) | console zone 7 cells, every rock in it (ore included) marked and to destroy, hack 12 s; dotted ring; growing circle is a transparent filled disc, zone interior transparent; chest unchanged | `FillOpacity` 0.25 is a guess; shader recompiled but never seen; a block whose centre is just outside the zone stays unmarked |
| Heavy slam (115, 118, 119, 123) | ground effect validated by the owner on 2026-10-13: orange ring at 289 uu and a growing filled disc during the 0.6 s telegraph, gone if it dies, nothing on a heavy running from far away; the old ShockRing mesh is no longer shown | ShockRing component deleted (task-124) |
| Heal (088, 089) | veins 2 x 2, 15 HP each; a heal pickup is not taken at full health and is taken later | |
| Duskite (070, 100) | violet 2 x 2 groups at 8 % of the rock (359 deposits), mined whole in 1.5 s, 4 chunks, no start-zone protection | positions changed with the rename |
| Robots (071, 093) | slide around rock, shotgun knockback, heavy slower, shots still hit | knockback compared only on the new path |

## Owner status (2026-10-13)
- Validated by the owner: weapon row (106, 109), barrel (114), ground material (112, parameters left to tune), heavy ground effect (115, 119, 123, 124); on the owner's word "the rest works" after tasks 118 to 122: capture zones, big XP block, barrel spread.
- Validated by the owner on 2026-10-13 (everything else): all the HUD (subtitles, cards, alerts, vignette, minimap and arrow, XP bar), the pools and prewarm, and all the pending gameplay (kamikaze, explosions, heal, Duskite, robots). Nothing is left to validate.

## Relations
- [[production-index]] — parent index.
- [[handover-2026-10-12-gameplay-hud]] — the state this list belongs to.
- [[ground-telegraph]] — the ground circles design.
- [[hud-refactor-audit]] — the HUD structure.
