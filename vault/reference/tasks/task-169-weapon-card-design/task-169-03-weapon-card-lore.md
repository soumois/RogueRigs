---
id: task-169-03-weapon-card-lore
description: "Load weapon_texts.json and give new-weapon cards their lore line in LoreText."
parent: task-169-weapon-card-design
kind: task
status: todo
---
# New-weapon cards: lore loading in C++

- goal: Load weapon_texts.json and give new-weapon cards their lore line in LoreText.
- agent: cpp
- depends_on: task-169-01-weapon-lore-lines, task-168-04-card-icon-data
- inputs: sys-text-content (vault/reference/tech/systems/ui/sys-text-content.md), sys-upgrades (vault/reference/tech/systems/core/sys-upgrades.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: URRTextContentSubsystem reads weapon_texts.json with the same fallback rules (GetWeaponFlavor); MakeWeaponCard fills LoreText; schema and event id docs and sys-text-content, sys-upgrades updated; existing tests broken by the change fixed, no new tests.
- done_when: Build succeeds; existing RogueRigs.Text.* and RogueRigs.Upgrades.* tests pass; docs updated; committed.
- editor_state: closed
- token_ceiling: 60k
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-169-weapon-card-design]] — parent global task.
