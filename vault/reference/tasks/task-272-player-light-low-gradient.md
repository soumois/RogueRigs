---
id: task-272-player-light-low-gradient
description: "Global task: player light below the rock tops so walls shade the floor; smooth wide light gradient, no disc edge; judged on -game captures."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Low player light and broad gradient

- request: light must not be above the rocks or the rocks no longer block it; no marked light disc, a fade over the whole image accelerating near the player, the rest less dark (owner feedback on task-270/271 captures) (owner, 2026-10-21). Formalised by the main session with the capture procedure of task-270; one global task because both points tune the same light and are judged on the same captures. Extended 2026-10-21 by the owner after the r6-r7 tests (relayed by the main session): also test a hybrid, a faint shadow-casting light for the wall shadows plus a radial light mask shader around the player, and compare both variants.
- goal: The player light sits below the rock tops so walls cast their shadows on the floor, and the screen shows a smooth wide light gradient with no disc edge, judged on -game captures.
- acceptance: Light height below the rock tops (read back); final captures A to H at the same frames as r5 judged by the Orchestrator: wall shadows on the floor behind walls (G), falling wall opens the light (H), no visible disc edge, far floor readable dark violet (measured); light cost stated; docs updated with before, after and revert values; final side by sides in Saved/LookTune/final_272_*.png; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-21).
- confirmed_by_owner: yes (2026-10-21)
- milestone: M1
- next_step: task-272-07-light-mask-writer
- result: open (physical-light variant done in steps 1-5: see subtask results; hybrid variant in progress)

## Subtasks
1. task-272-01-player-light-low-gradient — cpp, baseline G/H and runtime sweep
2. task-272-05-low-light-sweep-r2 — cpp, second sweep on the DRGS ring profile (added after step 1)
3. task-272-02-player-light-low-apply — bp, BP_Miner light values
4. task-272-03-ambient-level-apply — bp, sun, sky, exposure (if changed)
5. task-272-04-low-light-capture — cpp, final captures and side by sides (physical-light variant)
6. task-272-06-light-mask-material — bp, MPC and post-process mask material
7. task-272-07-light-mask-writer — cpp, MPC writer on the miner
8. task-272-08-light-mask-ppv — bp, mask in PPV_XPGlow
8b. task-272-12-light-mask-centre-fix — bp, mask centred on the miner (added after step 7)
9. task-272-09-hybrid-sweep — cpp, hybrid runtime sweep
10. task-272-10-hybrid-apply — bp, winning values applied
11. task-272-11-hybrid-capture — cpp, final captures of both variants

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-272-01-player-light-low-gradient]] — step 1, cpp.
- [[task-272-05-low-light-sweep-r2]] — step 2, cpp.
- [[task-272-02-player-light-low-apply]] — step 3, bp.
- [[task-272-03-ambient-level-apply]] — step 4, bp.
- [[task-272-04-low-light-capture]] — step 5, cpp.
- [[task-272-06-light-mask-material]] — step 6, bp.
- [[task-272-07-light-mask-writer]] — step 7, cpp.
- [[task-272-08-light-mask-ppv]] — step 8, bp.
- [[task-272-12-light-mask-centre-fix]] — step 8b, bp.
- [[task-272-09-hybrid-sweep]] — step 9, cpp.
- [[task-272-10-hybrid-apply]] — step 10, bp.
- [[task-272-11-hybrid-capture]] — step 11, cpp.
