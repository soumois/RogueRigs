---
id: task-229-pickup-physics
description: "Pickups land into Chaos physics (simple box collisions, floor and walls only) so they roll and tumble; a very light spin while they fly."
parent: tasks-done-index
kind: task
status: done
---
# Pickups: real physics on landing

- request: "Concernant les collectables, il ne faut pas qu'ils se posent simplement sur le sol, mais qu'ils roulent en physique. Et lorsqu'ils sont projetés, il faut une très légère rotation." then "Vraie physique Chaos, il faudra simplifier leur collision (rectangle pour les écrous, et cube pour le cube, vois pour le minerai ce qui est le mieux). On reviendra à un roulis simulé si besoin (marque-le dans les optimisations possibles). Car pour l'instant, on arrive à des cubes qui tiennent sur leur pointe (ce qui est physiquement très peu probable). Collision uniquement sur mur et sol." (owner 2026-10-17)
- goal: every pickup (XP nut, big XP cube, ore chunk, heal) flies its pop with a very light spin, then on landing becomes a Chaos rigid body that rolls, tumbles and settles on a face; it collides only with the floor and the walls (rock cells); the magnet and collection are unchanged.
- acceptance: simple collisions on the pickup meshes; C++ physics hand-off; no cube resting on a corner in a capture; physics cost measured in the S7 burst and written into [[perf-lead-pickup-simulated-roll]].
- gdd_check: [[sys-pickups-lifecycle]] (pop arc, floaty pop, rest orientation, magnet, pool), [[sys-pickups]], [[bp-pickup-xp]]. Defaults (Orchestrator): the pop arc stays kinematic (its floaty tuning is kept) and adds a light spin; at landing the pickup switches to physics with the arc's landing velocity plus the spin; on magnet latch physics stops and the kinematic fly-in takes over; the pool turns physics off on release; RestOrientation no longer forces the landed orientation (physics decides); collision boxes exclude the glow disc slot; ore chunk collision chosen by the agent (box per variant expected).
- confirmed_by_owner: yes
- milestone: M1
- next_step: none
- result: Done. Commit 57311527: vault/assets/pickups-physics-flight.png (a cube changing tilt between shots: light spin visible, random axis per pickup) and pickups-physics-rest.png (cubes on a face, none clearly on a corner; nuts flat or on one of their six flats; glow discs not cut). Roll after landing small (5-25 px, under one cube width), a lower bound: each screenshot froze the game 0.27 s and physics is capped at 1/30 s per frame, so physics ran about 8x slower during the shots. Knob if more roll is wanted: LandingHorizontalSpeedScale (landing horizontal speed about 70 uu/s). No error in the log.

## Subtasks
- [[task-229-01-pickup-simple-collisions]] — cpp (headless): simple collisions on the pickup meshes.
- [[task-229-02-pickup-physics-cpp]] — cpp: collision profile, landing hand-off to physics, flight spin, magnet and pool, build, S7 cost.
- [[task-229-03-pickup-physics-capture]] — cpp: one `-game` run, capture of landed pickups.

## Relations
- [[tasks-done-index]] — parent index.
- [[sys-pickups-lifecycle]] — the lifecycle changed.
- [[perf-lead-pickup-simulated-roll]] — the fallback if physics costs too much.
