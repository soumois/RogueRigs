---
id: task-498-01-capture-before
description: "Capture: A -game capture of several heavy slams to see whether a dust ring and rock chips already appear."
parent: task-498-heavy-slam-impact-fx
kind: task
status: wait-answers
---
# Heavy slam impact: dust ring and rock chips: capture

- goal: A -game capture of several heavy slams to see whether a dust ring and rock chips already appear.
- agent: capture
- depends_on: none
- inputs: agent-capture (vault/reference/agents/agent-capture.md), enemy-heavy (vault/reference/gdd/enemies/enemy-heavy.md)
- expected_output: see done_when
- done_when: C1 Dust ring on the ground at the slam frame: seen, not seen or ambiguous, with its colour and size against the telegraph circle. C2 Rock chips at the slam frame: seen, not seen or ambiguous. Frames sent to the owner. The Executor writes in this node which of the next subtasks run.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: open
- usage: open
- runs: 1
- milestone: M2
- started: 2026-11-10T20:39:35Z
- waiting_for: Owner: the slam already shows a beige dust ring at about the slam radius and small pale chips (NS_HeavySlamImpact, task-300, kept by you 2026-10-23). Replace them with the orange cartoon ring and violet chips (02-05 run, rework of NS_HeavySlamImpact), or keep them (task ends)?
- result: Capture before (1 launch, Saved/Captures/task498_before, heavy spawned at 200 uu, peace mode, 30 shots at 0.08 s, 2 slams covered). C1 SEEN, other style: a soft beige-whitish dust ring (not orange, not cartoon: blurred sprite dots), expanding from the feet on slam_16-17 and reaching about the telegraph circle radius, slightly beyond, on slam_18, gone by slam_19; the existing NS_HeavySlamImpact of task-300 (bp-heavy-slam-impact, owner 2026-10-23 had kept it as is). C2 SEEN, other style: a few small pale whitish square chips at the heavy feet on slam_18-19 (not the rock and floor violets). Telegraph circle gone on the ring frames, heavy readable. Per Q2 (owner default: existing ring or chips in another style = stop and show the images before touching anything) nothing is built; subtasks 02-05 wait for the owner. Frames: Saved/Captures/task498_before/before_ring_vs_telegraph.png, before_slam_crop.png, contact_sheet.png.

## Relations
- [[task-498-heavy-slam-impact-fx]] — parent global task.
