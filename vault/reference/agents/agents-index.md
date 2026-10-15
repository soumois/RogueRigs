---
id: agents-index
description: "The dev crew: one node per AI agent, their lessons by theme, and the orchestration protocol. Open before running or changing an agent."
parent: root
kind: note
---
# Dev crew

The main Claude Code session acts as the Orchestrator. It splits each owner request into subtasks and delegates each to one specialized subagent (`.claude/agents/`). Agents never talk to each other: they read inputs from and write outputs to this vault and the repo. Token ceilings are first estimates, corrected after each milestone retrospective.

Crew in use: orchestrator → cpp, bp (code and editor); level-design (layouts); text → style-guide (critic); art → style-guide → 3d → bp (assets).
Planned, not written yet: agent-balance, agent-animation, agent-sound, agent-qa (deferred).

Lessons are filed by theme, and each agent reads the node about its own limits: owner rules for the Orchestrator, [[bp-mcp-lessons]] for bp, [[art-3d-lessons]] for art and 3d.

## Relations
- [[root]] — the vault map.
- [[agent-orchestrator]] — the Orchestrator: role, tools, model, limits.
- [[agent-level-design]] — writes arena layouts as validated grid JSON.
- [[agent-cpp]] — writes and builds C++ and Node tooling, runs visual checks, updates system docs.
- [[agent-bp]] — builds the editor layer through Unreal MCP and documents each Blueprint.
- [[agent-text]] — writes player-facing text from the lore nodes, with a retrieval log.
- [[agent-style-guide]] — the critic of text and images: verdict and correction per item against numbered rules.
- [[agent-art]] — image prompts and candidate images with GPT Image 2 through Kie.ai (`Tools/art/kie_image.js`).
- [[agent-3d]] — validated image to cleaned FBX through Meshy and headless Blender (`Tools/art/`).
- [[cloud-session-git]] — committing, tagging and pushing from a cloud session linked to the PC: lock files, identity, trailers; open before a commit from the cloud.
- [[orchestration-protocol]] — the step-by-step procedure the Orchestrator follows for every request, including resume after a limit.
- [[orchestrator-runbook]] — the commands that work: git, editor start and stop, delegation prompt, closing a subtask, dashboard.
- [[orchestrator-lessons]] — owner rules and planning lessons; read before every plan.
- [[orchestrator-script-quirks]] — task scripts, git, editor, shells and permission pitfalls; open before closing a subtask or committing.
- [[bp-mcp-lessons]] — bp agent limits and Unreal MCP working rules; read by bp and before delegating to it.
- [[art-3d-lessons]] — owner art preferences, art and 3D agent limits, FBX and import traps; read by art and 3d.
