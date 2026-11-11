---
id: pause-menu
description: "In-run pause menu: how it opens, the miner stats shown (global and per weapon), Resume and Exit to main menu, look from a Kie.ai mockup."
parent: run-index
kind: spec
status: provisional
---
# Pause menu

Requested by the owner 2026-11-03 ([[task-424-pause-menu-stats]]). Values provisional: the owner accepted the defaults of task-424 Q1 to Q7 (his words in its request).

## Opening (provisional, task-424 Q2)
- Escape or gamepad Start during play pauses the game and shows the menu; the same key, or Resume, closes it. P does the same (Escape stops a Play In Editor session).
- Not available while another screen already pauses the game (level-up cards, intermission stats and cards, loading, game over, run won) nor in the main menu.
- Mouse cursor shown while open; keyboard focus on Resume.

## Content (provisional, task-424 Q3)
- Global: the miner's own stats of [[player-stats]] at their current value (max health and luck in points; move speed, pickup radius, XP gain, mining speed as a bonus % over base), then the sum of global weapon bonuses taken (damage %, fire rate %, reload speed %, critical chance and critical damage in points).
- Per weapon: one block per owned weapon with its current damage, fire rate, reload speed, critical chance, critical damage, magazine and pierce; locked slots not shown.
- Labels in English, hardcoded in the widget like [[main-menu]]; title "PAUSED"; buttons "Resume" and "Exit".

## Buttons (provisional, task-424 Q1)
- Resume: closes the menu and unpauses.
- Exit: no confirmation; ends the run as the run-won "Main Menu" button does ([[main-menu]]): run reset, ore back to 0, L_MainMenu opened directly, no loading screen, no fade.

## Look (owner 2026-11-03, decided in the request)
- One mockup made with Kie.ai GPT Image 2 image-to-image (`Tools/art/kie_image.js`), reference attached: the validated level-end mockup ([[art-prompts-level-end-screen-mockup]]). One round, credits reported; further rounds only on the owner's change requests.
- Nothing is extracted or built (C++ included) before the owner says "je valide". Then the elements are taken from the validated image, existing sprites reused where one matches; layout rule [[umg-layout]].
- Mockup: one image per round, 16:9, 1K (task-424 Q4). Elements: existing sprites first; a new element is cut from the validated mockup when clean at 1K, otherwise regenerated through Kie.ai image-to-image from it, 1 variant each, 24 credits at most, frames 3x3 (task-424 Q5).

## Relations
- [[run-index]] — parent index.
- [[player-stats]] — the stats shown.
- [[main-menu]] — where Exit leads; run-won Main Menu behaviour reused.
- [[art-prompts-level-end-screen-mockup]] — the style reference and reusable sprites.
- [[task-424-pause-menu-stats]] — the task that builds it.
- [[options-menu]] — the options screen its Options button opens.
