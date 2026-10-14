---
id: task-151-02-chaser-sheet-simplified
description: "Correct sheet A2: three views only, simplified chunky facets, slimmer limbs like the reference crop."
parent: task-151-chaser-robot
kind: task
status: done
---
# Chaser: simplified three-view sheet

- goal: Correct sheet A2: three views only, simplified chunky facets, slimmer limbs like the reference crop.
- agent: art
- depends_on: task-151-01-chaser-robot
- inputs: art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md), color-code (vault/reference/gdd/art-audio/color-code.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), agent-art (vault/reference/agents/agent-art.md)
- expected_output: Art/Source/chaser/prompts/turnaround-a2-simplified.txt (a correction prompt opening with "Keep this exact image: same ..."; Image 1 = Art/Source/chaser/20261013-183518-chaser-2.png, the sheet being corrected, Image 2 = Art/Source/chaser/ref-chaser.png, the proportions to follow; state that in the prompt) asking for: the same layout in a row but THREE views only (FRONT, SIDE, BACK, same scale, no top view), the same neutral pose, colours, red eye, logo plate and plain light grey background; every part simplified to chunky low-poly volumes with large flat facets, no small panel lines, bolts, cables or cylinders; slimmer limbs and thinner legs as in Image 2 (lanky chaser, large boxy head and torso block); the same robot in the three views; no text. Generated with kie_image.js image-to-image, 16:9, 2 variants (12 credits), run log; a self-review of both against the A rules and Image 2 (name every difference: proportions of legs, arms and head box, number of visible parts, facets, three views agree).
- done_when: exactly one run of 2 variants (12 credits); both sheets show three views of one robot; self-review written; prompt and log committed by name, the sheets untracked; nothing in vault/assets/; no 3D, no Unreal
- editor_state: closed
- token_ceiling: open
- model: sonnet
- tokens: input 14, cache write 22012, cache read 91582, output 660
- runs: 1
- milestone: M2
- result: x

## Relations
- [[task-151-chaser-robot]] — parent global task.
