---
id: task-424-07-doc-pause-menu
description: "Doc: write sys-pause-menu and wbp-pause-menu, link them in their indexes, update touched docs"
parent: task-424-pause-menu-stats
kind: task
status: done
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
- model: sonnet
- tokens: input 22, cache write 51071, cache read 324249, output 15758
- usage: turns 11, context start 14k peak 55k, resumes 0, 2 min, units 0.18 M
- runs: 1
- milestone: M3
- started: 2026-11-03T07:13:36Z
- waiting_for: none
- result: Commit e976bce9: sys-pause-menu and wbp-pause-menu created, indexes, sys-main-menu, sys-cheats, tool-capture, bp-rr-player-controller updated. Doc agent did not git-show the commits (worked from the brief).

## Relations
- [[task-424-pause-menu-stats]] — parent global task.
