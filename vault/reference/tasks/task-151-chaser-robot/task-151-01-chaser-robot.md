---
id: task-151-01-chaser-robot
description: "Build the chaser robot: turnaround sheet, parts model, baked run and attack animation, in-game integration and a cost check."
parent: task-151-chaser-robot
kind: task
status: done
---
# Chaser: turnaround sheet

- goal: Build the chaser robot: turnaround sheet, parts model, baked run and attack animation, in-game integration and a cost check.
- agent: art
- depends_on: none
- inputs: art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), art-prompts-robots (vault/reference/gdd/art-audio/art-prompts-robots.md), color-code (vault/reference/gdd/art-audio/color-code.md), kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md)
- expected_output: Art/Source/chaser/ with: a crop of the chaser from vault/assets/enemies-lineup.jpg as the reference (ref-chaser.png; the chaser is the yellow robot at the left, about x 10 to 390 and y 350 to 735 of the 1600 x 900 image; crop with PowerShell System.Drawing, no other tool); up to 4 candidate sheets (1K, image-to-image from the crop, GPT Image 2 through Kie.ai, 6 credits each, at most 2 runs of 2 variants): each sheet shows the SAME chaser in a neutral standing pose (legs straight, arms slightly forward and away from the body, no weapon, no effect, even light, no shadows, plain light grey background) from FRONT, SIDE, BACK and TOP, all four views in one image, same scale, parts and colours identical to the crop (company yellow, dark gunmetal joints, one red eye in a boxy head, small yellow logo plate, no hazard stripes), flat-shaded low-poly visible facets; two different layouts: prompt A all four views in a row, prompt B a 2 x 2 grid; prompt files in prompts/, run logs; a self-review of each sheet against the A rules and against the crop (name every difference: proportions, number of visible parts, arms, head box, eye); the Orchestrator sends the sheets to the owner with SendUserFile; nothing goes to vault/assets/ until the owner validates one
- done_when: ref-chaser.png exists and shows only the chaser; at most 4 candidate sheets generated (24 credits at most); each has the four views of the same robot; self-review written per sheet; prompts and logs committed; no image in vault/assets/; no 3D, no Blueprint, no Unreal
- editor_state: closed
- token_ceiling: open
- model: sonnet
- tokens: input 18, cache write 29653, cache read 144575, output 1437
- runs: 1
- milestone: M2
- result: ref-chaser.png and four candidate sheets (24 credits): too detailed and too bulky, top view not a real top view; owner chose a correction pass on A2.

## Relations
- [[task-151-chaser-robot]] — parent global task.
