---
id: _navigation
description: "Reading protocol for the vault: route from root, descend via Relations glosses only, never search or guess paths, fail loudly."
parent: root
kind: rule
applies_to: reading
---
# Vault navigation protocol

Works with any tool that can list a directory and read a file.

1. Read `vault/root.md`. Pick the entry branch with its `## Routing` table: route on the OBJECT of the task, not on its topic.
2. Descend via `[[wikilinks]]`. In an index, `## Children` comes first and is the descent path: its glosses say what each child holds and WHEN to open it. Open a node body only for the branch you need. `## Relations` (parent, cross-links, meta nodes) is lateral: follow it only when the task needs that link.
3. Before opening a node, state in one clause what you expect from it. If you cannot, do not open it. Loading the wrong body costs more than one extra step of descent.
4. Constraint task: first read the `kind: rule` nodes whose `applies_to` matches the task.
5. Read only what the task needs. Never load a whole branch "for context".
6. A field set to `open` is undecided: do not invent a value. Report it, or ask.

## Descent is the only lookup method
- Never build a path by analogy. Use only paths from a directory listing, from `_paths.md`, or from a resolved `[[wikilink]]`.
- Never search the vault (grep, glob, full-text) to locate a node.
- `node vault/scripts/descend.js <index-id> [<index-id>...]` is descent, like `path-of.js` (owner 2026-11-13, [[task-534-descent-script]]): for exact index ids only, it prints in one call the `## Children` of each index and of its child indexes (one level, no deeper), each line as id, path, gloss. It has no text or topic parameter and prints no body and no `## Relations`; choose by the glosses, then open the node.
- If descent does not reach what you need: STOP and report the trajectory (last node reached, and that no gloss matches). A failed descent is a defect in the tree; it must stay visible so the tree gets fixed.

## Execution exception (owner intent 2026-10-27, [[task-327-subtask-paths]])
During execution of a confirmed task, the Executor and the agents it launches may open a task or subtask node directly from a path printed by `task-start.js` / `task-next.js` (`next: <path>`) or listed in the global node's `## Subtasks`, without descending through the indexes and without re-walking the nodes. Searching (grep, glob, find) to locate a node stays forbidden, and a task path is never built by analogy. Reading outside execution is unchanged: everything above applies.

## Resolving a `[[wikilink]]`
A wikilink is an id, and the file is `<id>.md`.
1. Parent link: the child is in the current node's directory or a direct subdirectory. List that directory and take the path from the listing.
2. Cross-branch link: open `vault/_paths.md` once per session; it maps every non-task id to its path. Or, for ids you already hold, run `node vault/scripts/path-of.js <id> [<id>...]`: it prints `vault/<path>` per id and matches the exact id only (never partial or fuzzy), so it is resolution, not search; an unknown id exits 1. Searching a topic stays forbidden.
3. Task link (`task-NNN-slug`, `task-NNN-MM-slug`): not in `_paths.md` (`path-of.js` resolves it). A global task is `reference/tasks/task-NNN-slug.md`; its subtasks are in the folder `reference/tasks/task-NNN-slug/` (list it). Open tasks through `tasks-index`. The global `## Subtasks` list gives each subtask path (`id (path) — agent: goal`); during execution open it directly (see "Execution exception" above).

## Relations
- [[_authoring]] — the writing counterpart.
- [[_paths]] — generated id -> path map.
- [[root]] — the map this protocol starts from.
