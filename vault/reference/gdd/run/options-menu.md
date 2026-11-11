---
id: options-menu
description: "Options screen: where it opens, Sound, Graphics and Gameplay settings, persistence, layout and Kie.ai mockup look. Open for options or settings work."
parent: run-index
kind: spec
status: open
---
# Options screen

Requested by the owner 2026-11-11 ([[task-504-options-screen-look]]): "Ok, on va faire l'écran des options [...] Il va falloir passer par l'api chatgpt pour les images ( Kie ). Il faut prévoir aussi les options classiques pour le son, la qualité, et différents élément gamplay."

Values below: proposed defaults of task-504 Q1 to Q9, awaiting the owner.

## Opening (Q1)
- Main menu: the Options plate becomes active (plate hover of [[ui-button-hover]]) and opens this screen over the menu.
- Pause menu: a third button "Options" between Resume and Exit opens it over the pause menu; the game stays paused.
- Back or Escape returns to the screen it came from, focus on its Options button.

## Layout (Q2)
- Title "OPTIONS", three tabs "Sound", "Graphics", "Gameplay", buttons "Reset to defaults" (current tab only) and "Back". Labels in English, hardcoded like [[main-menu]].
- A change applies at once: no Apply button, no confirmation countdown after a resolution change.
- Mouse first; keyboard focus starts on the first tab; layout rule [[umg-layout]].

## Sound (Q3)
- Master, Music, Effects: sliders 0 to 100 %, step 5, default 100 %.
- The game has no sound yet ([[audio-direction]], M3): the sliders drive the volume groups future sounds will use.

## Graphics (Q4)
- Window mode: Fullscreen, Windowed fullscreen, Windowed. Resolution: the monitor list (Fullscreen and Windowed).
- VSync: On, Off. Frame rate limit: 30, 60, 120, 144, Unlimited.
- Quality preset: Low, Medium, High, Epic, Custom. It sets Shadows, Effects, Anti-aliasing, Post-processing and Textures (Low to Epic each), each changeable alone (the preset then reads Custom).
- Resolution scale: 50 to 100 %, step 5.
- First-launch values: what the game uses today, recorded here by the build task.

## Gameplay (Q5)
- Damage numbers ([[damage-numbers]]): On, Off; default On.
- Aim indicator ([[aim-indicator]]): On, Off; default On.
- Screen shake: On, Off; default On; only if the game has a camera shake, otherwise left out.
- Pause when the window loses focus: On, Off; default On; in a run only.

## Persistence (Q6)
- Saved on disk per Windows user, outside the repository, at each change; restored and applied at every launch.

## Look (Q7)
- One mockup through Kie.ai GPT Image 2 image-to-image (`Tools/art/kie_image.js`), references the validated pause menu and level-end mockups ([[art-prompts-pause-menu-mockup]], [[art-prompts-level-end-screen-mockup]]), Graphics tab shown, 16:9, 1K, one image per round, credits reported; further rounds only on the owner's change requests. Nothing extracted or built before his "je valide".
- Elements: existing sprites first; a new element (tab, slider, toggle, left/right selector) is cut from the validated mockup when clean at 1K, otherwise regenerated through Kie.ai image-to-image from it, 1 variant each, 36 credits at most. Files kept in `Art/Source/options-screen/`.

## Relations
- [[run-index]] — parent index.
- [[main-menu]] — opens it from Options.
- [[pause-menu]] — opens it from its Options button.
- [[ui-button-hover]] — hover of its buttons.
- [[audio-direction]] — why the sound sliders have nothing to play yet.
- [[damage-numbers]] — turned off by a Gameplay toggle.
- [[aim-indicator]] — turned off by a Gameplay toggle.
- [[art-prompts-pause-menu-mockup]] — style reference of the mockup.
- [[task-504-options-screen-look]] — the task that decides and draws it.
