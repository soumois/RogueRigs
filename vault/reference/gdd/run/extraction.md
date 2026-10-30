---
id: extraction
description: "End of a level: shuttle lands far from the miner, countdown to reinforcements, risk window choice, consequences of missing the shuttle."
parent: run-index
kind: spec
status: approved
---
# Extraction

- Triggered when the alert bar is full and no mandatory objective is pending ([[level-flow]]).
- The shuttle lands far from the miner. A countdown starts before company reinforcements arrive.
- **Risk window:** between landing and the end of the countdown, the player chooses to keep collecting ore and XP, or to run for the shuttle.
- Reaching the shuttle ends the level; ore carried is secured ([[ore-economy]]).
- Missing the shuttle loses the run and the ore carried.
- Values (provisional, owner 2026-10-03): lands 37 to 50 cells (67 uu, about 2500–3350 uu) from the miner (owner 2026-10-07: the exit was far too far; was at least 75 cells, no maximum), on a reachable cell; countdown 45 s.
- Landing clearance (provisional, owner after handover-2026-10-06): the shuttle lands only where its whole 4 × 3-cell footprint is open floor, so its mesh never overlaps rock. Replaced (owner 2026-10-29, provisional, task-376): the extractor lands on a reachable floor cell of the 37-50 cell band, preferring a spot whose 16 x 12-cell footprint is open floor; the rock of its footprint and of a 2-cell margin around it is destroyed ([[destruction-rules]]); it never leaves the band to find an open spot.
- M1 (single level): reaching the shuttle shows a success screen, missing it shows a run-lost screen; both offer a button that restarts the map.
- Look (provisional, owner 2026-10-29, task-366): the shuttle is the Rigs' Rustwing quadrotor, called the extractor; its mesh fits the 4 x 3-cell footprint; in game it is scaled x4 on each axis (owner 2026-10-29, provisional, task-375: about 786 x 804 x 344 uu, footprint 16 x 12 cells) and its four rotors are separate meshes so they can spin (files: node art-prompts-extractor).
- Extractor in game (owner 2026-10-29, provisional, tasks 369 to 374): the rule above is unchanged; the miner touching its board box validates the level, no button (owner 2026-10-30, provisional, task-377: an adjustable box, `BoardZone`, set in BP_ExtractionShuttle; default 1608 x 1608 uu centred on the extractor, 400 uu high from the floor, the square around the former 12-cell = 804 uu radius; the drone does not count; inactive on the arrival extractor; 3 cells before the x4 scale of task-375, 12 cells until task-377). It appears already landed (no landing animation); the grey-box glow pulse goes, the countdown chevrons stay. At the contact the success screen opens at once, the miner and his drone disappear, the game stays paused and only the extractor takes off behind the screen like a helicopter: rotors spin up 0.5 s on the ground, vertical rise of 400 uu in 1.5 s, then nose down 20 deg and acceleration along the nose until it leaves the camera view (about 2 s). Its rotors always spin (diagonal pairs opposite, 3 rev/s on the ground, 5 rev/s in flight, 0.5 s ramp); a rotor-wash effect plays under each rotor (downward air streaks and a ground dust ring within 300 uu of the floor, pale grey-beige, low opacity, intensity with the rotor speed). Arrival on a level: [[level-flow]]; blocks under it: [[destruction-rules]].
- Milestone: M1.

## Relations
- [[run-index]] — parent index.
- [[run-structure]] — run win and loss.
- [[lore]] — the shuttle and the reinforcements.
