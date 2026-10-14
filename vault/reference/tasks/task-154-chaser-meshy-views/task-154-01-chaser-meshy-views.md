---
id: task-154-01-chaser-meshy-views
description: "Generate four reference views of the chaser (front, left, back, right) as input images for Meshy multi-view."
parent: task-154-chaser-meshy-views
kind: task
status: in-progress
---
# Chaser: four views for Meshy

- goal: Generate four reference views of the chaser (front, left, back, right) as input images for Meshy multi-view.
- agent: art
- depends_on: none
- inputs: art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), art-prompts-robots (vault/reference/gdd/art-audio/art-prompts-robots.md), kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md), agent-art (vault/reference/agents/agent-art.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), color-code (vault/reference/gdd/art-audio/color-code.md)
- expected_output: Art/Source/chaser-meshy/ with four PNG: chaser-meshy-front.png, chaser-meshy-left.png, chaser-meshy-back.png, chaser-meshy-right.png (3:4 portrait, 1K), one prompt file each in prompts/ (each says the image is a reference view for the image-to-3D tool Meshy), run logs, and a self-review per view against the sheet and the other views; generation order: front first from the sheet and the front crop, then back, left and right from the generated front plus the sheet
- done_when: Four images of the same robot (same parts, proportions and colours as vault/assets/chaser-turnaround.png), each a single full-body view on a plain light grey background, no shadow, no text, neutral pose; at most 6 images generated (36 credits); self-review written; prompts and logs committed by name; images untracked until the owner retains them; nothing in vault/assets/; no 3D, no Unreal
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-154-chaser-meshy-views]] — parent global task.
