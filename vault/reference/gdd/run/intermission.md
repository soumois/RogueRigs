---
id: intermission
description: "Two screens between run levels: stats (recap, kills per robot, damage per weapon), then cards shop on black while the next level loads."
parent: run-index
kind: spec
status: provisional
---
# Intermission

Owner 2026-10-17, provisional; split into two screens and preloading by the owner 2026-10-26 ([[task-317-stats-then-cards]], [[task-318-preload-next-level]]), provisional.

- Shown after a successful extraction on levels 1 to 3 ([[run-structure]]). Gameplay is paused. Two distinct screens, in this order.
- Stats screen (over the paused finished level): recap of the level, ore secured in this level and run total, robots destroyed per robot type (type names as text for now, no icons) and damage dealt per weapon. Button "Next level" opens the cards screen. Also shown after the level 4 extraction, where it leads to the run-won screen instead (no cards screen).
- Cards screen: opaque black background (the finished level is no longer visible); the run shop ([[run-shop]]): 5 upgrade cards, ore balance, reroll; Continue button.
- When the cards screen opens, the next level starts loading in the background: terrain from the deterministic C++ generator ([[arena-grid]]; until it exists, a layout written beforehand by the Level-Design agent), map and pool prewarm, so the level can be fully ready before Continue.
- Continue with the next level ready: fade to the next level (fade out 0.3 s, fade in 0.5 s, provisional, owner 2026-10-26). Not ready: loading screen with a progress bar (an image slot is kept for later: image open) until the level is ready, then fade in. The level starts only when it is ready.
- Playing a map directly from the editor starts at level 1 with no menu and no intermission before it.

## Relations
- [[run-index]] — parent index.
- [[run-structure]] — where the intermission sits in a run.
- [[run-shop]] — what can be bought on this screen.
- [[arena-grid]] — where the next layout comes from.
