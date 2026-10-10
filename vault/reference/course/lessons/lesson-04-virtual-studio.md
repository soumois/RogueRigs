---
id: lesson-04-virtual-studio
description: "Lesson 04 (Sep 29): building a crew from the real project, prompts for dev agents, hierarchical orchestration, memory as docs, context collapse, engine bridge, Mermaid."
parent: lessons-index
kind: note
---
# Lesson 04 — The virtual studio

## Key points
- The instructor no longer recommends CrewAI or LangGraph for games: frontier models absorbed them. Raw orchestration with Claude is the target.
- Build agents from the real project, one at a time, starting with the orchestrator; challenge the model's proposals as questions.
- Prompts for dev agents: defined input format, specified output schema (fields, types), constraints stated early. A constraint in an agent file stays soft; a hard constraint must be made physically impossible.
- Retrospective on every bug: understand why the agent produced it, then fix the agent.
- Hierarchical over sequential: one manager decomposes the goal into atomic tasks and delegates; a dependency graph so generation starts only when its input is validated.
- Memory is documentation: one `.md` per system, an up-to-date GDD, agents that update the docs. More memory costs more. Too much autonomy produced 670 pages of docs on the instructor's game.
- Context: each agent in its own context; clear after each task; restart from a clean context after repeated failures; archive resolved tasks.
- Engine bridge: detect malformed output before the engine, send errors back to the responsible agent, validate before compiling. The owner is the first QA line: play the latest build.
- Document the architecture with Mermaid; it reveals bottlenecks and circular dependencies.
- Do not get stuck improving the pipeline instead of making the game.

## Applies to Rogue Rigs
- The crew, the vault and the guardrails follow this lesson ([[agents-index]], [[orchestration-protocol]]).

## Relations
- [[lessons-index]] — parent index.
- [[assignment-3-agent-crew]] — the assignment set in this lesson.
