---
id: options-screen
description: "Options screen: where it opens, Sound, Graphics and Gameplay settings, display keep-or-revert countdown, saving, Kie.ai mockup look. Open for options or settings work."
parent: run-index
kind: spec
status: provisional
---
# Options screen

Requested by the owner 2026-11-12 ([[task-510-options-screen]]). Values provisional, owner 2026-11-11/12: his answers relayed in the task request ("ok pour tout", display countdown remark accepted); where his set did not cover a point, the task-510 default stands.

## Opening
- Main menu Options button active (no longer greyed, [[main-menu]]); pause menu third button "Options" between Resume and Exit ([[pause-menu]]), the game stays paused.
- Back or Escape returns to the menu it came from.

## Layout
- Title "OPTIONS"; tabs "Sound", "Graphics", "Gameplay"; buttons "Reset to defaults" (current tab) and "Back". English labels hardcoded in the widget, like [[main-menu]]. Buttons take [[ui-button-hover]]. The three tabs keep the same size, selected or not (owner 2026-11-13, task-521).
- Black backdrop as the pause menu, over the main menu image or the paused arena; [[umg-layout]].

## Sound
- Sliders Master, Music, Effects, 0 to 100 % in steps of 5, 100 % at start, applied live. They drive the Master, Music and Effects volume groups of every sound and music track ([[audio-direction]]). Moving Effects plays a sample effect at the new level (owner 2026-11-13, provisional, task-520, see [[audio-direction]]).

## Graphics
- No Apply button: every change applies at once.
- Window mode: Fullscreen, Windowed fullscreen, Windowed. Resolution: the list of the screen.
- After a window mode or resolution change: dialog "Keep these display settings?" with "Keep" and "Revert", 15 s countdown; Escape or the end of the countdown restores the previous values.
- VSync On/Off. Frame rate limit 30, 60, 120, 144, Unlimited. Resolution scale 50 to 100 %.
- Quality preset Low, Medium, High, Epic, Custom: sets Shadows, Effects, Anti-aliasing, Post-processing and Textures, each also editable alone (the preset then reads Custom).
- First launch keeps the game's current values.

## Gameplay
- On/Off toggles, all On at start: "Damage numbers" (also hides heal and ore numbers, [[damage-numbers]]); "Aim indicator" ([[aim-indicator]]); "Screen shake" (every camera shake, the [[miner]] hit shake included); pause when the window loses focus (during play it opens the pause menu where Escape would).
- Aim assist keeps no menu option ([[miner]]).

## Saving
- Saved to disk at every change, per Windows user, outside the repository; reloaded and applied at every launch. While the display countdown runs, the saved display values are the previous ones.

## Look
- One mockup per round, Graphics tab shown, Kie.ai GPT Image 2 image-to-image, 16:9, 1K ([[kie-image-workflow]]), references the validated pause menu mockup ([[task-424-01-pause-menu-kie-mockup]]) and the level-end mockup ([[art-prompts-level-end-screen-mockup]]); credits reported; further rounds only on request; nothing cut or built before the go.
- Elements: existing sprites first, else cut from the mockup, else regenerated, 1 variant each, at most 36 credits.

## Relations
- [[run-index]] — parent index.
- [[main-menu]] — opens it.
- [[pause-menu]] — opens it during a run.
- [[damage-numbers]] — the toggle the GDD already asked for.
- [[miner]] — hit shake and aim assist.
- [[aim-indicator]] — the aim indicator toggled.
- [[audio-direction]] — the sounds the sliders will drive.
- [[task-510-options-screen]] — the task that builds it.
