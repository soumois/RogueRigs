---
id: art-prompts-options-screen-mockup
description: "Options screen mockup from Kie.ai image-to-image off the pause and level-end mockups: prompt files, images, logs, credits per round, review, owner words."
parent: art-prompts
kind: note
---
# Options screen mockup (Kie.ai, task-504-01)

Content of [[options-menu]] (Graphics tab shown), style of [[art-prompts-pause-menu-mockup]] (Image 1) and [[art-prompts-level-end-screen-mockup]] (Image 2), both attached with `--ref`. All files labelled keep, in `Art/Source/options-screen/`.

## Round 1
- Prompt: `Art/Source/options-screen/prompts/01-mockup.txt` (one full prompt, no correction).
- Command: `node Tools/art/kie_image.js --asset options-screen --prompt-file Art/Source/options-screen/prompts/01-mockup.txt --ref Art/Source/pause-menu/20261102-223354-pause-menu-1.png --ref Art/Source/level-end-screen/reference/chatgpt-level-end-mockup-2026-10-30.webp --aspect 16:9 --variants 1`.
- Image: `Art/Source/options-screen/20261111-224939-options-screen-1.png` (1K, 16:9). Log: `Art/Source/options-screen/20261111-224939-options-screen.json`, model gpt-image-2-image-to-image, 2 refs.
- Credits: 6 (round 1). Total 6.
- Review (style-guide, task-504-01): content complete (title, three tabs with Graphics selected, the 11 Graphics rows with a divider after Resolution scale, Reset to defaults and Back), English, no yellow, green or blue, style matches the pause mockup. One fail inherited from the validated pause mockup: the dimmed arena reads charcoal, not lilac rock tops and a magenta floor (colour code, A13); it is the darkened backdrop, a UMG overlay in game. Observation: the Resolution scale knob sits at about 85 % of the track while the value reads 100 %, the maximum (50 to 100 %).
- Self-review gaps: the VSync toggle thumb is grey; the selected tab sits just above the frame rather than joining it; values are placeholders.

## Owner words
- Round 1: "Je valide" (2026-11-11), after seeing the image and the three review points; no correction round. The slider knob at about 85 % for 100 %, the charcoal backdrop, the grey VSync thumb, the tab not joining the frame and the placeholder values are kept on purpose.

## Elements (task-504-02)
New controls regenerated from the validated mockup as one sheet on flat magenta: prompt `Art/Source/options-screen/prompts/02-elements-sheet.txt`, image `Art/Source/options-screen/20261111-225406-options-screen-1.png`, log `20261111-225406-options-screen.json`, 1 variant, 6 credits (cap 36; task total 12). Keyed with `Tools/art/ui_icon_prepare.js --keep-aspect --margin 0` (no magenta left), imported to `/Game/RogueRigs/UI/Options/` with the settings of `T_LevelEnd_Frame` (UI group, no mips). Margins are estimates, untested.

| Element | File | Margins (UMG fraction) | Credits |
|---|---|---|---|
| Main frame | `Art/UI/LevelEnd/T_LevelEnd_Frame.png` (reused) | as in [[art-prompts-pause-menu-mockup]] | 0 |
| Title side rails | `Art/UI/Shop/T_Shop_TitleBar.png` (reused, mirrored) | none | 0 |
| Reset to defaults, Back, selected tab | `Art/UI/Shop/T_Shop_ButtonContinue.png` (reused) | as in the pause node | 0 |
| Unselected tab | `Art/UI/Options/T_Options_TabPlate.png` 675 x 179 | L, R 0.12; T, B 0 | sheet |
| Selector arrows | `T_Options_ArrowButton.png` 193 x 185, right arrow mirrored | none (fixed size) | sheet |
| Slider track | `T_Options_SliderTrack.png` 675 x 71 | L, R 0.09; T, B 0 | sheet |
| Slider knob | `T_Options_SliderKnob.png` 153 x 156 | none | sheet |
| Toggle track (On and Off) | `T_Options_ToggleTrack.png` 309 x 132 | L, R 0.10; T, B 0.23 | sheet |
| Toggle thumb (right and "On", left and "Off") | `T_Options_ToggleThumb.png` 144 x 130 | none | sheet |
| Group divider, labels, values | plain UMG line and text | none | 0 |
| Dimmed backdrop | UMG dark overlay | none | 0 |

Review (style-guide, task-504-02): pass, no fail. Gap for the owner: the knob is a teal octagon with a cream dome, not the small square knob with a glowing teal centre of the mockup.

## Relations
- [[art-prompts]] — parent: method and prompt nodes.
- [[options-menu]] — the screen this mockup shows.
- [[art-prompts-pause-menu-mockup]] — style reference, Image 1.
- [[art-prompts-level-end-screen-mockup]] — style reference, Image 2.
- [[task-504-01-options-mockup]] — the subtask that makes it.
