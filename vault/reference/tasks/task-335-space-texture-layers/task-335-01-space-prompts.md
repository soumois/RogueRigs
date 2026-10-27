---
id: task-335-01-space-prompts
description: "Write the ChatGPT prompts for the two tileable space layers (deep washes, cloud wisps) and their prompt node, then hand them to the owner."
parent: task-335-space-texture-layers
kind: task
status: todo
---
# Space layers: ChatGPT prompts

- goal: the owner has two ready-to-paste prompts (deep layer, cloud layer) that give square tileable space textures in the void palette, stored in the repo and a prompt node.
- agent: art
- depends_on: none
- inputs: the global `request` and `gdd_check` with the owner answers S1 to S10 (vault/reference/tasks/task-335-space-texture-layers.md); art-prompts-floor-texture (vault/reference/gdd/art-audio/art-prompts-floor-texture.md, the model to follow: chain, route, seam method, checklist); art-prompts-rock-top-texture (vault/reference/gdd/art-audio/art-prompts-rock-top-texture.md); art-prompts (vault/reference/gdd/art-audio/art-prompts.md); color-code (vault/reference/gdd/art-audio/color-code.md, void palette); readability-rules (vault/reference/gdd/art-audio/readability-rules.md, reserved colours); art-3d-lessons (vault/reference/agents/art-3d-lessons.md); kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md, only if S2 chose the Kie.ai route).
- expected_output: `Art/Source/space-background/prompts/01-space-deep.txt` and `02-space-clouds.txt`, plus a correction prompt `03-space-correction.txt` ("Keep this exact image: same ..." changing one thing); each asks for a flat square 1:1 texture, near-black base, palette and hex range of the void line in color-code, content per S3, elements crossing the borders, no frame, vignette, star, planet, asteroid or text. Node `art-prompts-space-background`, child of art-prompts (sections: need, prompt chain, generation route, seam method, review checklist, files), with art-prompts updated. No image generated on the ChatGPT route; on the Kie.ai route, 2 variants per layer at most, shown to the owner.
- done_when: the three prompt files exist; the node passes lint and is linked both ways; the prompts are handed to the owner with the file names to save (`Art/Source/space-background/owner-<date>-space-deep.png`, `...-space-clouds.png`); committed by explicit path. The Executor then sets the global `wait-answers` for the owner's images.
- editor_state: any
- token_ceiling: 0.15 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-335-space-texture-layers]] — parent global task.
