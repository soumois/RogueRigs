---
id: agents-index
description: "The dev crew: one node per AI agent (role, inputs, output format, token ceiling) and the orchestration protocol. Open before running or changing an agent."
parent: root
kind: note
---
# Dev crew

The main Claude Code session acts as the Orchestrator. It splits each owner request into subtasks and delegates each to one specialized subagent (`.claude/agents/`). Agents never talk to each other: they read inputs from and write outputs to this vault and the repo. Token ceilings are first estimates, corrected after each milestone retrospective.

Assignment #3 crew (provisional): orchestrator → level-design → cpp → bp.
Content crew (Assignment #4, provisional): orchestrator → text → style-guide (critic).
Art crew (after Assignment #4, provisional): orchestrator → art → 3d → bp.
Planned, not written yet: agent-balance, agent-animation, agent-sound, agent-qa (deferred).

## Relations
- [[root]] — the vault map.
- [[agent-orchestrator]] — the Orchestrator: role, tools, model, limits.
- [[agent-level-design]] — writes arena layouts as validated grid JSON.
- [[agent-cpp]] — writes and builds C++ gameplay code, updates system docs.
- [[agent-bp]] — builds the Blueprint layer through Unreal MCP and documents each Blueprint.
- [[agent-text]] — writes player-facing text from the lore nodes, with a retrieval log.
- [[agent-style-guide]] — the critic: verdict and correction per item against numbered rules.
- [[agent-art]] — image prompts and candidate images through the OpenAI image API.
- [[agent-3d]] — validated image to cleaned, split FBX through Meshy or Tripo and Blender MCP.
- [[orchestration-protocol]] — the step-by-step procedure the Orchestrator follows for every request, including resume after a limit.
- [[orchestrator-runbook]] — the commands that work: git, editor start and stop, delegation prompt, closing a subtask, dashboard.
- [[orchestrator-lessons]] — bp agent and MCP limits, visual checks, measuring, owner preferences.
