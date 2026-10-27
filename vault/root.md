---
id: root
description: "Root index of the Rogue Rigs project vault: routes every task to its branch. Start here, then read _navigation."
kind: note
---
# Rogue Rigs — project vault

Rogue Rigs is a top-down roguelite auto-shooter built with Unreal Engine 5.8 in C++. This vault is the single source of truth for game design, AI agents, technical documentation and production. It is read by several Claude accounts and possibly other AI tools, so everything is plain Markdown with YAML frontmatter.

Read [[_navigation]] before descending. Read [[_authoring]] before creating or modifying any node.

## Routing — pick the branch from the OBJECT of the task, not its topic

| Task | Entry branch |
|---|---|
| Know what the game is or does: rules, entities, look, sound, dialogue | `gdd-index` |
| Run, define or change an AI agent of the dev crew | `agents-index` |
| Write or change C++, Blueprints, pipelines, build | `tech-index`, then the `gdd` node it implements |
| Plan work, check milestones, check what is cut, resume after a handover | `production-index` |
| Pick up, create or update a work item | `tasks-index` |
| Course material, assignments, rubrics, feedback | `course-index` |

`gdd` says WHAT the game does; `tech` says HOW it is built. A code task reads both, in that order.

## Children
- [[gdd-index]] — the game specification: pillars, glossary, run, player, weapons, enemies, terrain, art and audio.
- [[agents-index]] — the dev crew: one node per agent with role, inputs, output format and token ceiling.
- [[tech-index]] — how the game is built: conventions, C++ systems, Blueprint docs, pipelines.
- [[production-index]] — milestones, scope rules and the wishlist of cut ideas.
- [[tasks-index]] — Orchestrator work items and their status.
- [[course-index]] — ELVTR course material: lesson summaries, assignments, rubrics, instructor feedback.

## Relations
- [[_navigation]] — how to traverse the vault: read FIRST.
- [[_authoring]] — writing contract: read BEFORE any create/modify.
- [[_paths]] — generated id -> path map; open once per session to resolve a non-task cross-branch wikilink.
