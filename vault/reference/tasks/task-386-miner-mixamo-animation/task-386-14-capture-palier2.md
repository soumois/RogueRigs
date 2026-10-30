---
id: task-386-14-capture-palier2
description: "Capture: -game frames of the miner standing then moving, the forward run must play"
parent: task-386-miner-mixamo-animation
kind: task
status: todo
---
# Miner animated with the owner Mixamo clips: capture of step 2

- goal: Check by eye that the miner switches from the idle to the forward run when it moves.
- agent: capture
- depends_on: task-386-13-abp-palier2-run
- inputs: agent-capture (vault/reference/agents/agent-capture.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), Saved/Captures of the previous capture, reference palette Art/Source/miner-meshy/texture_0.png. No debug command is known to move the miner in a -game run: use what agent-capture offers or synthetic key input to the game window (W key); if nothing moves it, report C2 not seen and it goes to the owner
- expected_output: frames and crops where agent-capture stores them, one line per point with the frame path
- done_when: C1 standing still the miner holds the idle (not T-pose); C2 while moving the legs run (forward run visible, at least two frames with different leg poses); C3 back to still, the idle returns
- editor_state: closed
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
- [[task-386-miner-mixamo-animation]] — parent global task.
