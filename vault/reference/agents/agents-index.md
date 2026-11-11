---
id: agents-index
description: "The dev crew: one node per AI agent, their lessons by theme, and the orchestration protocol. Open before running or changing an agent."
parent: root
kind: note
---
# Dev crew

The main Claude Code session only relays between the owner and two agents: the Planner turns each owner request into confirmed task nodes, then one fresh Opus Executor per global task does the work itself, applying the role nodes below (owner 2026-11-05). Agents read inputs from and write outputs to this vault and the repo. Token ceilings are bounds on a subtask's weighted units (`<x> M units`), provisional, corrected after each milestone retrospective.

Crew in use: planner → executor → style-guide (critic of its text and images).
Not written yet: agent-balance, agent-animation, agent-sound, agent-qa (deferred).

Lessons are filed by theme: owner rules for the Planner, [[bp-mcp-lessons]] for bp, [[art-3d-lessons]] for art and 3d.

## Children
- [[agent-planner]] — plans a request into confirmed task nodes and asks the owner; open to run or change planning.
- [[agent-executor]] — one Opus agent runs one confirmed global task from its nodes, doing each subtask itself; open to run or change execution.
- [[agent-level-design]] — writes arena layouts as validated grid JSON.
- [[agent-cpp]] — writes and builds C++ and Node tooling, runs stress and tests, returns a change summary.
- [[agent-bp]] — builds the editor layer through Unreal MCP, returns a change summary.
- [[agent-capture]] — runs `-game` captures from a planner checklist, reports each point seen, not seen or ambiguous; Sonnet, edits nothing.
- [[agent-doc]] — writes the `sys-*` and `bp-*` nodes of a task from the Executor's brief, in its own subtask.
- [[agent-text]] — writes player-facing text from the lore nodes, with a retrieval log.
- [[agent-style-guide]] — the critic of text and images: verdict and correction per item against numbered rules.
- [[agent-art]] — image prompts and candidate images with GPT Image 2 through Kie.ai (`Tools/art/kie_image.js`).
- [[agent-3d]] — validated image to cleaned FBX through Meshy and headless Blender (`Tools/art/`).
- [[cloud-session-git]] — committing, tagging and pushing from a cloud session linked to the PC: lock files, identity, trailers; open before a commit from the cloud.
- [[request-families]] — entry routing of a request into réglage, comportement, visuel or gros morceau; open to relay a request or change the routing.
- [[orchestration-protocol]] — planning procedure of the Planner for every request, steps 0 to 4.
- [[execution-protocol]] — execution procedure of the Executor, steps 5 and 6, including resume.
- [[task-spec]] — how a plan is written: task-create spec, defaults file, confirmation; read before writing a plan.
- [[verification-levels]] — verification level of a global task: criteria, verification size, indicative executor budget; open to plan or run a task.
- [[orchestrator-runbook]] — the commands that work: git, editor start and stop, delegation prompt, closing a subtask, dashboard.
- [[orchestrator-lessons]] — owner rules and planning lessons; read before every plan.
- [[orchestrator-script-quirks]] — task scripts, git, editor, shells and permission pitfalls; open before closing a subtask or committing.
- [[bp-mcp-lessons]] — bp agent limits and Unreal MCP working rules; read by bp and before delegating to it.
- [[art-3d-lessons]] — owner art preferences, art and 3D agent limits, FBX and import traps; read by art and 3d.

## Relations
- [[root]] — the vault map.
