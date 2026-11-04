---
id: task-429-02-magnet-arc-doc
description: "Doc: document the arc magnet in sys-pickups and sys-pickups-lifecycle"
parent: task-429-pickup-magnet-arc-path
kind: task
status: in-progress
---
# Pickup magnet flight along an arc: doc

- goal: Update the pickup docs from the cpp change summary: the Magnet paragraph and properties list of sys-pickups-lifecycle (arc instead of straight line, new tunables and defaults, provisional owner 2026-11-04, task-429, unverified on screen), the summary line of sys-pickups; bp-pickup-xp and bp-pickup-ore only if they state the straight line.
- agent: doc
- depends_on: task-429-01-magnet-arc-cpp
- inputs: agent-doc (vault/reference/agents/agent-doc.md), sys-pickups (vault/reference/tech/systems/world/sys-pickups.md), sys-pickups-lifecycle (vault/reference/tech/systems/world/sys-pickups/sys-pickups-lifecycle.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), bp-pickup-ore (vault/reference/tech/blueprints/fx/bp-pickup-ore.md), task-429-pickup-magnet-arc-path (vault/reference/tasks/task-429-pickup-magnet-arc-path.md) (vault/reference/tasks/task-429-pickup-magnet-arc-path.md) (global request, gdd_check and decided Questions)
- expected_output: Doc nodes updated, lint clean, committed by path.
- done_when: sys-pickups-lifecycle describes the arc magnet with every new property and default; sys-pickups summary mentions it; no doc still says the magnet flies a straight line except as the bArcMagnet false behaviour; lint clean.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-04T20:44:06Z
- waiting_for: none
- result: open

## Relations
- [[task-429-pickup-magnet-arc-path]] — parent global task.
