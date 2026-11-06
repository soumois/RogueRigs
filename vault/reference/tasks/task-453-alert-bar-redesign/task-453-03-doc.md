---
id: task-453-03-doc
description: "Doc: Docs describe the new look."
parent: task-453-alert-bar-redesign
kind: task
status: done
---
# Alert bar redesigned and better integrated: doc

- goal: Docs describe the new look.
- agent: doc
- depends_on: task-453-02-alert-bar
- inputs: wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), blueprints-ui-index (vault/reference/tech/blueprints/ui/blueprints-ui-index.md)
- expected_output: see done_when
- done_when: Anchors listed per umg-layout; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-453
- usage: included in the executor run of task-453
- runs: 1
- milestone: M2
- started: 2026-11-06T00:55:33Z
- waiting_for: none
- result: Commit 851779a5. wbp-alert-objectives updated: components (AlertBar dark track, AlertFrame, C/B/S font 28, horde marks as T_HUD_AlertMark images), layout table (anchors 0.5,0, alignment, offsets, size mode, z per widget), PlaceAlertIcon Widget param, HideForOffer/RestoreAfterOffer/HideForEnd frame handling, assets, verified, relation to art-prompts-hud-unified-mockup (both ends); blueprints-ui-index gloss. Lint clean (pre-existing warnings only).

## Relations
- [[task-453-alert-bar-redesign]] — parent global task.
