---
id: orchestrator-lessons-2026-10-12
description: "Owner rules and agent, tool and script lessons learned running gameplay, HUD, pool and ground-circle tasks on 2026-10-07 to 10-12. Open before planning."
parent: agents-index
kind: note
---
# Orchestrator lessons, session of 2026-10-12

Complements [[orchestrator-lessons]] and [[orchestrator-runbook]].

## Owner rules (Pierre)
- Rules versus rendering: when the owner changes how something looks, the game rule stays unless he says so; ask once if a sentence can be read both ways (the capture zone "n'empêche pas la progression" meant: rendering only).
- HUD architecture (owner 2026-10-11): `WBP_HUD` is a placement root containing one widget per logic; each widget binds to events; only continuous values stay per frame ([[hud-refactor-audit]]). Same spirit elsewhere: events over polling, one generic class shared by systems (the actor pool, the ground telegraph).
- Placeholder assets are marked temporary in the file name and in a vault note ([[placeholder-weapon-icons]]); the Orchestrator may copy the owner-chosen images into `vault/assets/` when he says so.
- The Orchestrator may force-close its own editor when the git tree is clean (owner 2026-10-11); never close an editor whose "Save Content" dialog is open without asking.
- "Prépare la tâche, ne lance rien": create the nodes and show the plan, do not launch.
- When the owner cannot test, skip his validations and go on; list the unseen items ([[validation-checklist-2026-10-12]]).
- Say plainly what is unverified: nobody sees the screen, agents read values back.

## Agents
- bp agents often stop at the 40-turn limit without committing: tell them to commit after each group, and resume with SendMessage; check `git log` and `git status` first, since unsaved work may live only in the open editor.
- cpp agents stop at 60 turns while a long test run goes on: resume them to collect the result.
- Parallel bp agents are fine on different assets (up to 3 at once on one editor).
- An agent's description must start with the subtask id (`task-NNN-SS ...`) or `record-cost.js` finds no usage.

## Tools and scripts
- `task-next.js` closes the global task when the next global differs: for a global with several subtasks use `task-status.js` per subtask. It also fails if new subtask files are untracked (add them first).
- `git add` of an ignored path (`Reference/`) fails the whole command: add by name and skip ignored paths.
- Two sessions create task numbers in parallel: `ls vault/reference/tasks` right before creating ([[handover-2026-10-12-gameplay-hud]]).
- Unreal MCP: `set_properties` takes `values` as a JSON string; SizeBox overrides need their `bOverride_*` flag; pure nodes are re-evaluated at each consumer (a time delta computed after the setter is zero); `TextureTools.import_file` refuses to overwrite (delete, import, set the UI texture group again); material graphs can be built with `MaterialTools`; more in [[unreal-mcp-quirks]].
- A Windows Store `python3` alias hangs on stdin and leaves stuck shells (the app showed "4 running tasks"): use node, and kill the stuck `bash.exe` and `python3.exe` trees.
- Headless tests and builds need the editor closed; a build "up to date" after asset-only work means the tests do not exercise the Blueprints.

## Relations
- [[agents-index]] — parent index.
- [[orchestrator-lessons]] — the earlier lessons.
- [[orchestrator-runbook]] — commands.
- [[handover-2026-10-12-gameplay-hud]] — the session state these lessons come from.
