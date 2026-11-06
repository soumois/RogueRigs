---
id: task-457-04-doc
description: "Doc: Docs describe the framed radar and the ore count place."
parent: task-457-radar-frame-ore
kind: task
status: done
---
# Radar frame holding the ore count: doc

- goal: Docs describe the framed radar and the ore count place.
- agent: doc
- depends_on: task-457-03-ore-in-frame
- inputs: wbp-minimap-locator (vault/reference/tech/blueprints/ui/wbp-minimap-locator.md), wbp-ore-counter (vault/reference/tech/blueprints/ui/wbp-ore-counter.md), wbp-hud (vault/reference/tech/blueprints/ui/wbp-hud.md), blueprints-ui-index (vault/reference/tech/blueprints/ui/blueprints-ui-index.md)
- expected_output: see done_when
- done_when: Anchors listed per umg-layout; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-457
- usage: included in the executor run of task-457
- runs: 1
- milestone: M2
- started: 2026-11-06T00:38:57Z
- waiting_for: none
- result: Commit 22cd9f29. Updated wbp-minimap-locator (layout table: anchors 0,0, alignments, offsets, z of RadarFrame, MinimapImage, markers, locator; T_HUD_RadarFrame asset; verified), wbp-ore-counter (purpose, layout of OreIcon and OreText in the plate, verified), wbp-hud (task-457 layout check), blueprints-ui-index glosses, art-prompts-hud-unified-mockup back relation. Lint clean (pre-existing warnings only).

## Relations
- [[task-457-radar-frame-ore]] — parent global task.
