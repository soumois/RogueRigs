---
id: orchestrator-lessons
description: "Owner rules (Pierre) and planning lessons: splitting, questions, unrequested work, rules versus rendering, showing results, measuring. Read before every plan."
parent: agents-index
kind: note
---
# Orchestrator lessons: owner rules and planning

Agent limits live with each agent: [[bp-mcp-lessons]], [[art-3d-lessons]], [[agent-cpp]]. Scripts, git and editor: [[orchestrator-script-quirks]].

## Splitting (owner 2026-10-06, mandatory)
- One logical owner point = one global task (task-NNN); subtasks only when that point needs several systems or agents (the 67 uu grid: render, flow field, layout, switch). Never bundle points to save editor switches or re-reading: task-034-01 bundled 10 points across 9 systems and hit the turn limit before point 2.
- The subtasks shown in a plan match the points the owner sees.
- Editor work is split one main asset per bp subtask ([[orchestration-protocol]] step 3, owner 2026-10-14): the bp subtasks that bundled many assets needed 4 to 6 runs (task-014-04, 016-03, 022-02, 024-05).
- "Prépare la tâche, ne lance rien": create the nodes and show the plan, launch nothing. "Lance les tâches jusqu'au bout": run the whole plan without asking again.

## Owner (Pierre)
- Write in French; never correct his spelling.
- Ask all questions at once, each with a default he can accept with "ok"; "ok" accepts every default. A sentence with two readings is asked once.
- A "non" to every option means no new task: ask or wait, never relaunch on a reinterpreted target. Never add unrequested work (tests, bot profiles, screenshot sessions, full reruns); only fix test expectations a requested change breaks.
- Report an unrelated in-progress task once, then keep reports on the current topic.
- Rules versus rendering: when he changes how something looks, the game rule stays unless he says so.
- Architecture (2026-10-11): events over polling, one generic class shared by systems; `WBP_HUD` is a placement root, one widget per logic ([[hud-refactor-audit]]).
- Colours (2026-10-13): code an object by its main colour only ([[color-code]]).
- He lets provisional values be set (written `provisional, owner <date>`); report decisions to confirm. Placeholder assets are marked temporary in the file name and a vault note ([[placeholder-weapon-icons]]).
- When he cannot test, skip his validations and list the unseen items ([[validation-checklist-2026-10-12]]).
- He reports defects with screenshots and videos: describe what you see before theorising, check with a real game run before deciding a cause. Frames from `.mkv` in `C:\Users\soumo\Videos`: WinGet ffmpeg, `-vf "fps=4,scale=640:-1,tile=4x3" -frames:v 1`.
- He tracks cost per subtask ([[cost-log]]) and wants a stress report as soon as it ends.

## Showing and verifying
- Nobody sees the screen; say plainly what is unverified. Visual checks go to the cpp agent (`-game` run, `rr.Debug.Shots`, see [[agent-cpp]]).
- A task that changes what the player sees (widget layout, HUD, asset on screen) ends with a `-game` capture by cpp, compared with the look before, and sent to the owner; this check is part of the task, not unrequested work (owner 2026-10-16: task-203-07 shrank the health bar and was closed unseen).

## Measuring
- His everyday profile (browser, Discord, Steam open) is the reference: do not ask him to close apps; only a running game is worth noting. At 60 Hz pacing only game-thread time is usable.
- Procedures: [[perf-stress-test-m1]], [[perf-stress-opt-m1]], [[perf-stress-grid-cells]].

## Relations
- [[agents-index]] — parent index.
- [[orchestrator-runbook]] — the commands.
- [[orchestrator-script-quirks]] — scripts, git, editor and permission pitfalls.
- [[bp-mcp-lessons]] — bp agent limits.
- [[art-3d-lessons]] — art and 3D preferences and agent limits.
