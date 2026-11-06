---
id: task-333-nebula-visible
description: "Global task: make the nebula layer of the space background clearly visible at normal brightness, by retuning MI_Space_Nebula only."
parent: tasks-done-index
kind: task
status: cancelled
---
# Clearly visible nebula in the void

- request: owner, 2026-10-27 (verbatim, French): "Il faut plus un effet nébuleuse et d'autres astéroid plus proche ( on est sur un astéroid )". Point 1 of 2 (nebula); the nearer asteroids are [[task-334-near-asteroids]]. Context relayed by the main session: in the task-332-03 capture the nebula peak luma is about 1.4 (Intensity 0.06), visible only after a 25x boost; the mask (FarBrightness 0.08) multiplies the background by about 0.08 far from the miner. No file given. Owner answer (2026-10-27, relayed verbatim by the main session): "ok", after being told "ok" accepts every default and confirms the plan, so N1 to N6 take their defaults (decided values written in [[visual-style]] and [[color-code]]).
- goal: at normal brightness, near an arena edge, the player sees soft violet-magenta nebula clouds in the void behind the stars, while the floor and units still read first.
- acceptance: MI_Space_Nebula retuned and saved, no graph change of M_SpaceBackground (subtask 01); a `-game` capture near an arena edge shows the nebula without any boost, nebula peak sRGB luma in the band set by N1, measured (subtask 02); doc node bp-space-background-material updated (subtask 03); committed by explicit paths.
- gdd_check: read [[visual-style]] (void background line, owner 2026-10-27 provisional: "always darker than the floor beside it (nebula peak at most the far floor value)", "the darkening mask applies to it"), [[color-code]] (void background palette), [[bp-space-background-material]] (parameters and finding of task-332-03: void mean luma 0.16, floor strips 18 to 49, brightest star 108, nebula peak about 1.4), [[mpp-player-light-mask]], [[milestones]] (M3 polish, in scope, as task-332). Conflict: "clearly visible" supersedes Q4 of [[task-332-space-background]] (nebula peak at most the far floor value, about 10/255); flagged to the owner (N1). Questions, each with its default; owner "ok" (2026-10-27) = every default below is DECIDED (provisional values, tuned by eye later):
  - N1 brightness: default nebula peak about 20 to 35 sRGB luma in the edge capture, mean void luma at least 4: brighter than the far floor (about 10) but still below the floor strip beside the edge (18 to 49); stars stay the brightest points. Alternative: nebula allowed as bright as or brighter than the floor.
  - N2 darkening mask: default kept on the background (no stencil exception, M_PP_PlayerLightMask untouched); Intensity compensates, so the nebula is strongest near the miner and fades toward the screen corners. Alternative: exempt the background from the mask (cross-system change, own subtask on the post-process material).
  - N3 shape: default MI_Space_Nebula parameters only (Intensity, and Density, Contrast, Scale if needed for clouds covering about a third to a half of the void with soft edges); palette of Q3 (task-332) unchanged; no material graph change.
  - N4 dust: default MI_Space_Dust and MI_Space_Stars untouched (not asked; the near layer becomes the asteroids of task-334).
  - N5 capture: default one `-game` capture near an arena edge by cpp, two shots about 3 s apart, numbers measured, sheet sent to the owner (lessons rule 2026-10-16, visible change). No other run.
  - N6 order: default this task runs before task-334, so the asteroid capture shows the final nebula.
- confirmed_by_owner: yes (2026-10-27, "ok", relayed by the main session)
- milestone: M3
- next_step: none
- started: 2026-10-27T21:59:10Z
- waiting_for: none
- executor_tokens: input 16, cache write 33643, cache read 191200, output 2778
- executor_usage: runs 1, turns 8, context start 16k peak 34k, resumes 1, 3 min, units 0.08 M
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Cancelled 2026-10-27, superseded by task-335-space-texture-layers (S1, owner ok): owner judged the procedural nebula not working. 01 done: MI_Space_Nebula Intensity 0.06 to 1.6, commit 975425cf (stays, plane switched by task-335). 02 capture taken, not measured: Saved/Captures/task333_nebula/, used as the before of task-335-05. 03 docs folded into task-335-06.

## Subtasks
1. task-333-01-nebula-tune (vault/reference/tasks/task-333-nebula-visible/task-333-01-nebula-tune.md) — bp: retune MI_Space_Nebula so the nebula reaches the N1 band.
2. task-333-02-capture (vault/reference/tasks/task-333-nebula-visible/task-333-02-capture.md) — cpp: one `-game` capture near an arena edge, nebula and floor luma, sheet for the owner.
3. task-333-03-docs (vault/reference/tasks/task-333-nebula-visible/task-333-03-docs.md) — doc: update bp-space-background-material (parameter table, finding).

## Relations
- [[tasks-done-index]] — parent index.
- [[task-333-01-nebula-tune]] — step 1, bp.
- [[task-333-02-capture]] — step 2, cpp.
- [[task-333-03-docs]] — step 3, doc.
- [[task-332-space-background]] — the background this retunes; its Q4 is superseded for the nebula.
- [[task-334-near-asteroids]] — point 2 of the same owner message.
- [[bp-space-background-material]] — the material and its finding.
