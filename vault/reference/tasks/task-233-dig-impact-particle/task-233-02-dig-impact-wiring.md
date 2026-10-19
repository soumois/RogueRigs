---
id: task-233-02-dig-impact-wiring
description: "The drone spawns NS_DigImpact at the laser impact point while it digs, chip colour from the dug block type."
parent: task-233-dig-impact-particle
kind: task
status: todo
---
# Dig impact particle wiring

- goal: while the drone cuts, NS_DigImpact plays at the beam end on the wall face (one beam since task-232), chip colour = colour of the dug block (rock lavender, Duskite violet, heal red per color-code), stops when the dig stops; -game capture sent to the owner.
- agent: cpp
- depends_on: task-233-01-dig-impact-particle, task-232-01-drone-single-beam
- inputs: vault/reference/gdd/player/drone.md, vault/reference/tech/systems/combat/sys-drone/sys-drone-dig.md, vault/reference/tech/blueprints/player/bp-drone.md, the NS_DigImpact doc node written by 233-01, vault/reference/agents/agent-cpp.md
- expected_output: ARRDrone UPROPERTY for the system, a pooled Niagara component placed at the impact point, user colour parameter set from the block type; docs updated; capture.
- done_when: build exits 0 editor closed; RogueRigs.* tests pass except the 2 known pre-existing failures; capture sent to the owner; committed.
- editor_state: closed
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-233-dig-impact-particle]] — parent global task.
