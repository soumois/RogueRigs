---
id: lesson-07-goal-oriented-agents
description: "Lesson 07 (Oct 8), part 1: task-oriented vs goal-oriented agents, codebase perception in four steps, utility scoring, the priority problem, core-loop advice."
parent: lessons-index
kind: note
---
# Lesson 07 — Goal-oriented agents (1/2): perceive and prioritise

## Task-oriented vs goal-oriented
- Agents so far follow instructions ("write this dialogue"). A goal-oriented agent uses perception and retrieval over local files (GDD, lore, code) to decide what to do, with a goal in mind.
- The owner no longer says "build this, then the next one". The agent loads what the GDD requires, finds what is missing and proposes solutions. The owner stays on design, game feel and playtesting. "The agent says it is good" is never enough.

## Codebase perception (four steps)
1. Folder structure. 2. Existing files and imports: what is wired vs stubbed. 3. Diff against the GDD feature list. 4. Gap identification.
- The agent reads like a new developer: onboarding documentation matters.
- An inconsistent structure (four graphics folders) makes the agent report as missing what exists elsewhere, every session. Keep one clear structure.
- If possible, give the agent engine perception and the ability to play the game. Instructor's workflow: implement, run the game, capture the feature in action, check the capture, only then call it done.

## Utility scoring
- Several things are missing; which first? Dependencies matter (no shop UI before inventory; no combat without level and movement).
- Factors: dependency order, blockers (what it unblocks), project priority (what the GDD marks critical), current state (partial vs intact). No exact formula is required, a structure that says what comes first is.
- Example (dungeon crawler): 1 inventory (blocks shop and loot), 2 enemy AI (blocks core feel), 3 save/load, 4 level generation (blocks nothing yet), 5 UI polish (depends on all). Inventory and AI are unrelated, so parallel teams.
- Save/load and co-op are decided early: everything built later must plug into them.
- Underrated priority: volume control (minutes of work, spares the players' ears).
- Without a priority structure, 30 agents build any GDD line, even a passing idea with no "post-1.0" section. Being able to add a mechanic is no reason to add it.
- Core loop first: the instructor spent 15 to 20 hours on one Garbler shot. More content does not improve a game; good content does, once the game is already good.

## Relations
- [[lessons-index]] — parent index.
- [[lesson-07-acting-and-agent-state]] — part 2: review, approval, markdown state, demo.
- [[assignment-5-goal-oriented-agent]] — the assignment this lesson sets.
