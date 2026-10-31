---
id: task-409-06-doc-hand-grip
description: "Doc: doc: bp-miner (V2Grip, upper body source) and bp-burst-rifle (RifleMesh and Muzzle)"
parent: task-409-miner-right-hand-on-grip
kind: task
status: todo
---
# Miner right hand on the rifle pistol grip, rifle kept where the owner placed it: doc

- goal: bp-miner and bp-burst-rifle describe the new animation, the active upper body source, the reset RifleMesh offset and Muzzle, and why the rifle keeps its place.
- agent: doc
- depends_on: task-409-03-import-v2-grip, task-409-04-rifle-mesh-offset-reset
- inputs: agent-doc (vault/reference/agents/agent-doc.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), bp-burst-rifle (vault/reference/tech/blueprints/player/bp-burst-rifle.md)
- expected_output: Nodes updated, values left open, lint result, commit hash.
- done_when: bp-miner lists A_Miner_UpperRifle_V2Grip (from V2, right hand on the grip, right elbow kept at its V2 height as far as the arm allows, Q2) as UpperBodySequence, V1..V3 kept, how to switch back (UpperBodySequence, or rr.Debug.MinerUpperPose with the RifleMesh offset caveat), no capture (the owner checks himself, Q3); bp-burst-rifle lists RifleMesh (0, 0, 0) scale 1.25 and Muzzle (52.25, 0, 0) with the task-404 values as history; unverified items stated; lint clean; committed by path.
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
- [[task-409-miner-right-hand-on-grip]] — parent global task.
