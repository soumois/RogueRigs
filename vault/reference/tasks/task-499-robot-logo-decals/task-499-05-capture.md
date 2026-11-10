---
id: task-499-05-capture
description: "Capture: A -game capture of the three robots with the emblem, beside frames before."
parent: task-499-robot-logo-decals
kind: task
status: done
---
# Yellow Kessler-Voss emblem decal on Heavy, Kamikaze, Chaser v2: capture

- goal: A -game capture of the three robots with the emblem, beside frames before.
- agent: capture
- depends_on: task-499-04-chaser-v2-decal
- inputs: agent-capture (vault/reference/agents/agent-capture.md)
- expected_output: see done_when
- done_when: C1 Heavy emblem seen from the game camera. C2 Kamikaze emblem seen, core still orange and pulsing. C3 Chaser v2 emblem seen on the yellow body. C4 Shield chaser v2 emblem seen. C5 Emblem stays on a running robot (two frames). C6 Before and after sheet sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus (executor run)
- tokens: not measured
- usage: not measured
- runs: 1
- milestone: M2
- started: 2026-11-10T21:17:31Z
- waiting_for: none
- result: No commit (captures only, Saved/ is not tracked). Same -game setup as the before run (L_ArenaM1 default map, rr.Stress.Peace 1, spawner off, heavy 500 uu yaw 0, kamikaze 700 yaw 90, chaser v2 500 yaw 180, shield chaser v2 500 yaw 270): before Saved/Captures/task-499-before/b_00-02.png, after Saved/Captures/task-499-after/a_00-02.png; extra runs at a closer camera (rr.Debug.Camera -80 1100): task-499-check, task-499-debris. C1 heavy: seen at the game camera as a small yellow mark on the head top (Saved/Renders/task-499/before_after_crop.png). C2 kamikaze: seen as a small yellow mark on the head top, core unchanged at the front (same sheet; pulse not judgeable on stills). C3 chaser v2: AMBIGUOUS, not distinguishable at the default game camera (a few dark pixels), clearly seen at the closer camera (Saved/Renders/task-499/after_closeup_heavy_kamikaze_chaserv2_shieldv2.png). C4 shield chaser v2: AMBIGUOUS, same as C3. C5: seen, heavy and kamikaze emblems stay on the walking robots and during the heavy telegraph (Saved/Renders/task-499/running_two_frames.png), and on the heavy torso death piece (Saved/Renders/task-499/after_heavy_torso_debris.png). C6: sheets Saved/Renders/task-499/before_after_game.png and before_after_crop.png listed for the owner in the final report (the executor cannot send files). 13 launches in total for this task, over the 5 of the capture rules: the first after run showed unmasked squares (material defect fixed in 02) and the debris and shield-break checks needed retries; the shield-break state was not caught on a frame. Cost not recorded (known tooling issue).

## Relations
- [[task-499-robot-logo-decals]] — parent global task.
