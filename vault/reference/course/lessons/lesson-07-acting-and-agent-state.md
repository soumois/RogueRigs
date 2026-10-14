---
id: lesson-07-acting-and-agent-state
description: "Lesson 07 (Oct 8), part 2: before/after run, human review, pull requests and approval gates, markdown agent state, frameworks vs raw orchestration, Code Architect demo."
parent: lessons-index
kind: note
---
# Lesson 07 — Goal-oriented agents (2/2): act, review, remember

## Acting
- Example: before, `/src` has `game.py` and `player.py`; the GDD requires `inventory.py`, `dialogue.py` and `data/items.json`. After the run, the first two are unchanged; the agent wrote the others, following `player.py` patterns. No manual file creation.
- The owner then reads every generated file. "Follows the patterns" does not mean "matches the design intent". The agent writes the mechanical code; the owner decides if it is right. Then play the game.
- Make the feature testable in one click (developer test room with every item, or a button to the shop), not after 6 to 7 minutes of menus and cinematics.
- Never merge automatically: always a pull request.
- Avoid drift: agents score priorities and act only after human approval or defined criteria (for example a very high score). No consensus means a human contact point: agents present scores and a recommendation, then ask for sign-off.

## Persistent state in markdown
- Plain readable, editable files, no database. The agent writes what happened and re-reads it next session. Four categories: built (every file or feature, timestamped), failed (attempts, compile errors, so it does not repeat them), left (GDD features not done, priority, blockers, required human approvals), decisions and reasoning.
- Keep logs short: raw documentation in its own folder, a second agent compiles a short status. The owner can edit a decision in the file; the next session only has that context.
- The GDD and feature documents are agent memory too. Alternatives: vector databases, MemPalace.
- Daily stand-ups by a product-manager agent are optional (models have no sense of time).

## Frameworks vs raw orchestration
- LangGraph and CrewAI are rarely useful for games. They lock the order and limit tools. Justified case: a dedicated 3D generation pipeline (Tripo or Meshy by API) that must behave identically every time, with non-LLM steps.

## Demo: Code Architect
Receive (GDD feature list and project folder), scan and diff (built vs missing), score and prioritise (dependency order, project needs), build (code for the top feature, update the markdown state file). A live blackboard shows what it sees, how it scored and what code it writes. The live part is incomplete in the transcript.

## Q&A
A pixel-art character from PixelLab goes into the prototype as a gray box first (a square that moves), then the sprite.

## Relations
- [[lessons-index]] — parent index.
- [[lesson-07-goal-oriented-agents]] — part 1: perception and scoring.
- [[assignment-5-goal-oriented-agent]] — blackboard and review requirements.
