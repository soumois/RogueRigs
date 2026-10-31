---
id: task-400-06-rifle-in-hand-capture
description: "Capture: game capture: the 3D rifle in the right hand, idle and running"
parent: task-400-burst-rifle-in-hand
kind: task
status: done
---
# Burst rifle 3D mesh in the miner right hand: capture

- goal: A -game capture on the first arena checks the rifle in the hand against C1 to C5.
- agent: capture
- depends_on: task-400-04-bp-burst-rifle-mesh, task-400-05-shotgun-cube-removal
- inputs: agent-capture (vault/reference/agents/agent-capture.md), bp-burst-rifle (vault/reference/tech/blueprints/player/bp-burst-rifle.md), burst-rifle-mesh (vault/reference/gdd/art-audio/burst-rifle-mesh.md), Look before: the grey cube beside the miner (any earlier frame of the miner in `Saved/`); debug commands `rr.Debug.Shots`, `rr.Debug.MinerAnimForce "x y"`
- expected_output: Report C1..C5 seen, not seen or ambiguous with frame or crop paths; frames sent to the owner.
- done_when: C1 the 3D rifle (white, teal, rust, dark barrel) is visible, no grey cube; C2 standing still, the rifle is in the right hand, muzzle toward the aim; C3 running forward and sideways (MinerAnimForce), the rifle stays in the hand; C4 shots still leave toward the aim; C5 with the shotgun equipped (only through an existing debug command or cheat, otherwise reported not seen), no grey cube beside the miner and its pellets leave from the same place as the rifle bullets. Each point reported; ambiguous points go to the owner.
- editor_state: closed
- token_ceiling: 0.4 M units
- model: sonnet
- tokens: input 38, cache write 70223, cache read 457389, output 8710
- usage: turns 19, context start 14k peak 42k, resumes 0, 7 min, units 0.18 M
- runs: 2
- milestone: M2
- started: 2026-10-31T02:25:43Z
- waiting_for: none
- result: Run 1 (RifleInHand) read the shouldered rifle as not held; PIE analysis with running ABP shows socket correct (RightHand, rifle at hand, barrel along aim, scale 1; Mixamo stance puts right hand at cheek, black visible hand is the LEFT, 14 cm off handguard = task-401). Run 2 (RifleInHand2, 1920x1080, camera arm 350): C2 seen, C3 seen, C1 ambiguous (rifle renders flat cream-white, teal/rust/dark not readable: possible material or lighting defect, owner to judge), C4 ambiguous (no clear tracer), C5 not seen (no command equips shotgun). No grey cube seen.

## Relations
- [[task-400-burst-rifle-in-hand]] — parent global task.
