---
id: task-424-07-doc-pause-menu
description: "Doc: write sys-pause-menu and wbp-pause-menu, link them in their indexes, update touched docs"
parent: task-424-pause-menu-stats
kind: task
status: todo
---
# Pause menu: miner stats screen with Resume and Exit: doc

- goal: Document the pause menu C++ system and its widget from the Executor brief.
- agent: doc
- depends_on: task-424-03-pause-menu-cpp, task-424-04-pause-menu-sprites-import, task-424-05-pause-menu-widget
- inputs: agent-doc (vault/reference/agents/agent-doc.md), `vault/reference/tech/systems/ui/sys-pause-menu.md` (new, template system.md), `vault/reference/tech/blueprints/ui/wbp-pause-menu.md` (new, template blueprint.md, anchors per umg-layout), systems-ui-index (vault/reference/tech/systems/ui/systems-ui-index.md), blueprints-ui-index (vault/reference/tech/blueprints/ui/blueprints-ui-index.md), sys-player (vault/reference/tech/systems/core/sys-player.md) (pause input binding, if changed), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md) (rr.Debug.PauseMenu, if it lists debug commands), sys-gas-core (vault/reference/tech/systems/core/sys-gas-core.md) (URRWeaponAttributeSet is implemented: fix the "not yet" line), pause-menu (vault/reference/gdd/run/pause-menu.md), umg-layout (vault/reference/tech/conventions/umg-layout.md)
- expected_output: New doc nodes sys-pause-menu and wbp-pause-menu with parents and index lines; touched docs updated; pause-menu Relations linked to both.
- done_when: Both nodes exist under 400 words, list the public API, the debug command and the widget anchors; indexes and Relations on both ends updated; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-424-pause-menu-stats]] — parent global task.
