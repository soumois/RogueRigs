---
id: task-273-01-floor-texture
description: "Image prompts for ChatGPT of a tileable painted floor of cracked faceted plates, with seam method and review; no image generated."
parent: task-273-floor-texture
kind: task
status: in-progress
---
# Floor texture prompts

- goal: Prompts the owner pastes into ChatGPT to generate a square, top-down, tileable painted floor texture of large cracked faceted plates (own design, not a copy of the DRGS references), plus the seam method and review checklist for the image he validates.
- agent: art
- depends_on: none
- inputs: art-3d-lessons (vault/reference/agents/art-3d-lessons.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), art-prompts-rock-top-texture (vault/reference/gdd/art-audio/art-prompts-rock-top-texture.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md); references Saved/LookTune/ref/crop_core.png, crop_farleft.png, drgs_frame_004.png
- expected_output: Prompt files in `Art/Source/floor-texture/prompts/`; prompt node `art-prompts-floor-texture`, child of art-prompts; no image generated (no Kie run).
- done_when: Prompts written (tileable, straight top-down, no baked light or shadow, no rubble, own design, base colour stated); seam method and review checklist in the node; node linked from art-prompts; lint clean; committed by path.
- editor_state: any
- token_ceiling: 30k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-273-floor-texture]] — parent global task.
