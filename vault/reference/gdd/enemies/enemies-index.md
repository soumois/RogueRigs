---
id: enemies-index
description: "The corporate security robots, one node each, plus how robots die. Open for any enemy work: behavior, rig, look, stats."
parent: gdd-index
kind: note
---
# Enemies

All robots share red eyes and a yellow corporate logo plate. Every robot except the kamikaze is a biped. Robots spawn just outside the miner's field of view; there are no fixed spawn points. M1 spawn rule (provisional, owner 2026-10-03): at most 100 alive; mix chaser 65 / kamikaze 12 / shooter 15 / heavy 8, at most 5 heavies alive (owner 2026-10-04); one robot every 2 s for the first 30 s, then packs of 8 to 15 robots about every 6 s, each pack from one direction, with a rare lone robot between packs; always on a floor cell reachable from the miner. Wave director (provisional, owner 2026-10-05), from the DRGS analysis ([[drgs-analysis]]): 10 s calm, isolated robots until a first pack at 22 s, then packs of one type from 2–3 sides with a shrinking interval, tough robots in front; two hordes (about 1:35 and 2:55) announced by a 5 s lull and a "HORDE EN APPROCHE !" banner 4 s before a front of 40–60 robots from one side; at most 150 alive. Shield chasers (chaser with a 30-point shield) join chaser packs after 1:00. Robots drop XP only. Their health and attack power grow with the run level and the threat counter ([[difficulty]]). Reference: `vault/assets/enemies-lineup.jpg`.

## Relations
- [[gdd-index]] — parent index.
- [[enemy-chaser]] — small yellow biped, melee rusher.
- [[enemy-kamikaze]] — hovering bomber, explodes on proximity or death.
- [[enemy-shooter]] — acid-green biped, ranged, needs line of sight.
- [[enemy-heavy]] — steel-blue biped, twice the size, tough, resists knockback.
- [[enemy-death]] — how any robot breaks apart on death.
