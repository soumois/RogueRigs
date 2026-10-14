---
id: task-158-chaser-glow-mask-fix
description: "Global task: Stop the whole chaser from glowing: only the red parts of the texture may emit light."
parent: tasks-done-index
kind: task
status: done
---
# Chaser: glow only on the red parts

- request: In game the whole yellow chaser glows like a lamp (owner video, 2026-10-14); cause: the red mask of the emissive, saturate((R - max(G, B)) x 4), is also positive on yellow (linear yellow 0.91, 0.49, 0.02 gives about 1) (owner, 2026-10-14)
- goal: Stop the whole chaser from glowing: only the red parts of the texture may emit light.
- acceptance: Both materials recompile without error; the mask expression read back; the assets saved and committed by name; the docs updated; the editor stays open; the owner is told to look at the yellow body (no glow) and the red eye (glow); committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-14).
- confirmed_by_owner: yes (2026-10-14)
- milestone: M2
- next_step: none
- result: Done. emissive mask saturate((R - 2 max(G,B)) x 3) in both chaser materials; Max node blue input read back as G (harmless for yellow/olive/dark/red)

## Subtasks
1. task-158-01-chaser-glow-mask-fix — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-158-01-chaser-glow-mask-fix]] — step 1, bp.
