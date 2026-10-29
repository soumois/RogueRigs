---
id: task-372-01-cpp-extractor-footprint-clear
description: "Cpp: grid footprint clear with source Extractor at each extractor ground placement, drops at rest, barrels without explosion."
parent: task-372-extractor-clears-rocks
kind: task
status: in-progress
---
# Extractor: blocks under its footprint are destroyed, resources stay: cpp

- goal: ARRArenaGrid clears a footprint (source Extractor) before each extractor ground placement, with Q5 drops.
- agent: cpp
- depends_on: task-371-01-cpp-extractor-arrival
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), sys-grid (vault/reference/tech/systems/world/sys-grid.md), sys-grid-api (vault/reference/tech/systems/world/sys-grid/sys-grid-api.md), sys-grid-deposits (vault/reference/tech/systems/world/sys-grid/sys-grid-deposits.md), sys-grid-barrels (vault/reference/tech/systems/world/sys-grid/sys-grid-barrels.md), sys-pickups (vault/reference/tech/systems/world/sys-pickups.md), sys-run-log (vault/reference/tech/systems/world/sys-run-log.md), run-log-schema (vault/reference/tech/pipelines/run-log-schema.md), sys-objectives-shuttle (vault/reference/tech/systems/world/sys-objectives/sys-objectives-shuttle.md), destruction-rules (vault/reference/gdd/terrain/destruction-rules.md), ore-blocks (vault/reference/gdd/terrain/ore-blocks.md)
- expected_output: ERRTerrainDamageSource::Extractor; a grid footprint destroy entry point (whole deposits and heal veins, drops placed at rest on their cells, barrels removed without explosion); called by ARRShuttle placement at arrival and landing before the actor is visible; run log source; log line with counts; one commit; change summary.
- done_when: build succeeds; a -game log of a level 2 arrival and an extraction landing shows the clear line with counts by type; existing Dig, Explosion and Extraction tests pass; committed by path.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-29T21:42:09Z
- waiting_for: none
- result: open

## Relations
- [[task-372-extractor-clears-rocks]] — parent global task.
