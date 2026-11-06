---
id: task-461-04-doc
description: "Doc: Docs describe the bottom panel and the XP frame."
parent: task-461-bottom-hud-panel
kind: task
status: done
---
# Bottom HUD built as one riveted panel and a framed XP bar: doc

- goal: Docs describe the bottom panel and the XP frame.
- agent: doc
- depends_on: task-461-03-xp-frame
- inputs: wbp-weapons (vault/reference/tech/blueprints/ui/wbp-weapons.md), wbp-xp-status (vault/reference/tech/blueprints/ui/wbp-xp-status.md), wbp-hud (vault/reference/tech/blueprints/ui/wbp-hud.md), blueprints-ui-index (vault/reference/tech/blueprints/ui/blueprints-ui-index.md), art-prompts-hud-unified-mockup (vault/reference/gdd/art-audio/art-prompts-hud-unified-mockup.md)
- expected_output: see done_when
- done_when: Nodes state the current look and assets; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-461
- usage: included in the executor run of task-461
- runs: 1
- milestone: M2
- started: 2026-11-06T04:37:39Z
- waiting_for: none
- result: wbp-health-status, wbp-weapons, wbp-xp-status, wbp-hud, blueprints-ui-index and art-prompts-hud-unified-mockup state the current look and assets (bottom panel, slot wells, XP frame, Lv cap, positions at 1080p); lint clean. Commit 7705ad12.

## Relations
- [[task-461-bottom-hud-panel]] — parent global task.
