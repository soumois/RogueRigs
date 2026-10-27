---
id: task-335-02-space-textures
description: "Save the owner's two space images in the repo, make them tileable at 2048 with 3 x 3 previews, review and record them."
parent: task-335-space-texture-layers
kind: task
status: wait-answers
---
# Space layers: tileable textures from the owner images

- goal: two seamless 2048 px textures, deep and clouds, from the images the owner retained, ready for import.
- agent: art
- depends_on: task-335-01-space-prompts
- source_change: S2 changed by the owner 2026-10-27: the art agent generates the two source images through Kie.ai (`Tools/art/kie_image.js`) from prompts/01 and 02 (03 for a correction), saved as `Art/Source/space-background/<run-id>-1.png`, instead of owner files. Stop for owner validation of the raw images before the tileable step.
- inputs: the global `request` and `gdd_check` (vault/reference/tasks/task-335-space-texture-layers.md); the prompt node art-prompts-space-background written by task-335-01 (path in its result); art-prompts-floor-texture (vault/reference/gdd/art-audio/art-prompts-floor-texture.md, seam method and checklist); art-prompts-rock-top-texture (vault/reference/gdd/art-audio/art-prompts-rock-top-texture.md, half-shift cross-fade); color-code (vault/reference/gdd/art-audio/color-code.md); the owner's files as he gives them.
- expected_output: in `Art/Source/space-background/`: the owner files untouched plus lossless PNG copies; `T_Space_Deep.png` and `T_Space_Clouds.png` (2048, from `node Tools/art/make_tileable.js --in <image> --out <tileable> --preview <3x3> --size 2048`); `T_Space_Deep_tiled3x3.png` and `T_Space_Clouds_tiled3x3.png`; the review checklist answered per image in the prompt node, `## Files` filled. Previews sent to the owner. No engine work.
- done_when: both textures are 2048 x 2048; the 3 x 3 previews show no seam line and no strong repeating motif (else reported, with a correction prompt for the owner); hues inside the void palette, no reserved colour; files and node committed by explicit path.
- editor_state: any
- token_ceiling: 0.15 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: 2026-10-27T22:11:46Z
- waiting_for: owner validates the raw images Art/Source/space-background/20261027-141216-space-background-1.png (deep) and 20261027-141558-space-background-1.png (clouds), relayed by the main session; then make_tileable and previews
- result: open

## Relations
- [[task-335-space-texture-layers]] — parent global task.
