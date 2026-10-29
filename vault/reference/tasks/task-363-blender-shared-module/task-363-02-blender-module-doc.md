---
id: task-363-02-blender-module-doc
description: "Doc: 3d definition, agent-3d, blender-asset-route and art-3d-lessons tell the agent to import rr_blender instead of rewriting helpers."
parent: task-363-blender-shared-module
kind: task
status: done
---
# Shared Blender helper module for the 3d agent: doc

- goal: From the global (`request`, `gdd_check`, decided `## Questions`) and the 01 change summary: in `.claude/agents/3d.md` step 4 add that a Blender script imports `rr_blender` (the import lines) and never redefines its helpers, reading only the module header docstring for the API; in agent-3d `blender_route` add the same in one clause; in blender-asset-route add a short "Shared module" section (path, import lines, API = module header docstring, rule: generic helpers go in the module, task geometry stays in the script, never change an existing signature) and list the module in "Running"; in art-3d-lessons "Agent limits" add one line. Change nothing else.
- agent: doc
- depends_on: task-363-01-rr-blender-module
- inputs: task-363-blender-shared-module (vault/reference/tasks/task-363-blender-shared-module.md), `.claude/agents/3d.md`, agent-3d (vault/reference/agents/agent-3d.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), `Tools/art/blender/rr_blender.py` (header docstring only), _authoring (vault/_authoring.md)
- expected_output: the four files updated, one commit by path.
- done_when: `.claude/agents/3d.md`, agent-3d, blender-asset-route and art-3d-lessons each name `rr_blender` and say not to redefine its helpers; the function names in blender-asset-route match the module header docstring; blender-asset-route stays under ~450 words; no other file changed; `node vault/scripts/lint.js --write` clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: sonnet
- tokens: input 10, cache write 23814, cache read 79378, output 2608
- usage: turns 5, context start 11k peak 27k, resumes 0, 0 min, units 0.05 M
- runs: 1
- milestone: M2
- started: 2026-10-29T15:41:03Z
- waiting_for: none
- result: Doc committed 790b3efd: 3d.md step 4, agent-3d, blender-asset-route (Shared module section), art-3d-lessons name rr_blender. blender-asset-route was already 533 words by wc before (now 616 with markup); section itself is 3 bullets.

## Relations
- [[task-363-blender-shared-module]] — parent global task.
