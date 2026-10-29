---
id: task-372-01-cpp-extractor-footprint-clear
description: "Cpp: grid footprint clear with source Extractor at each extractor ground placement, drops at rest, barrels without explosion."
parent: task-372-extractor-clears-rocks
kind: task
status: done
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
- model: opus
- tokens: input 128, cache write 172288, cache read 7028078, output 42738
- usage: turns 62, context start 13k peak 178k, resumes 1 (turn-limit x1), 16 min, units 1.13 M
- runs: 2
- milestone: M2
- started: 2026-10-29T21:42:09Z
- waiting_for: none
- result: Done: ClearExtractorFootprint + terrain source Extractor (de889932, 0e85c9a0); build ok, Dig/Explosions/Extraction 31/31; -game log shows clear lines at level 1 and 2 arrivals and an extraction landing, all footprints open (0 blocks); non-zero counts (5 and 7 rock blocks) seen only in a first test run. Unverified: deposits, pickups, barrels under the extractor in game; a barrel with an already queued explosion still explodes. Cost: agent hit the 60-turn limit and was resumed; counted under first description.

## Relations
- [[task-372-extractor-clears-rocks]] — parent global task.
