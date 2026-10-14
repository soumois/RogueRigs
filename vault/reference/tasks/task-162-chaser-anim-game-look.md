---
id: task-162-chaser-anim-game-look
description: "Global task: See the animated chaser in the game: spawn chasers near the miner and take screenshots."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Chaser: animation seen in the game

- request: In game the animated chaser falls apart while it runs, although the baked data checks out in Blender (EXR path equals keyed path within 0.034 cm, four fixed poses look like a run); nobody has seen what the engine renders, so the cause (texture import, material plumbing, part index, time, mesh space) is unknown (owner, 2026-10-14) (owner, 2026-10-14)
- goal: See what the animated chaser really looks like in the game: run the game, spawn chasers near the miner and take screenshots over time.
- acceptance: The game ran headless-ish with -game (editor closed), shots exist and were opened and described; the report names the symptom precisely (for each part: where it is relative to the body); nothing modified in Content/ or Source/; the report committed by name (a markdown or json note in Saved is not committed: put the note in the final message only), the PNGs left untracked; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-14).
- confirmed_by_owner: yes (2026-10-14)
- milestone: M2
- next_step: task-162-01-chaser-anim-game-look
- result: open

## Subtasks
1. task-162-01-chaser-anim-game-look — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-162-01-chaser-anim-game-look]] — step 1, cpp.
