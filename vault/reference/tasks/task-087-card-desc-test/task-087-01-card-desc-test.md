---
id: task-087-01-card-desc-test
description: "The existing test Integration.WeaponCardEquipsShotgun matches the card flavor line now shown on stat cards."
parent: task-087-card-desc-test
kind: task
status: in-progress
---
# Card description test update

- goal: The existing test Integration.WeaponCardEquipsShotgun matches the card flavor line now shown on stat cards.
- agent: cpp
- depends_on: none
- inputs: sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), sys-text-content (vault/reference/tech/systems/sys-text-content.md), sys-level-up (vault/reference/tech/systems/sys-level-up.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: find why Integration.WeaponCardEquipsShotgun fails (it fails with rr.Enemy.LightMovement 0 as well, so it is the card text change of tasks 080 and 084: WBP_HUD now shows the flavor line, the text after the first newline of DescriptionText, in the CardDesc blocks of a stat card). Update only that test expectation to the new intended behaviour: on a stat card the flavor CardDesc block is visible when the loader gives a flavor and collapsed otherwise; new-weapon cards unchanged. If the failure is instead a real defect (for example a block visible with empty text, or a new-weapon card showing a flavor), report it precisely and do not change the Blueprint. No new tests, no gameplay change. Editor closed.
- done_when: build exits 0 with the editor closed; all RogueRigs.* tests pass headless (204/204); sys-automation-tests updated; lint clean; committed.
- editor_state: closed
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-087-card-desc-test]] — parent global task.
