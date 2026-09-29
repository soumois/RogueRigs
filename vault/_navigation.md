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
2. Descend via `[[wikilinks]]`. At each node, the `## Relations` glosses say what each child holds and WHEN to open it. Open a node body only for the branch you need.
3. Before opening a node, state in one clause what you expect from it. If you cannot, do not open it. Loading the wrong body costs more than one extra step of descent.
4. Constraint task: first read the `kind: rule` nodes whose `applies_to` matches the task.
5. Read only what the task needs. Never load a whole branch "for context".
6. A field set to `open` is undecided: do not invent a value. Report it, or ask.

## Descent is the only lookup method
- Never build a path by analogy. Use only paths from a directory listing, from `_paths.md`, or from a resolved `[[wikilink]]`.
- Never search the vault (grep, glob, full-text) to locate a node.
- If descent does not reach what you need: STOP and report the trajectory (last node reached, and that no gloss matches). A failed descent is a defect in the tree; it must stay visible so the tree gets fixed.

## Resolving a `[[wikilink]]`
A wikilink is an id, and the file is `<id>.md`.
1. Parent link: the child is in the current node's directory or a direct subdirectory. List that directory and take the path from the listing.
2. Cross-branch link: open `vault/_paths.md` once per session; it maps every id to its path.

## Relations
- [[_authoring]] — the writing counterpart.
- [[_paths]] — generated id -> path map.
- [[root]] — the map this protocol starts from.
