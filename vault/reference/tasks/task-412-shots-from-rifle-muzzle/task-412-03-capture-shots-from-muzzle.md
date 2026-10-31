---
id: task-412-03-capture-shots-from-muzzle
description: "Capture: -game capture: bullets at the barrel tip; miner against a rock, no bullet beyond; close robot hit"
parent: task-412-shots-from-rifle-muzzle
kind: task
status: todo
---
# Shots leave from the rifle muzzle: capture

- goal: In a -game run, check the checklist in done_when on frames from the gameplay camera and report each point seen, not seen or ambiguous with the frame or crop path.
- agent: capture
- depends_on: task-412-02-bp-projectiles-recompile
- inputs: task-412-shots-from-rifle-muzzle (vault/reference/tasks/task-412-shots-from-rifle-muzzle.md), agent-capture (vault/reference/agents/agent-capture.md), bp-burst-rifle (vault/reference/tech/blueprints/player/bp-burst-rifle.md), debug command rr.Debug.Shots (frames); miner placed against a rock block aiming into it for C2; a chaser next to the miner for C3
- expected_output: Frames and crops under Saved/Captures/ShotsFromMuzzle412/, a report C1 to C3 seen / not seen / ambiguous with paths.
- done_when: C1: rifle bullets appear at the barrel tip of the held rifle seen from the camera, on at least two aim directions. C2: with the miner against a rock block and aiming into it, no bullet appears beyond the rock. C3: a robot touching the miner is hit (hit flash or damage number). Each point reported seen, not seen or ambiguous with its frame path; ambiguous points go to the owner.
- editor_state: closed (-game run)
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
- [[task-412-shots-from-rifle-muzzle]] — parent global task.
