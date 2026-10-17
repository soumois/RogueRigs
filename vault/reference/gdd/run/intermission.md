---
id: intermission
description: "Screen between two levels of a run: level recap, run shop slot, Continue; next terrain prepared meanwhile, loading screen if not ready."
parent: run-index
kind: spec
status: provisional
---
# Intermission

Owner 2026-10-17, provisional.

- Shown after a successful extraction on levels 1 to 3 ([[run-structure]]). Gameplay is paused.
- Recap of the level: ore secured in this level and run total. Later, once run logs exist: damage per second per weapon and robots killed.
- Run shop slot: purchases valid for the current run only ([[run-shop]]). The first version is a placeholder with an empty shop area.
- Continue button: starts the next level.
- While the player is on this screen, the next level's terrain is already being prepared (the deterministic C++ generator, [[arena-grid]]; until it exists, a layout written beforehand by the Level-Design agent is loaded in the background).
- Pressing Continue before the terrain is ready shows a loading screen. The loading screen also covers the map load and the pool prewarm, and the level starts only when they are done.
- Playing a map directly from the editor starts at level 1 with no menu and no intermission before it.

## Relations
- [[run-index]] — parent index.
- [[run-structure]] — where the intermission sits in a run.
- [[run-shop]] — what can be bought on this screen.
- [[arena-grid]] — where the next layout comes from.
