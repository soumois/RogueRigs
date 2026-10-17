---
id: task-219-02-rock-ore-review
description: "Style-guide review of the rock and ore look images against the A rules, colour code and readability rules."
parent: task-219-rock-ore-look
kind: task
status: done
---
# Rock and ore look: review

- goal: a verdict and a correction per image of task-219-01, against A1–A18, [[color-code]] and [[readability-rules]].
- agent: style-guide
- depends_on: task-219-01-rock-ore-look
- inputs: the images, prompts and node listed in the task-219-01 result; vault/reference/gdd/art-audio/art-style-guide.md, vault/reference/gdd/art-audio/color-code.md, vault/reference/gdd/art-audio/readability-rules.md, vault/reference/gdd/art-audio/rock-look-reference.md.
- expected_output: review file `Art/Source/rock-look/review-219-02.md` (per image: pass or fail per rule, correction).
- done_when: every image has a verdict; every fail is shown to the owner with its rule; his choice (correct or keep) is recorded in this result.
- editor_state: any
- token_ceiling: 30000
- model: opus
- tokens: input 8, cache write 31973, cache read 47541, output 8693
- runs: 1
- milestone: M2
- result: review d6ec4825: 10 rule fails (A1 and A12 on all four, colour code on A-1) and 5 brief fails; owner 2026-10-17: A-1 and A-2 rejected (not organic enough), image C (20261016-231332-rock-look-1) kept as the direction and to be corrected (task-219-04); note 03 refocused on the dual grid

## Relations
- [[task-219-rock-ore-look]] — parent global task.
