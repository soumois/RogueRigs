---
id: agents-index
description: "The dev crew: one node per AI agent (role, inputs, output format, token ceiling) and the orchestration protocol. Open before running or changing an agent."
parent: root
kind: note
---
# Dev crew

The main Claude Code session acts as the Orchestrator. It splits each owner request into subtasks and delegates each to one specialized subagent (`.claude/agents/`). Agents never talk to each other: they read inputs from and write outputs to this vault and the repo. Token ceilings are first estimates, corrected after each milestone retrospective.

Assignment #3 crew (provisional): orchestrator → level-design → cpp → bp.
Planned, not written yet: agent-cpp, agent-bp, agent-balance, agent-text, agent-style-guide, agent-art, agent-animation, agent-sound, agent-qa (deferred).

## Relations
- [[root]] — the vault map.
- [[agent-orchestrator]] — the Orchestrator: role, tools, model, limits.
- [[agent-level-design]] — writes arena layouts as validated grid JSON.
- [[orchestration-protocol]] — the step-by-step procedure the Orchestrator follows for every request, including resume after a limit.
