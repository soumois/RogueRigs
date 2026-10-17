---
id: task-230-04-rock-top-texture-review
description: "Review of the rock top texture candidates."
parent: task-230-rock-slab-integration
kind: task
status: done
---
# Review of the rock top texture candidates

- goal: Review of the rock top texture candidates.
- agent: style-guide
- depends_on: task-230-03-rock-top-texture
- inputs: the files listed in the task-230-03 result; vault/reference/gdd/art-audio/art-style-guide.md, vault/reference/gdd/art-audio/color-code.md, vault/reference/gdd/art-audio/readability-rules.md
- expected_output: `Art/Source/rock-top-texture/review-230-04.md`.
- done_when: every fail shown to the owner with its rule; his choice recorded.
- editor_state: any
- token_ceiling: 30000
- model: opus
- tokens: input 8, cache write 23769, cache read 40490, output 3749
- runs: 1
- milestone: M2
- result: review bdfc52d2: T_RockTop fails colour code only (saturated lavender, close to Duskite violet); owner 2026-10-17: fix in the material (09): desaturate about 50-60 %, tint as parameter; rock vs floor brightness and faint repetition to check in game (12)

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
