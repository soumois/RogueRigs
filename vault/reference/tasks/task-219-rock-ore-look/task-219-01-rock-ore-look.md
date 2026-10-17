---
id: task-219-01-rock-ore-look
description: "Image proposals for the look of rock blocks in their damage states and of unmined ore blocks, seen from the game camera"
parent: task-219-rock-ore-look
kind: task
status: in-progress
---
# Rock and ore look: proposals

- goal: Image proposals for the look of rock blocks in their damage states and of unmined ore blocks, seen from the game camera
- agent: art
- depends_on: none
- inputs: rock-look-reference (vault/reference/gdd/art-audio/rock-look-reference.md), color-code (vault/reference/gdd/art-audio/color-code.md), visual-style (vault/reference/gdd/art-audio/visual-style.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), art-prompts-arena-final (vault/reference/gdd/art-audio/art-prompts-arena-final.md), ore-blocks (vault/reference/gdd/terrain/ore-blocks.md), heal-ore (vault/reference/gdd/terrain/heal-ore.md), arena-grid (vault/reference/gdd/terrain/arena-grid.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md)
- expected_output: 4 images max (2 tracks x 2 candidates) in Art/Source/rock-look/, prompts in Art/Source/rock-look/prompts/, run logs, a prompt node art-prompts-rock child of art-prompts
- done_when: each image shows, from the steep game camera, plain rock intact and in two damage states (light cracks, heavy fracture into tilted plates), a Duskite deposit and a heal ore vein; track A per-cell blocks, track B continuous slab with rounded outline; self-review against the A rules done; images sent to the owner
- editor_state: any
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-219-rock-ore-look]] — parent global task.
