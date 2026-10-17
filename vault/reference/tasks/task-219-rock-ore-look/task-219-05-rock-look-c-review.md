---
id: task-219-05-rock-look-c-review
description: "Style-guide review of the 2 corrected image C boards against A rules, colour code, readability and the correction list."
parent: task-219-rock-ore-look
kind: task
status: done
---
# Rock and ore look: review of the correction

- goal: verdict and correction per image of task-219-04.
- agent: style-guide
- depends_on: task-219-04-rock-look-c-fix
- inputs: the images listed in the task-219-04 result; vault/reference/tasks/task-219-rock-ore-look/task-219-04-rock-look-c-fix.md (corrections); vault/reference/gdd/art-audio/art-style-guide.md, color-code.md, readability-rules.md.
- expected_output: `Art/Source/rock-look/review-219-05.md`.
- done_when: every image has a verdict; every fail shown to the owner with its rule; his choice recorded here.
- editor_state: any
- token_ceiling: 30000
- model: opus
- tokens: input 8, cache write 28316, cache read 42827, output 8432
- runs: 1
- milestone: M2
- result: review 3c4e48e1: image 2 better base; fails A12 camera 50-55 deg, A1 grain, A14 stub, K3 cell-like cracks, K4 hairline cracks, colour code intact without cracks; owner 2026-10-17 keeps every deviation (exploration) and wants intact rock smooth: color-code and readability-rules updated (cracks = damaged)

## Relations
- [[task-219-rock-ore-look]] — parent global task.
