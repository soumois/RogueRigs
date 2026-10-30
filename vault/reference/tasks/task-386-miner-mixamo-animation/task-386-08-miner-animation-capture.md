---
id: task-386-08-miner-animation-capture
description: "Capture: -game capture of the animated miner against the checklist C1 to C7"
parent: task-386-miner-mixamo-animation
kind: task
status: in-progress
---
# Miner animated with the owner Mixamo clips: capture

- goal: Check in a -game run that the miner shows the new mesh and its clips, against the checklist.
- agent: capture
- depends_on: task-386-07-bp-miner-mesh-swap
- inputs: agent-capture (vault/reference/agents/agent-capture.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), miner (vault/reference/gdd/player/miner.md), vault/reference/tech/pipelines/miner-mixamo-clips.md (created by step 01), owner words, planner measures and Q answers: global `request`, `gdd_check` and `## Questions` (vault/reference/tasks/task-386-miner-mixamo-animation.md), reference look: miner palette white, teal, rust plates, amber visor (`Art/Source/miner-meshy/texture_0.png`); look-before frames: none on file (BP_Miner showed Quinn); debug commands: those of agent-capture (`rr.Debug.Shots`, `rr.Debug.Delayed`); no command known to drive the miner movement in a -game run: if none exists, report C4 to C7 as not seen (no new debug command is written for this task) and they go to the owner.
- expected_output: frames and crops where agent-capture stores them, one line per point: seen, not seen or ambiguous, with the frame path.
- done_when: report gives each point with its frame: C1 the miner is the Meshy mesh (white, teal and rust plates, amber visor), not the grey Quinn mannequin; C2 the feet touch the floor (no float, no sinking); C3 standing still the miner holds the rifle-aiming idle pose, not an A-pose or T-pose; C4 moving toward the aim plays a forward run; C5 moving away from the aim plays a backward run; C6 moving sideways to the aim plays a strafe while the body keeps facing the aim; C7 no visible foot sliding at full speed. Ambiguous points are listed for the owner.
- editor_state: closed
- token_ceiling: 0.4 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-30T05:54:55Z
- waiting_for: none
- result: open

## Relations
- [[task-386-miner-mixamo-animation]] — parent global task.
