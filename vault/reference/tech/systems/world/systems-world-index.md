---
id: systems-world-index
description: "Grid, flow field, pickups, objectives, pools: the terrain and what lives on it."
parent: systems-index
kind: note
---
# C++ systems — world

Grid, flow field, pickups, objectives, pools: the terrain and what lives on it.

## Relations
- [[systems-index]] — parent index.
- [[sys-grid]] — grid data, autotiling, instanced rendering, terrain damage.
- [[sys-flow-field]] — flow field path provider and generic enemy movement component as built.
- [[sys-pickups]] — ore, heal and XP pickups: drop rule, pop, attraction within PickupRadius, collection event, XP grant.
- [[sys-objectives]] — run flow (alert bar, console drop and pause, extraction-ready event) and the console objective with its Blueprint events.
- [[sys-actor-pool]] — generic actor pool subsystem: reuse, creation spread over MaxFrames, IRRPoolable reset hooks, IsPooledIdle filter.
- [[sys-run]] — run state: level index, layout list, miner snapshot carried between levels.
- [[sys-run-log]] — run log collector: per-level counters of terrain, damage, kills, upgrades and XP.
