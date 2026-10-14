---
id: task-169-03-weapon-card-lore
description: "Load weapon_texts.json and give new-weapon cards their lore line in LoreText."
parent: task-169-weapon-card-design
kind: task
status: in-progress
---
# New-weapon cards: lore loading in C++

- goal: Load weapon_texts.json and give new-weapon cards their lore line in LoreText.
- agent: cpp
- depends_on: task-169-01-weapon-lore-lines, task-168-04-card-icon-data
- inputs: sys-text-content (vault/reference/tech/systems/ui/sys-text-content.md), sys-upgrades (vault/reference/tech/systems/core/sys-upgrades.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), text-content-schema (vault/reference/tech/pipelines/text-content-schema.md), text-content-files (vault/reference/tech/pipelines/text-content-files.md), text-content-validator (vault/reference/tech/pipelines/text-content-validator.md)
- expected_output: Tools/content/validate_text.js accepts weapon_texts (file name, run_id stem, items id wpn_*, weapon, flavor 3 to 12 words, no digit) and the text-content-schema and text-content-files nodes say so; URRTextContentSubsystem reads weapon_texts.json with the same fallback rules (GetWeaponFlavor); MakeWeaponCard fills LoreText; schema and event id docs and sys-text-content, sys-upgrades updated; existing tests broken by the change fixed, no new tests.
- done_when: validate_text.js exits 0 on weapon_texts.json; build succeeds; existing RogueRigs.Text.* and RogueRigs.Upgrades.* tests pass; docs updated; committed.
- editor_state: closed
- token_ceiling: 60k
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-169-weapon-card-design]] — parent global task.
