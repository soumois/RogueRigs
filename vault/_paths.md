---
id: _paths
description: "Generated id -> path map for resolving a wikilink you already hold to its file. Regenerate with: node scripts/lint.js --write. Never edit by hand."
parent: root
kind: note
---
# Wikilink resolution map (generated)

One line per node: `id  path`. Use it to resolve a wikilink whose target is not in the
directory you are reading (cross-branch jump). This is resolution, not search: you
must already hold the id from a `## Relations` gloss. Do not read it to find a topic.

```
agents-index  reference/agents/agents-index.md
art-audio-index  reference/gdd/art-audio/art-audio-index.md
blueprints-index  reference/tech/blueprints/blueprints-index.md
conventions-index  reference/tech/conventions/conventions-index.md
course-index  reference/course/course-index.md
enemies-index  reference/gdd/enemies/enemies-index.md
gdd-index  reference/gdd/gdd-index.md
pipelines-index  reference/tech/pipelines/pipelines-index.md
player-index  reference/gdd/player/player-index.md
production-index  reference/production/production-index.md
root  root.md
run-index  reference/gdd/run/run-index.md
systems-index  reference/tech/systems/systems-index.md
tasks-index  reference/tasks/tasks-index.md
tech-index  reference/tech/tech-index.md
terrain-index  reference/gdd/terrain/terrain-index.md
weapons-index  reference/gdd/weapons/weapons-index.md
```

## Relations
- [[_navigation]] — the protocol that says when to open this map.
- [[root]] — the map this file serves.
