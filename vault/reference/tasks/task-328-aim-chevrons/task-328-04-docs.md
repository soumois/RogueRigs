---
id: task-328-04-docs
description: "Document the aim indicator: new sys-aim-indicator node with the material contract, sys-player and systems-combat-index updated."
parent: task-328-aim-chevrons
kind: task
status: done
---
# Aim chevrons: documentation

- goal: the aim indicator component and its material are documented so nobody opens the code or the asset to know them.
- agent: doc
- depends_on: task-328-02-chevron-material, task-328-03-aim-indicator-component
- inputs: create `vault/reference/tech/systems/combat/sys-aim-indicator.md` (template `vault/templates/system.md`, parent systems-combat-index; component API, M_AimChevrons parameter contract and asset settings, hiding rules, stop-at-rock query, implements aim-indicator); update systems-combat-index (vault/reference/tech/systems/combat/systems-combat-index.md, one Children line), sys-player (vault/reference/tech/systems/core/sys-player.md, the new default subobject and its Relations link), aim-indicator (vault/reference/gdd/art-audio/aim-indicator.md, Relations link to sys-aim-indicator); decided values from the global `request` and `gdd_check`; change summaries and commit hashes of task-328-02 and task-328-03.
- expected_output: the nodes above, lint clean.
- done_when: sys-aim-indicator exists and is linked both ends; sys-player and systems-combat-index list it; lint clean; committed by explicit path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: sonnet
- tokens: input 16, cache write 37319, cache read 187504, output 5550
- usage: turns 8, context start 10k peak 37k, resumes 0, 1 min, units 0.09 M
- runs: 1
- milestone: M3
- started: 2026-10-27T15:54:40Z
- waiting_for: none
- result: sys-aim-indicator created; sys-player, systems-combat-index, aim-indicator, build-toolchain updated (commit 223ab90a)

## Relations
- [[task-328-aim-chevrons]] — parent global task.
