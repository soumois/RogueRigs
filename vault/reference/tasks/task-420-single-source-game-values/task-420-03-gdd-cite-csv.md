---
id: task-420-03-gdd-cite-csv
description: "Doc: gdd nodes cite the CSV row instead of copying its numbers"
parent: task-420-single-source-game-values
kind: task
status: in-progress
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: 2026-11-02T19:54:47Z
- waiting_for: none
- result: open

## Relations
- [[task-420-single-source-game-values]] — parent global task.
