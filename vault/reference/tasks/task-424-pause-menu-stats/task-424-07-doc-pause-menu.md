---
id: task-424-07-doc-pause-menu
description: "Doc: write sys-pause-menu and wbp-pause-menu, link them in their indexes, update touched docs"
parent: task-424-pause-menu-stats
kind: task
status: todo
---
# Pause menu: miner stats screen with Resume and Exit: doc

- goal: From the change summaries of task-424-03, 04 and 05: create sys-pause-menu (template system.md) under systems-ui-index and wbp-pause-menu (template blueprint.md, anchors table per umg-layout) under blueprints-ui-index; update sys-main-menu and sys-cheats if they were touched (Exit path, rr.Debug.PauseMenu), and tool-capture for the debug command.
- agent: doc
- depends_on: task-424-03-pause-menu-cpp, task-424-04-pause-menu-sprites-import, task-424-05-pause-menu-widget
- inputs: systems-ui-index (vault/reference/tech/systems/ui/systems-ui-index.md), blueprints-ui-index (vault/reference/tech/blueprints/ui/blueprints-ui-index.md), sys-main-menu (vault/reference/tech/systems/ui/sys-main-menu.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), tool-capture (vault/reference/tech/pipelines/tool-capture.md), agent-doc (vault/reference/agents/agent-doc.md), vault/templates/system.md, vault/templates/blueprint.md, vault/reference/tech/systems/ui/sys-pause-menu.md (to create), vault/reference/tech/blueprints/ui/wbp-pause-menu.md (to create)
- expected_output: Doc nodes created and updated, indexes linked, lint clean, commit `task-424-07-doc-pause-menu: ...`.
- done_when: sys-pause-menu and wbp-pause-menu exist and describe the code and widget as built; touched docs updated; no gdd node edited; lint clean; committed by path.
- editor_state: any (no editor needed)
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-424-pause-menu-stats]] — parent global task.
