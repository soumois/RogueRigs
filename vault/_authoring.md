---
id: _authoring
description: "Writing contract: frontmatter, node kinds, status values, templates, size limits, Blueprint doc rule, lint and commit. Read before any create or modify."
parent: root
kind: rule
applies_to: writing
---
# Vault writing contract — read BEFORE creating or modifying any node

## File and placement
- Location: `vault/reference/<branch>/…/<id>.md`. File name = id, kebab-case, unique.
- A node lives in its parent's directory or a direct subdirectory of it (lint-enforced). A new subdirectory needs an index node whose parent is the enclosing index.

## Frontmatter (required)
- `id`, `description`, `parent` (root excepted), `kind`.
- `description`: one dense sentence, max 25 words, "what's inside + when to open". It drives descent.
- `kind`:
  - `note`: index or prose.
  - `rule`: a contract; also needs `applies_to`.
  - `spec`: a game entity or rule (enemy, weapon, objective…); also needs `status`.
  - `doc`: technical documentation of a C++ system or a Blueprint.
  - `task`: an Orchestrator work item; also needs `status`.
- `status` for `spec`: `approved` | `provisional` | `open` | `deferred`.
- `status` for `task`: `todo` | `in-progress` | `review` | `done` | `blocked` | `cancelled` | `wait-answers` (stopped for the owner; `waiting_for` says what for).

## Body
- English. Concise, factual, no filler. ONE concept per node.
- Max ~400 words per node; beyond that, split. Max ~30 entries per index.
- Use the matching file in `vault/templates/`. Every template field is mandatory: an undecided value is written `open`, never left out.
- End with `## Relations`: `- [[id]] — <relation + when to open>`.
- A fact already stated elsewhere is referenced with a wikilink, never copied.

## Blueprint documentation rule
- Every Blueprint has a `doc` node in `tech/blueprints/` (template `blueprint.md`).
- A task that changes a Blueprint is not done until its doc node is updated.
- C++ has no per-class doc: each system gets one `doc` node in `tech/systems/` (template `system.md`); function-level detail lives in header comments.

## After writing (mandatory)
- Update `parent` and `## Relations` on both ends.
- Run `node vault/scripts/lint.js --write` from the repo root and fix what it reports. Never edit `_paths.md` by hand.
- Commit with a meaningful message. History is git; there is no log file.

## Forbidden
- Secrets (keys, tokens, passwords). Reference them by name only.
- Orphans, duplicated facts, invented values for `open` fields.
- Editing a `kind: rule` node without explicit intent from the project owner.

## Relations
- [[_navigation]] — the reading counterpart.
- [[root]] — the map every parent chain must reach.
