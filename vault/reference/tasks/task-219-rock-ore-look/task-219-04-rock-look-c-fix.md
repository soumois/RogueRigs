---
id: task-219-04-rock-look-c-fix
description: "Art correction of image C (slab track): steep camera, flat facets, seamless irregular cracks, tilted plates, 2 x 2 ore as four clusters."
parent: task-219-rock-ore-look
kind: task
status: in-progress
---
# Rock and ore look: correction of image C

- goal: 2 images correcting `Art/Source/rock-look/20261016-231332-rock-look-1.png` (image C, kept by the owner as the direction; A and B rejected, "pas assez organique").
- agent: art
- depends_on: task-219-02-rock-ore-review
- inputs: image C and its prompt `Art/Source/rock-look/prompts/02-track-b-slab.txt`; review `Art/Source/rock-look/review-219-02.md`; vault/reference/gdd/art-audio/art-prompts-rock.md, vault/reference/gdd/art-audio/art-style-guide.md, vault/reference/gdd/art-audio/color-code.md, vault/reference/gdd/art-audio/readability-rules.md, vault/reference/agents/art-3d-lessons.md, vault/reference/tech/pipelines/kie-image-workflow.md.
- corrections: camera 75–80 degrees from horizontal (A12); flat colour per facet, no gradient or grain on the top (A1); cracks irregular and crossing zones without seams, light cracks thick enough to read at game distance; heavy damage = plates tilted up; each deposit four crystal clusters in a 2 x 2 square, no orange in the heal vein; corridors 3 cells wide for scale, so the outline is more cut up; keep C's rounded slab, lighter rim and faceted sides.
- expected_output: 2 PNG, prompt and run log in `Art/Source/rock-look/`, `art-prompts-rock` updated.
- done_when: 2 images exist and are sent to the owner; self-review against the corrections done.
- editor_state: any
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-219-rock-ore-look]] — parent global task.
