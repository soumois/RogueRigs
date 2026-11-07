---
id: death-screen
description: "Run-lost screen when the miner dies: death sequence, appearance animation, look, texts, short recap, Retry and Main Menu buttons. Open for loss-screen work."
parent: run-index
kind: spec
status: provisional
---
# Death screen

Requested by the owner 2026-11-06 ([[task-484-death-screen]]). Values provisional: the owner accepted the defaults of task-484 Q1 to Q10 (his words in its request).

## Death sequence (Q1)
- Lethal hit: controls cut, the game runs at 30 % speed for 1 s real time, camera unchanged; then the screen appears and gameplay pauses. No miner death animation.

## Appearance (Q2)
- About 1.4 s in total: backdrop black 0 -> 75 % with the level desaturated behind (0.6 s); title drops in large (scale 1.4 -> 1, fade in, 0.25 s, short shake); recap frame slides up with a fade (0.3 s); buttons last (0.2 s). A click or key during the animation skips to the end state. No sound.

## Look (Q3)
- Same family as the level-end and pause screens ([[art-prompts-level-end-screen-mockup]], [[art-prompts-pause-menu-mockup]]): riveted frame, title side bars, cream rusty buttons, red title accent ([[color-code]]). One Kie.ai image-to-image mockup first, process of [[pause-menu]] "Look"; nothing built before the owner's "je valide". Layout rule [[umg-layout]].

## Texts (Q4)
- Title and subtitle in the company voice ([[company-voice]]) from the screen text file ([[sys-text-content]]), as the run-won screen; first proposal "CONTRACT TERMINATED" and one subtitle line, checked by the style guide; fallback title "RUN LOST".

## Recap (Q5)
- Level reached ("Level N of 4"), robots destroyed over the run (total), Duskite lost (the balance held at death, [[ore-economy]]).

## Buttons (Q6, Q7, Q8)
- "Retry" and "Main Menu", English, hardcoded in the widget. Keyboard and gamepad focus on Retry; mouse cursor shown; no confirmation.
- Retry: as [[main-menu]] New Run: run reset, level 1, loading screen "DROPPING IN", fade in.
- Main Menu: as the run-won Main Menu button ([[main-menu]]): run reset, L_MainMenu opened directly.
- While the screen shows: HUD hidden; the old GAME OVER text and the RUN LOST / Play again panel no longer appear on a loss.

## Missed shuttle (Q9)
- Same screen, own title and subtitle.

## Relations
- [[run-index]] — parent index.
- [[run-structure]] — run loss rules this screen ends.
- [[main-menu]] — New Run and Main Menu behaviour reused by the buttons.
- [[pause-menu]] — mockup process and look reused.
- [[task-484-death-screen]] — the task that builds it.
