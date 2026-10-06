---
id: enemies-index
description: "The corporate security robots, one node each, plus how robots die. Open for any enemy work: behavior, rig, look, stats."
parent: gdd-index
kind: note
---
# Enemies

All robots share red eyes and a yellow corporate logo plate. Every robot except the kamikaze is a biped. Robots spawn just outside the miner's field of view; there are no fixed spawn points. M1 spawn rule (provisional, owner 2026-10-03): at most 100 alive; mix chaser 65 / kamikaze 12 / shooter 15 / heavy 8, at most 5 heavies alive (owner 2026-10-04); one robot every 2 s for the first 30 s, then packs of 8 to 15 robots about every 6 s, each pack from one direction, with a rare lone robot between packs; always on a floor cell reachable from the miner. Wave director (owner 2026-10-06 after the first play test: start sooner, ramp slower, shooters blended, robots 20 % slower; values provisional), from the DRGS analysis ([[drgs-analysis]]): no calm phase, a first robot at about 1 s, then a lone chaser every 2 s until a first pack of 6–8 chasers from one side at 15 s; then waves of 1–2 packs until 1:30, 2–3 packs after, each from a distinct side, every 6 s shrinking to 3.5 s over 3 min 30. Packs never hold only shooters: every flow pack carries 1–2 shooters in its back half, never in front, about 9–10 % of all robots (task-030-01, provisional). Pack types: chasers 8–14 with 2–3 kamikazes; from 1:00 the same with 2–4 shield chasers; heavies 2–3 leading 8–12 chasers (weights 92 / 8: heavies come about half as often, owner 2026-10-06, provisional). The first pack, lone robots and hordes carry no shooters. At most 6 shooters alive: shooters fire from afar and pile up while the miner shoots backing away, so a pack drawn at the cap gets chasers in place of its shooters (owner 2026-10-06, cap provisional). Robots arrive just outside the view, 100 to 500 uu beyond its edge, so the first contact comes within about 2 s; they come from several sides so the miner gets surrounded (owner 2026-10-06: keep). Running away must not escape them (owner 2026-10-06, split provisional): while the miner runs, 55 % of spawns fall in front of its heading (within 45° of it, NW to NE when running north), 35 % on the sides (45° to 135°), 10 % behind; heading = movement over the last second; standing still spreads them all around; packs and the horde side follow the same split. Two hordes triggered by run time, fronts at about 1:35 and 2:55: a 5 s lull, a "HORDE EN APPROCHE !" banner 4 s before a front of 40–60 robots spawned along one side over 2 s, heavies 3–5 first, then kamikazes 3–5 and a carpet of chasers. At most 150 alive. Shield chasers (chaser with a 30-point shield) join chaser packs after 1:00. Robots drop XP only. Their health and attack power grow with the run level and the threat counter ([[difficulty]]). Reference: `vault/assets/enemies-lineup.jpg`.

## Relations
- [[gdd-index]] — parent index.
- [[enemy-chaser]] — small yellow biped, melee rusher.
- [[enemy-kamikaze]] — hovering bomber, explodes on proximity or death.
- [[enemy-shooter]] — acid-green biped, ranged, needs line of sight.
- [[enemy-heavy]] — steel-blue biped, twice the size, tough, resists knockback.
- [[enemy-death]] — how any robot breaks apart on death.
