---
id: ui-button-hover
description: "Mouse hover effect of every game screen button: scope, plate and text buttons, timing, focus, disabled, sound. Open for button or menu work."
parent: art-audio-index
kind: spec
status: open
---
# UI button hover

Requested by the owner 2026-11-06 ([[task-485-ui-button-hover]]): "il faudrait des effets de hover lorsqu'on passe au dessus des boutons dans toutes les interfaces".

Pending: the values below are the proposed defaults of task-485 Q1 to Q7, not yet answered.

## Scope (Q4, Q5)
- Every clickable button: main menu New Run and Exit; pause menu Resume and Exit; run-lost Retry and Main Menu; stats screen Continue; shop Reroll, Continue and the Buy button of each card; level-up Reroll; run-won end screen button.
- Not changed: the level-up cards keep their own hover (scale, glow, sheen, [[wbp-upgrade-choice]]); the shop card body has no hover, only its Buy button; the invisible skip area of the run-lost screen.
- A disabled button (Options, greyed Reroll or Buy) shows no hover effect.

## Look (Q1, Q2, Q3)
- Plate buttons (cream rusty plates): the plate brightens visibly, about +25 % brightness, same hue, and the button grows to 1.05; the label keeps its colour.
- Main menu text buttons: keep the orange of [[main-menu]] and grow to 1.05.
- Ease in and out over 0.1 s, real time, so it works on paused screens. The growth never moves or resizes neighbouring widgets: layouts and anchors are unchanged.
- Pressed look unchanged.

## Focus and sound (Q5, Q6)
- Keyboard or gamepad focus alone shows no hover look (the main menu keeps its focus colour).
- No sound ([[audio-direction]]: sound palette open, M3).

## Relations
- [[art-audio-index]] — parent index.
- [[main-menu]] — the text buttons and their orange.
- [[death-screen]] — Retry and Main Menu buttons.
- [[wbp-upgrade-choice]] — the card hover left unchanged.
- [[audio-direction]] — why no sound.
- [[task-485-ui-button-hover]] — the task that applies it.
