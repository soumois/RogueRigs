---
id: task-428-02-glow-from-spawn-capture
description: "Capture subtask waived by the owner (Q5): no run, closed on his word; lists the unseen in-game glow checks C1 to C4."
parent: task-428-xp-glow-from-spawn
kind: task
status: todo
---
# XP pickup glow disc on from spawn: capture

- goal: NO RUN. Owner 2026-11-04 (Q5, verbatim): "sauf le point 5, je vérifie par moi même". The Executor launches no capture agent; it closes this subtask on the owner word (runs 0) after 01 passes, and lists C1 to C4 below as unseen items for the owner to check in game.
- agent: capture
- depends_on: task-428-01-glow-from-spawn-cpp
- inputs: agent-capture (vault/reference/agents/agent-capture.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), task-428-xp-glow-from-spawn (vault/reference/tasks/task-428-xp-glow-from-spawn.md) (acceptance A1 to A4), debug commands: open (rr.Debug.Shots per agent-capture)
- expected_output: none (no capture run); the Executor report lists C1 to C4 as unseen for the owner.
- done_when: Closed on the owner word (Q5), no frames. Unseen items handed to the owner: C1 an XP nut in mid-air during its pop arc shows its cyan glow disc. C2 a resting XP nut shows its glow disc. C3 a nut flying to the miner has no glow disc (Q1 default) or keeps it (Q1 other choice). C4 a big XP cube in the air shows its magenta glow disc (if Q3 keeps it; report not seen if no big cube dropped). Ambiguous points go to the owner.
- editor_state: any (no run)
- token_ceiling: 0.4 M units
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
