---
id: task-169-05-weapon-card-test-fix
description: "Fix the existing weapon-card integration test broken by the new layout: explanation now in WeaponDesc, lore in CardDesc."
parent: task-169-weapon-card-design
kind: task
status: done
---
# New-weapon cards: fix the broken integration test

- goal: Update the expectations of the existing integration test that the new weapon card layout breaks; no new test.
- agent: cpp
- depends_on: task-169-04-weapon-card-layout
- inputs: wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), sys-upgrades (vault/reference/tech/systems/core/sys-upgrades.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md); file Source/RogueRigs/Tests/RRWeaponsIntegrationTests.cpp (weapon branch around lines 347-399, Integration.WeaponCardEquipsShotgun)
- expected_output: RRWeaponsIntegrationTests.cpp reads the weapon explanation from WeaponDesc<i> and the lore from CardDesc<i> (LoreText) on new-weapon cards, through the UpgradeChoice child widget tree; any other existing test broken by tasks 168-05 / 169-04 fixed the same way.
- done_when: Build succeeds; Integration.WeaponCardEquipsShotgun and the other existing integration tests touching the upgrade cards pass headless; committed.
- editor_state: closed
- token_ceiling: 60k
- model: opus
- tokens: input 30, cache write 51193, cache read 504226, output 2927
- runs: 1
- milestone: M2
- result: RRWeaponsIntegrationTests: weapon card expectations moved to WeaponHeader/WeaponName/WeaponDesc and CardDesc = LoreText when non-empty. Build OK; WeaponCardEquipsShotgun, LevelUpUpgradeLoop, ConsoleExtractionWin, ShuttleTimeoutLoss pass (4/4). Commit 9e401a6.

## Relations
- [[task-169-weapon-card-design]] — parent global task.
