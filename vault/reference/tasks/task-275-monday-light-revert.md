---
id: task-275-monday-light-revert
description: "Global task: L_ArenaM1 lights and robot materials back to their Monday 2026-10-19 state, exposure frozen at the Monday auto exposure, darkening shader kept."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Monday lighting back, shader kept

- request: "J'ai l'impression qu'on a tout cassé avec les effets de lumiére, est-ce qu'il est possible de revenir à la lumiére d'avant ( celle de lundi ) et de juste laisser le shader ?" Scope chosen by the owner: "Lumières seules" + "Matériaux des robots"; keep the shader; exposure "Fixe, calée sur lundi" (owner, 2026-10-21, relayed by the main session). Context: after task-274 he saw a white milky veil on robots and the player.
- goal: the map lights and the robot materials are exactly as on Monday 2026-10-19 (state of commit 75610b21, just before task-261-01); PPV_XPGlow exposure is frozen (min = max) at the EV100 the auto exposure settles on with the Monday lights; MPC_PlayerLight (Enable 1, FarBrightness 0.03, Radius 250, Exponent 4) and the M_PP_PlayerLightMask blendable stay.
- acceptance: `git diff 75610b21 HEAD` empty on the DirectionalLight_0 and SkyLight_0 packages, M_Chaser_Meshy_Anim, M_ShieldChaser_Anim, MI_ShieldChaser_Emblem, MI_GB_Shooter, MI_GB_Heavy; editor read-back: sun 6 white castShadows true, sky 0.3 white castShadows true, PPV exposure min = max = measured EV100 (bias 1, bloom 1.3 / -1, mask blendable weight 1), MPC values above with Enable 1; docs carry before -> after and revert values; lint clean; committed.
- gdd_check: art-style-guide A12 and visual-style ("strong directional light with lifted shadows", shadows suspended provisionally by task-261): the revert restores the rule, the suspension lines are removed. No `open` value. Not reverted (owner): blob shadows (264), XP halo (269), deposit crystal (262), pulse fixes (257/267), floor (273), bloom 1.3 / -1, removed PlayerLight (272-15). Per-actor shadow flags of task-263-01 (C++: robots, miner, drone, pickups, weapons, projectiles cast no shadow; grid floor never, blocks per bBlocksCastShadows) are kept pending an owner question: on Monday these actors cast shadows, so restoring it would add a real sun shadow next to the blob disc.
- confirmed_by_owner: yes (2026-10-21, owner answers relayed by the main session)
- milestone: M1
- next_step: task-275-02-robot-materials-monday
- result: open

## Subtasks
1. task-275-01-monday-light-revert — bp, sun and sky packages restored to Monday (git, editor closed), docs.
2. task-275-02-robot-materials-monday — bp, chaser masters, emblem and greybox shooter / heavy instances restored to Monday (git, editor closed), doc.
3. task-275-03-monday-exposure-measure — cpp, one -game run with the Monday PPV (auto exposure, no mask): adapted EV100 measured.
4. task-275-04-exposure-freeze — bp, editor open: PPV_XPGlow min = max at the measured EV100, read-back of everything, docs.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-275-01-monday-light-revert]] — step 1, bp.
- [[task-275-02-robot-materials-monday]] — step 2, bp.
- [[task-275-03-monday-exposure-measure]] — step 3, cpp.
- [[task-275-04-exposure-freeze]] — step 4, bp.
