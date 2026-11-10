---
id: intermission
description: "Two screens between run levels: stats (recap, kills per robot, damage per weapon), then cards shop over the dimmed level while the next level loads."
parent: run-index
kind: spec
status: provisional
---
# Intermission

Owner 2026-10-17, provisional; split into two screens and preloading by the owner 2026-10-26 ([[task-317-stats-then-cards]], [[task-318-preload-next-level]]), provisional.

- Shown after a successful extraction on levels 1 to 3 ([[run-structure]]). Gameplay is paused. Two distinct screens, in this order.
- Stats screen (over the paused finished level): recap of the level, one ore line "Total Duskite:" (owner 2026-10-30, task-392 Q8, provisional; was "Total Ore"): the ore the player holds, i.e. the secured balance after this extraction, purchases and rerolls subtracted, the same number as the shop balance (owner 2026-10-30, task-391, final; was the ore secured since the run start, purchases not subtracted, task-383 Q1; replaces the lines ore this level and run total), robots destroyed per robot type (type names as text for now, no icons) and damage dealt per weapon. Button "Next level" opens the cards screen. Also shown after the level 4 extraction, where it leads to the run-won screen instead (no cards screen). Look (owner 2026-10-30, task-381 Q1 to Q5, provisional): the ChatGPT level-end mockup ([[art-prompts-level-end-screen-mockup]]): title with side bars, subtitle, riveted 9-slice frame whose thin top band holds the single Total Duskite line beside the ore icon, label and a larger number as the mockup (task-383 Q2, Q3), two columns (robots destroyed, damage per weapon) with names left and values right, split by bolted dividers, NEXT LEVEL button; texts unchanged, button drawn in upper case; the level 4 variant keeps this layout with its own texts.
- Cards screen: full-screen black at 70 % opacity over the finished level, which stays dimly visible behind (owner 2026-10-30, [[task-398-upgrade-screen-backdrop]]; was opaque black); the run shop ([[run-shop]]): 5 upgrade cards, ore balance, reroll; Continue button.
- The next terrain starts generating in the background as soon as the miner reaches the extractor, by the deterministic C++ generator ([[arena-grid]]; owner 2026-11-10, provisional, [[task-496-generator-in-runs]]; was: from the cards screen, with a layout written beforehand); when the cards screen opens the level is rebuilt behind it (map and pool prewarm), so the level can be fully ready before Continue. The stats screen still shows the finished level.
- Continue with the next level ready: fade to the next level (fade out 0.3 s, fade in 0.5 s, provisional, owner 2026-10-26). Not ready: loading screen with a progress bar until the level is ready, then fade in. Loading screen look (provisional, owner 2026-11-06, [[task-488-loading-screen-rework]]; was a plain dark screen with centred texts and an engine bar, image slot open): one full-screen static illustration in key art style, the same for New Run and every level (no text in the image; the run shuttle dropping toward the arena asteroid in the void, miner and drone aboard or at the hatch), a dark gradient at the bottom, and a riveted 9-slice frame in the stats screen style at bottom left holding the unchanged texts (DROPPING IN, Level N, stage line) and the bar, restyled as an orange fill in a dark riveted track; the spinning ore icon replaces the bar inside the frame when progress is not measured. No music, no image animation. When the progress cannot be measured (a map load: the map-travel fallback, the main menu New Run), the bar is hidden and a looping loading animation is shown instead (owner 2026-10-30, task-379; provisional, task-379 Q4 and Q5: the HUD ore icon T_Icon_Ore at 128 x 128 where the bar was, turning one turn per 2 s; it stops during the blocking map load itself (no engine loading screen) and turns before and after it). The level starts only when it is ready.
- Playing a map directly from the editor starts at level 1 with no menu and no intermission before it.

## Relations
- [[run-index]] — parent index.
- [[run-structure]] — where the intermission sits in a run.
- [[run-shop]] — what can be bought on this screen.
- [[arena-grid]] — where the next layout comes from.
- [[art-prompts-level-end-screen-mockup]] — the stats screen look (task-381).
