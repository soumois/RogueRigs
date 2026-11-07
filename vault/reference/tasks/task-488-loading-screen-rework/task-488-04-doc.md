---
id: task-488-04-doc
description: "Doc: Doc node of the loading widget updated."
parent: task-488-loading-screen-rework
kind: task
status: done
---
# Richer loading screen (illustration and framed texts): doc

- goal: Doc node of the loading widget updated.
- agent: doc
- depends_on: task-488-03-capture
- inputs: wbp-loading (vault/reference/tech/blueprints/ui/wbp-loading.md), intermission (vault/reference/gdd/run/intermission.md)
- expected_output: see done_when
- done_when: wbp-loading describes the new tree with anchors per umg-layout, the image asset and the restyled bar and spinner; the reserved LoadingImage note is updated; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-488
- usage: included in the executor run of task-488
- runs: 1
- milestone: M2
- started: 2026-11-07T04:39:13Z
- waiting_for: none
- result: wbp-loading updated (commit d082d554): canvas tree with anchors, alignment, offsets and size mode per umg-layout, assets T_LoadingBackground and T_Loading_BottomGradient, restyled bar (flat brushes, riveted strip) and spinner in the frame, LoadingImage now the background image, verified line with the task-488 captures, owner retouch points; relations added in art-prompts-loading-screen and wbp-intermission. Lint clean.

## Relations
- [[task-488-loading-screen-rework]] — parent global task.
