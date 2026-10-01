---
id: level-layout-schema
description: "Layout JSON format (cells in grid coordinates), brief format, file location, and the validator every layout must pass before reaching the engine."
parent: pipelines-index
kind: doc
---
# Level layout schema

## Layout file
- Location: `Content/RogueRigs/Data/Levels/<name>.json`. Read at runtime; the folder must be added to "Additional Non-Asset Directories to Package".
- Format:
```json
{ "version": 1, "name": "arena_m1", "width": 60, "height": 60,
  "cells": [ { "x": 30, "y": 30, "type": "spawn" }, { "x": 4, "y": 7, "type": "rock" } ] }
```
- `x` runs 0 → width−1, `y` runs 0 → height−1. Only non-empty cells are listed; any cell not listed is open floor.
- Types: `rock`, `ore`, `heal_ore`, `special_ore`, `barrel` (blocks), `spawn` (the miner's start, exactly one) and `void` (outside the asteroid: no floor, no block; shapes an irregular asteroid).
- No other field is allowed. Cell size is not in the file ([[arena-grid]]).

## Brief file (optional input to the validator)
```json
{ "width": 60, "height": 60, "rock_coverage": [0.10, 0.25], "counts": { "heal_ore": [6, 12] }, "reachable_floor_min": 0.95 }
```
`rock_coverage` bounds block coverage = blocks / non-void cells; `counts` bounds each type; `reachable_floor_min` (0 to 1) is the minimum reachable-floor share.

## Validator
- `node Tools/validate_layout.js <layout.json> [--brief <brief.json>]`; exit 0 = valid.
- Checks: header, bounds, known types, no duplicates, one spawn, a 5 × 5 area around the spawn with no block and no void, brief bounds.
- Reachable floor: share of floor cells (non-void, non-block) 4-connected to the spawn; always printed, fails only below `reachable_floor_min`.
- Prints an ASCII preview: `#` rock, `o` ore, `+` heal ore, `*` special ore, `B` barrel, `S` spawn, `.` floor, space void.
- A layout that fails never reaches the engine; the error goes back to the Level-Design agent.

## Relations
- [[pipelines-index]] — parent index.
- [[agent-level-design]] — writes layouts.
- [[sys-grid]] — loads layouts.
- [[layout-generator-recipe]] — how the current layout was generated.
