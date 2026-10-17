---
id: perf-lead-pickup-simulated-roll
description: "Lead: replace the Chaos physics of landed pickups by a roll simulated in the pickup subsystem loop if hundreds of bodies cost too much (task-229)."
parent: perf-leads
kind: note
---
# Lead: simulated pickup roll instead of Chaos

- origin (owner 2026-10-17): pickups now land into real Chaos physics (simple box collisions, floor and walls only) so they roll and tumble instead of resting on a corner ([[task-229-pickup-physics]]). The owner asked to keep a simulated roll as the fallback "si besoin".
- cost to watch: one simulated body per landed pickup until it sleeps; XP bursts can leave hundreds on the floor (stress S7: 1200 cubes from 400 chasers, [[sys-pickups]]). The measured cost of task-229 goes here once known.
- option: after landing, keep the landing velocity in `URRPickupSubsystem` (no physics body), decelerate with a friction constant, turn the mesh by distance / radius about the axis perpendicular to the motion, bounce on blocking grid cells with a 2D cell test, then snap to the nearest face-down orientation when slow (the cube never stays on a corner). Expected: near zero physics cost, one extra step in the existing loop.
- measure first: physics time (stat physics, game-thread time) in the S7 burst with physics on versus off.

## Relations
- [[perf-leads]] — parent index.
- [[sys-pickups-lifecycle]] — the pop, rest and magnet phases it would change.
