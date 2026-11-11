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
- Round 1: open (waiting for his go).

## Relations
- [[art-prompts]] — parent: method and prompt nodes.
- [[options-menu]] — the screen this mockup shows.
- [[art-prompts-pause-menu-mockup]] — style reference, Image 1.
- [[art-prompts-level-end-screen-mockup]] — style reference, Image 2.
- [[task-504-01-options-mockup]] — the subtask that makes it.
