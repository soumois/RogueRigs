---
id: art-prompts-options-screen-mockup
description: "Options screen mockup, Kie.ai image-to-image from the pause and level-end mockups: prompt, image, log, credits, review, go; elements taken (task-510-02)."
parent: art-prompts
kind: note
---
# Options screen mockup (Kie.ai, task-510-01)

Content of [[options-screen]] (Graphics tab shown), style of [[art-prompts-pause-menu-mockup]] (Image 1) and [[art-prompts-level-end-screen-mockup]] (Image 2).

## Round 1
- Prompt: `Art/Source/options-screen/prompts/01-mockup.txt`.
- Command: `node Tools/art/kie_image.js --asset options-screen --prompt-file Art/Source/options-screen/prompts/01-mockup.txt --ref Art/Source/pause-menu/20261102-223354-pause-menu-1.png --ref Art/Source/level-end-screen/reference/chatgpt-level-end-mockup-2026-10-30.webp --aspect 16:9 --variants 1`.
- Image (keep): `Art/Source/options-screen/20261112-062655-options-screen-1.png` (1K, 16:9). Log: `Art/Source/options-screen/20261112-062655-options-screen.json`, model gpt-image-2-image-to-image, 2 refs.
- Credits: 6. Total 6.
- Content: title OPTIONS with side bars; tabs Sound, Graphics (active, cream plate), Gameplay; frame in two columns "Display" (Window mode, Resolution, Frame rate limit arrow selectors, VSync pill toggle, Resolution scale slider) and "Quality" (preset and five sub-settings, arrow selectors); buttons "Reset to defaults" and "Back".

## Review
- [[agent-style-guide]]: pass, 0 fails. Notes without rule: the slider knob sits mid-track for 100 %; "Reset to defaults" wider than "Back"; VSync is a pill toggle, a second control style.

## Go
- Given by the main session under the owner sentence of 2026-11-11 ("Si la maquette est proche et que je n'ai pas fait de commentaire, tu pourras valider ( ou si mon commentaire est valable pour la maquette aussi"), not the owner's own "je valide". Noted for him: rows in two columns (Display, Quality); the slider knob point kept, no new round.

## Elements (task-510-02)
Kie.ai gpt-image-2 image-to-image from the mockup (Image 1), magenta key, 1 variant per run, keyed and trimmed with `Tools/art/ui_icon_prepare.js --keep-aspect --margin 0` (sheets split into rows first). Runs in `Art/Source/options-screen/` (PNG and log `.json` per run id, keep): `20261112-063128` tab (prompt `02-tab.txt`, Image 2 `T_Shop_ButtonContinue`); `20261112-063653` selector plate and arrow (`03-selector.txt`); `20261112-063814` slider track, fill, knob (`04-slider.txt`); `20261112-064153` round toggle (`05-toggle.txt`; image and log temporary, moved to `Saved/Scratch/`); `20261112-101341` faceted toggle (`06-toggle-faceted.txt`, correction of the style-guide A1 fail). Credits: 5 runs x 6 = 30 (cap 36).

| Element | Asset | 3-slice margins (fraction, by eye) |
|---|---|---|
| Frame | reused `Art/UI/LevelEnd/T_LevelEnd_Frame.png` | as [[art-prompts-level-end-screen-mockup]] |
| Column divider | reused `Art/UI/LevelEnd/T_LevelEnd_DividerV.png` | as level-end |
| Title side bars | reused `Art/UI/Shop/T_Shop_TitleBar.png` | none |
| Reset to defaults, Back, active tab | reused `Art/UI/Shop/T_Shop_ButtonContinue.png` | as pause menu |
| Inactive tab | `Art/UI/Options/T_Options_Tab.png` 1024 x 199 | L, R 0.10 |
| Selector plate | `Art/UI/Options/T_Options_SelectorPlate.png` 1024 x 136 | L, R 0.08 |
| Selector arrow (right; left mirrored in UMG) | `Art/UI/Options/T_Options_Arrow.png` 256 x 234 | none |
| Slider track and fill | `T_Options_SliderTrack.png` 1024 x 78, `T_Options_SliderFill.png` 1024 x 75 | L, R 0.03 |
| Slider knob | `T_Options_SliderKnob.png` 96 x 128 | none |
| Toggle on, off | `T_Options_ToggleOn.png` 512 x 123, `T_Options_ToggleOff.png` 512 x 125 | L, R 0.08 |
| Toggle knob | `T_Options_ToggleKnob.png` 256 x 254 | none |
| Header lines, labels, values | plain UMG lines and text | none |

Resized in task-510-07 to their drawn size (Slate draws Box margins at the texture size): Tab 396 x 77, SelectorPlate 346 x 46, SliderTrack 263 x 20, SliderFill 263 x 19, ToggleOn and ToggleOff 164 x 40; same margins.

Review ([[agent-style-guide]]): tab, selector and slider pass; the round toggle failed A1 (no facets) and was corrected by run 101341. Notes kept: the selector plate has teal end caps where the mockup plate has none; the fill bar carries its own rim.

## Relations
- [[art-prompts]] — parent: method and prompt nodes.
- [[options-screen]] — the screen this mockup shows.
- [[art-prompts-pause-menu-mockup]] — Image 1, style reference.
- [[art-prompts-level-end-screen-mockup]] — Image 2, style reference.
- [[task-510-01-options-kie-mockup]] — the subtask that made it.
- [[task-510-02-options-elements]] — the subtask that made the elements.
