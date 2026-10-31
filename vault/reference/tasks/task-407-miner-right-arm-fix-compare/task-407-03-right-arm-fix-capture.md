---
id: task-407-03-right-arm-fix-capture
description: "Capture: capture: one frozen frame, five offset pairs from two close views, labelled comparison sheets for the owner"
parent: task-407-miner-right-arm-fix-compare
kind: task
status: cancelled
---
# Miner right arm correction on the idle pose, compared on one frame: capture

- goal: The owner gets images of one same frame with the offset pairs of Q7, side by side and labelled with their values.
- agent: capture
- depends_on: task-407-02-abp-right-arm-fix
- inputs: agent-capture (vault/reference/agents/agent-capture.md), tool-capture (vault/reference/tech/pipelines/tool-capture.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), Commands: `rr.Stress.Peace 1`, `rr.Debug.Camera <Pitch> <ArmLength> [Yaw]` (close view of the right arm; the default top-down view hides the right hand, task-401-04), `rr.Debug.MinerAnimFreeze 1`, `rr.Debug.MinerArmFix "<elbowUp> <handForward>"`, `rr.Debug.Shots`, `rr.Debug.Delayed`; sheets with `Tools/capture/sheet.js` (labels = values in cm)
- expected_output: Frames and sheets in `Saved/Captures/RightArmFix407/` (sheet per view, labels "coude +E / main +H cm"), report per point with paths, launches used.
- done_when: Miner standing, weapons stopped, anim frozen once, then pairs (0,0), (3,2), (6,4), (9,6), (12,8) applied in turn and shot from the right side view and the three-quarter front view (10 frames, at most 5 launches), one labelled sheet per view. Checklist: C1 every frame of a view shows the same frame (legs, head, left arm, background identical apart from the right arm, rifle and left hand); C2 the right elbow is higher as elbowUp grows; C3 the right hand and the rifle are further forward as handForward grows; C4 the left hand stays on the rifle handguard; C5 no arm stretched straight, crossed through the body or in T-pose. Each point seen, not seen or ambiguous with a path; ambiguous points and the sheets go to the owner.
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
- [[task-407-miner-right-arm-fix-compare]] — parent global task.
