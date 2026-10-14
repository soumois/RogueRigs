---
id: task-162-01-chaser-anim-game-look
description: "See what the animated chaser really looks like in the game: run the game, spawn chasers near the miner and take screenshots over time."
parent: task-162-chaser-anim-game-look
kind: task
status: in-progress
---
# Chaser: animation seen in the game

- goal: See what the animated chaser really looks like in the game: run the game, spawn chasers near the miner and take screenshots over time.
- agent: cpp
- depends_on: none
- inputs: sys-enemies-chaser (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-chaser.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), bp-chaser (vault/reference/tech/blueprints/enemies/bp-chaser.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), orchestrator-lessons (vault/reference/agents/orchestrator-lessons.md)
- expected_output: Screenshots of the game viewport saved by rr.Debug.Shots (Saved/Screenshots), a series of at least 6 shots of a chaser running toward the miner (and one series of the attack), in a short report: command lines used, the file list, what each shot shows (described by the agent after opening the PNGs: parts in place or scattered, which parts, colours, glow, shadow), and a ranked list of causes consistent with what the shots show; no change to the game assets or code in this subtask
- done_when: The game ran headless-ish with -game (editor closed), shots exist and were opened and described; the report names the symptom precisely (for each part: where it is relative to the body); nothing modified in Content/ or Source/; the report committed by name (a markdown or json note in Saved is not committed: put the note in the final message only), the PNGs left untracked
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-162-chaser-anim-game-look]] — parent global task.
