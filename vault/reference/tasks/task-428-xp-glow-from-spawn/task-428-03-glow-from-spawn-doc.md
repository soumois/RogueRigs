---
id: task-428-03-glow-from-spawn-doc
description: "Doc: document bGlowDuringPop in sys-pickups and bp-pickup-xp"
parent: task-428-xp-glow-from-spawn
kind: task
status: todo
---
# XP pickup glow disc on from spawn: doc

- goal: Document bGlowDuringPop (default, effect, how to revert) where the glow fade is described: sys-pickups / sys-pickups-lifecycle and the bp-pickup-xp glow_disc_fade entry (add a dated line, task-428, provisional, owner 2026-11-04).
- agent: doc
- depends_on: task-428-01-glow-from-spawn-cpp
- inputs: agent-doc (vault/reference/agents/agent-doc.md), sys-pickups (vault/reference/tech/systems/world/sys-pickups.md), sys-pickups-lifecycle (vault/reference/tech/systems/world/sys-pickups/sys-pickups-lifecycle.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), task-428-xp-glow-from-spawn (vault/reference/tasks/task-428-xp-glow-from-spawn.md) (request, gdd_check, decided Questions)
- expected_output: Updated doc nodes, lint clean, committed by path.
- done_when: sys-pickups (or sys-pickups-lifecycle, wherever the fade is described) and bp-pickup-xp state that the XP glow disc shows from spawn when bGlowDuringPop, its default and the revert; no fact duplicated; lint clean; committed.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-428-xp-glow-from-spawn]] — parent global task.
