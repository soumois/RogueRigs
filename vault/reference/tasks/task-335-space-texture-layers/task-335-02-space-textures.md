---
id: task-335-02-space-textures
description: "Write bold prompts, regenerate the deep and cloud space images via Kie.ai, get owner validation, make them tileable at 2048 with previews."
parent: task-335-space-texture-layers
kind: task
status: in-progress
---
# Space layers: bold images and tileable textures

- goal: two bold, saturated, structured space images (deep, clouds) validated by the owner, turned into seamless 2048 px textures ready for import.
- agent: art
- depends_on: task-335-01-space-prompts
- source_change: S2 (owner 2026-10-27): the art agent generates through Kie.ai. B1 to B5 (owner "ok" 2026-10-27, after "ça reste trés timide" on the first pair): new bold prompts and regeneration of both images; the first pair (deep 20261027-141216, clouds 20261027-141558) stays in the folder as fallback, not retained.
- inputs: the global `request`, `gdd_check` and `feedback_2` with B1 to B7 (vault/reference/tasks/task-335-space-texture-layers.md); art-prompts-space-background (vault/reference/gdd/art-audio/art-prompts-space-background.md, first chain and its timid values); color-code (vault/reference/gdd/art-audio/color-code.md, void row as rewritten by B7); readability-rules (vault/reference/gdd/art-audio/readability-rules.md, reserved colours); kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md); art-prompts-rock-top-texture (vault/reference/gdd/art-audio/art-prompts-rock-top-texture.md, seam method); art-3d-lessons (vault/reference/agents/art-3d-lessons.md).
- expected_output: prompts `Art/Source/space-background/prompts/05-space-deep-bold.txt` and `06-space-clouds-bold.txt` (plus a correction file if used): square 1:1, tileable, elements crossing the borders, near-black share about 20 to 25 percent, saturated indigo, electric violet and magenta, deep ultramarine blue as second hue (mainly in the deep layer, never grey steel blue), bright cores, dense filaments, dark dust lanes; deep layer with large visible shapes, not a flat wash; no star, planet, asteroid, text, frame, vignette; no green, steel blue, red, orange, cyan, teal, salmon or pink. One Kie.ai image per layer, at most one correction each (24 credits max). Raw images shown to the owner; stop for his validation (the Executor sets `wait-answers`). Then `T_Space_Deep.png` and `T_Space_Clouds.png` (2048, `node Tools/art/make_tileable.js --in <image> --out <tileable> --preview <3x3> --size 2048`) and their `_tiled3x3.png` previews. art-prompts-space-background updated: bold chain, checklist (B values replace the hex caps and the 40 percent black), `## Files`. No engine work.
- done_when: the owner validated both raw images; both textures are 2048 x 2048; the 3 x 3 previews show no seam line and no strong repeating motif (else reported with a correction prompt); hues in the B3 palette, no reserved colour; at most 24 credits spent; retained files, prompts and node committed by explicit path.
- editor_state: any
- token_ceiling: 0.15 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: 2026-10-27T22:40:11Z
- waiting_for: none
- result: open

## Relations
- [[task-335-space-texture-layers]] — parent global task.
