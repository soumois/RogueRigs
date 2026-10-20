---
id: task-257-02-pulse-desync
description: "C++ writes a seeded pulse phase and rate to ore and heal pickups and to the ore and heal deposit crystal instances."
parent: task-257-pulse-desync
kind: task
status: done
---
# Pulse desync values (C++)

- goal: C++ writes a seeded pulse phase and rate to ore and heal pickups and to the ore and heal deposit crystal instances.
- agent: cpp
- depends_on: task-257-01-pulse-desync
- inputs: bp-pickup-ore (vault/reference/tech/blueprints/fx/bp-pickup-ore.md), bp-pickup-heal (vault/reference/tech/blueprints/fx/bp-pickup-heal.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), sys-pickups (vault/reference/tech/systems/world/sys-pickups.md), sys-pickups-lifecycle (vault/reference/tech/systems/world/sys-pickups/sys-pickups-lifecycle.md), sys-grid-deposits (vault/reference/tech/systems/world/sys-grid/sys-grid-deposits.md), sys-grid (vault/reference/tech/systems/world/sys-grid.md), bp-arena-grid (vault/reference/tech/blueprints/world/bp-arena-grid.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md)
- expected_output: C++ in Source/RogueRigs/Pickups and Grid: an editable PulseSeed (default 1) combined with a stable id (spawn counter per pickup life; cell or deposit id for deposit crystal instances) into a deterministic random phase over the full cycle and a rate in 0.8-1.2, written to the data indices documented by 257-01, for Ore and Heal pickups and for ore and heal deposit crystal instances only (XP untouched); docs updated.
- done_when: Build passes; existing RogueRigs.Dig, RogueRigs.Ore, RogueRigs.Integration.FineContactDigOpensPassage and RogueRigs.RunFlow tests pass; ONE headless check (no capture): the same seed gives the same phases, two neighbouring crystals of one deposit and two pickups get different phases, XP pickups keep the neutral values; docs updated; lint clean.
- editor_state: closed (C++ build)
- token_ceiling: 150k
- model: opus
- tokens: input 98, cache write 167875, cache read 5862616, output 35281
- runs: 1
- milestone: M1
- result: Seeded pulse desync: FRRPulseDesync hash (PulseSeed 1, phase 0-1 turns, rate 0.8-1.2) written at pickup life start for Ore and Heal (CPD 1 and 2, reset to neutral at pool release, XP untouched) and for every deposit crystal instance (per-instance data 4 and 5, 6 floats, deposit id); build OK, 27 of 27 tests pass incl. HealVein and the new PulseDesync check; docs and GDD lines updated. Look and runtime read of the material unverified.

## Relations
- [[task-257-pulse-desync]] — parent global task.
