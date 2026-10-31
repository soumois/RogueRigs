---
id: task-400-06-rifle-in-hand-capture
description: "Capture: game capture: the 3D rifle in the right hand, idle and running"
parent: task-400-burst-rifle-in-hand
kind: task
status: wait-answers
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-31T00:16:47Z
- waiting_for: Capture 2026-10-31 failed C2/C3: rifle floats above/behind the head, not in the hand (Saved/Captures/RifleInHand/crops.png); socket WeaponSocket_R needs retuning, decision needed
- result: open

## Relations
- [[task-400-burst-rifle-in-hand]] — parent global task.
