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
- Stats screen (over the paused finished level): recap of the level, one ore line "Total Duskite:" (owner 2026-10-30, task-392 Q8, provisional; was "Total Ore"): the ore secured since the run start, purchases not subtracted (owner 2026-10-30, task-383 Q1, provisional; replaces the lines ore this level and run total), robots destroyed per robot type (type names as text for now, no icons) and damage dealt per weapon. Button "Next level" opens the cards screen. Also shown after the level 4 extraction, where it leads to the run-won screen instead (no cards screen). Look (owner 2026-10-30, task-381 Q1 to Q5, provisional): the ChatGPT level-end mockup ([[art-prompts-level-end-screen-mockup]]): title with side bars, subtitle, riveted 9-slice frame whose thin top band holds the single Total Duskite line beside the ore icon, label and a larger number as the mockup (task-383 Q2, Q3), two columns (robots destroyed, damage per weapon) with names left and values right, split by bolted dividers, NEXT LEVEL button; texts unchanged, button drawn in upper case; the level 4 variant keeps this layout with its own texts.
- Cards screen: opaque black background (the finished level is no longer visible); the run shop ([[run-shop]]): 5 upgrade cards, ore balance, reroll; Continue button.
- When the cards screen opens, the next level starts loading in the background: terrain from the deterministic C++ generator ([[arena-grid]]; until it exists, a layout written beforehand by the Level-Design agent), map and pool prewarm, so the level can be fully ready before Continue.
- Continue with the next level ready: fade to the next level (fade out 0.3 s, fade in 0.5 s, provisional, owner 2026-10-26). Not ready: loading screen with a progress bar (an image slot is kept for later: image open) until the level is ready, then fade in. When the progress cannot be measured (a map load: the map-travel fallback, the main menu New Run), the bar is hidden and a looping loading animation is shown instead (owner 2026-10-30, task-379; provisional, task-379 Q4 and Q5: the HUD ore icon T_Icon_Ore at 128 x 128 where the bar was, turning one turn per 2 s; it stops during the blocking map load itself (no engine loading screen) and turns before and after it). The level starts only when it is ready.
- Playing a map directly from the editor starts at level 1 with no menu and no intermission before it.

## Relations
- [[run-index]] — parent index.
- [[run-structure]] — where the intermission sits in a run.
- [[run-shop]] — what can be bought on this screen.
- [[arena-grid]] — where the next layout comes from.
- [[art-prompts-level-end-screen-mockup]] — the stats screen look (task-381).
