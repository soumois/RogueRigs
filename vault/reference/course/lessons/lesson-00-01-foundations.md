---
id: lesson-00-01-foundations
description: "Lessons 00–01 (Sep 15, 17): course philosophy, grading, AI in the dev pipeline vs at runtime, scripts to agents, the AI contract, getting unstuck."
parent: lessons-index
kind: note
---
# Lessons 00–01 — Welcome and foundations of agency

## Key points
- Game first: every technique serves shipping a playable game. A finished game is the portfolio.
- Two uses of AI: agents in the dev pipeline (the course's focus: code, levels, GDD review, assets, QA, tests, balance) and AI inside the game (rare: latency, token cost per player, loss of control).
- Prefer generating ahead of time, validating by a human, and shipping frozen content.
- From scripts to agents: hard-coded automation → state machines and behavior trees → utility AI (score each option) → LLM agents as the decision engine. These add to existing methods; they do not replace them.
- The AI contract: creative emergence on one side, predictable outputs on the other. Constrain and validate outputs so they never break mechanics, lore or logic.
- Code moves from "written by hand" to "reviewed by hand"; guardrails are integration checks and automated verification.
- AI does not know what is fun or beautiful: human direction stays essential. "It has never been easier to make a game, and it is still as hard to make a good one."
- Getting unstuck: describe the problem, have the model read the docs, paste the error, verify the result; ask the model where its answer comes from.
- Q&A: deliver through a GitHub link; evaluate agents by success rate and token use; a short GDD (2–3 pages) for prompts plus detailed docs; 3D is about ten times harder than 2D (Blender MCP, Meshy).

## Applies to Rogue Rigs
- No AI at runtime is a hard constraint of the project ([[gdd-index]]).

## Relations
- [[lessons-index]] — parent index.
