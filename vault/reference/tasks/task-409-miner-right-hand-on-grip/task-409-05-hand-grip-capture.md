---
id: task-409-05-hand-grip-capture
description: "Capture: after: same two close views, before and after side by side for the owner"
parent: task-409-miner-right-hand-on-grip
kind: task
status: todo
---
# Miner right hand on the rifle pistol grip, rifle kept where the owner placed it: capture

- goal: The owner sees the current look and the new one from the same views, side by side and labelled.
- agent: capture
- depends_on: task-409-01-hand-grip-look-before, task-409-03-import-v2-grip, task-409-04-rifle-mesh-offset-reset
- inputs: agent-capture (vault/reference/agents/agent-capture.md), tool-capture (vault/reference/tech/pipelines/tool-capture.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), task-409-01-hand-grip-look-before (vault/reference/tasks/task-409-miner-right-hand-on-grip/task-409-01-hand-grip-look-before.md), Look-before frames: Saved/Captures/HandGrip409/before_side.png, before_front.png (camera commands in task-409-01 result), Commands: as task-409-01; sheets with `Tools/capture/sheet.js`
- expected_output: Frames `Saved/Captures/HandGrip409/after_side.png`, `after_front.png`, sheets `sheet_side.png`, `sheet_front.png` (labels "avant", "après"), report per point with paths.
- done_when: right side close view and three-quarter front close view (as task-408-04), miner standing, weapons stopped, animation frozen, same camera commands as task-409-01, at most 3 launches. Checklist: C1 the right hand closes on the pistol grip (no visible gap); C2 the rifle is at the same place and angle as in the before frame; C3 the left hand stays on the handguard; C4 no arm stretched straight, crossed through the body or twisted; C5 the right elbow looks natural (not through the torso). Each point seen, not seen or ambiguous with a path; ambiguous points and the sheets go to the owner.
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
- [[task-409-miner-right-hand-on-grip]] — parent global task.
