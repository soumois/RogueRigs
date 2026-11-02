---
id: task-420-03-gdd-cite-csv
description: "Doc: gdd nodes cite the CSV row instead of copying its numbers"
parent: task-420-single-source-game-values
kind: task
status: done
---
# Game values in one place: the CSV tables: doc

- goal: Descend the gdd branches below and, in every node that restates a value held in a CSV, replace the number with the citation `<file>.csv` row `<Name>` (column names when only some columns apply), keeping status, owner and decision date; values not in a CSV stay. Lint clean, commit by path.
- agent: doc
- depends_on: none
- inputs: task-420-single-source-game-values (vault/reference/tasks/task-420-single-source-game-values.md), agent-doc (vault/reference/agents/agent-doc.md), _authoring (vault/_authoring.md), weapons-index (vault/reference/gdd/weapons/weapons-index.md), enemies-index (vault/reference/gdd/enemies/enemies-index.md), player-index (vault/reference/gdd/player/player-index.md), terrain-index (vault/reference/gdd/terrain/terrain-index.md), run-index (vault/reference/gdd/run/run-index.md), `Content/RogueRigs/Data/Tables/` (WeaponStats, EnemyStats, PlayerBaseStats, ExplosionStats, UpgradeValues, UpgradeRarities, XPCurve, OreTypes .csv)
- expected_output: edited gdd nodes; one commit; list of nodes changed.
- done_when: no gdd node holds a number equal to a CSV cell it describes; each such stat cites file and row; `node vault/scripts/lint.js --write` clean.
- editor_state: any
- token_ceiling: 0.5 M units
- model: sonnet
- tokens: input 36, cache write 82051, cache read 829411, output 20659
- usage: turns 18, context start 10k peak 82k, resumes 0, 4 min, units 0.29 M
- runs: 1
- milestone: M3
- started: 2026-11-02T19:54:47Z
- waiting_for: none
- result: Done: 19 gdd nodes (weapons, enemies, player, terrain, run) cite CSV file and row instead of numbers; derived figures dropped; non-CSV values kept. Commit 0fc339f6, lint OK.

## Relations
- [[task-420-single-source-game-values]] — parent global task.
