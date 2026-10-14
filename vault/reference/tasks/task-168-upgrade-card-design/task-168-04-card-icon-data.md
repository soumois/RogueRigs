---
id: task-168-04-card-icon-data
description: "Give the UI one C++ place that maps a stat and a weapon to their icon textures, and a separate lore text on stat cards."
parent: task-168-upgrade-card-design
kind: task
status: in-progress
---
# Upgrade cards: icon mapping and lore field in C++

- goal: Give the UI one C++ place that maps a stat and a weapon to their icon textures, and a separate lore text on stat cards.
- agent: cpp
- depends_on: task-168-01-card-lore-lines, task-168-02-stat-icons
- inputs: sys-upgrades (vault/reference/tech/systems/core/sys-upgrades.md), sys-text-content (vault/reference/tech/systems/ui/sys-text-content.md), placeholder-weapon-icons (vault/reference/gdd/art-audio/placeholder-weapon-icons.md), wbp-weapons (vault/reference/tech/blueprints/ui/wbp-weapons.md), wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: C++ in Source/RogueRigs/: FRRUpgradeCard gets LoreText (the flavor line; DescriptionText no longer packs stat name + newline + flavor, or keeps it for compatibility, implementer choice documented); a BlueprintPure lookup (stat -> stat icon texture, weapon -> weapon icon texture) backed by data editable without code (soft references, defaults pointing at /Game/RogueRigs/UI/Icons/T_Icon_*); docs sys-upgrades and sys-text-content updated; existing tests whose expectations the change breaks are fixed, no new tests.
- done_when: Build succeeds; existing RogueRigs.Upgrades.* and RogueRigs.Text.* tests pass; lookup returns the weapon icons for the three weapons; docs updated; committed.
- editor_state: closed
- token_ceiling: 60k
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-168-upgrade-card-design]] — parent global task.
