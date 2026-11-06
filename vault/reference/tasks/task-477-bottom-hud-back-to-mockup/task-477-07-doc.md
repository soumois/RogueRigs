---
id: task-477-07-doc
description: "Doc: Docs describe the bottom HUD pieces and layout."
parent: task-477-bottom-hud-back-to-mockup
kind: task
status: done
---
# Bottom HUD redone from the art proposal, up to 6 correction rounds: doc

- goal: Docs describe the bottom HUD pieces and layout.
- agent: doc
- depends_on: task-477-06-capture
- inputs: wbp-health-status (vault/reference/tech/blueprints/ui/wbp-health-status.md), wbp-weapons (vault/reference/tech/blueprints/ui/wbp-weapons.md), wbp-xp-status (vault/reference/tech/blueprints/ui/wbp-xp-status.md), art-prompts-hud-unified-mockup (vault/reference/gdd/art-audio/art-prompts-hud-unified-mockup.md)
- expected_output: see done_when
- done_when: The three wbp nodes and the art node describe the new pieces and what replaced the task-467 sprites; lint clean; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-477
- usage: included in the executor run of task-477
- runs: 1
- milestone: M2
- started: 2026-11-06T19:57:29Z
- waiting_for: none
- result: wbp-health-status, wbp-weapons, wbp-xp-status: new current layout entries (task-477 pieces, slots, sizes, fonts, colours; 467 entries marked superseded; assets lists say which 467 and 461 sprites are no longer used); wbp-hud: Weapons offset -72 and the HealthStatus instance FillColorStops override trap; art-prompts-hud-unified-mockup already holds the cut pieces and what they replace (task-477-01). Lint clean (warnings only, pre-existing). Commit a73436c6.

## Relations
- [[task-477-bottom-hud-back-to-mockup]] — parent global task.
