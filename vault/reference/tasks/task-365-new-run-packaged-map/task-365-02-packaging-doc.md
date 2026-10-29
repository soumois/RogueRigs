---
id: task-365-02-packaging-doc
description: "Update build-toolchain and sys-main-menu with the MapsToCook rule, the two-map packaging command and the packaged New Run check."
parent: task-365-new-run-packaged-map
kind: task
status: done
---
# Document the cook rule and the packaged New Run check

- goal: Update build-toolchain: MapsToCook in DefaultGame.ini lists L_MainMenu and L_ArenaM1 (a map reached only by a string path is not cooked otherwise), the staged command uses both maps and starts without a map argument. Update sys-main-menu: data (packaged builds need both maps cooked), gotchas (verified packaged New Run, with the log lines from the cpp change summary; real mouse click still owner-only unless he reported it).
- agent: doc
- depends_on: task-365-01-cook-arena-map
- inputs: build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), sys-main-menu (vault/reference/tech/systems/ui/sys-main-menu.md)
- expected_output: build-toolchain and sys-main-menu updated, lint clean, committed by path.
- done_when: build-toolchain shows the MapsToCook rule and the two-map BuildCookRun command; sys-main-menu gotchas record the packaged New Run verification; node vault/scripts/lint.js --write clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: sonnet
- tokens: input 12, cache write 20477, cache read 103219, output 3397
- usage: turns 6, context start 11k peak 24k, resumes 0, 1 min, units 0.05 M
- runs: 1
- milestone: M2
- started: 2026-10-29T18:34:05Z
- waiting_for: none
- result: build-toolchain and sys-main-menu updated (commit 78d3ff51): MapsToCook rule, two-map command, packaged New Run verification; lint OK.

## Relations
- [[task-365-new-run-packaged-map]] — parent global task.
