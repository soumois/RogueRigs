---
id: task-467-07-doc
description: "Doc: Docs describe the reworked bottom HUD and its sprites."
parent: task-467-bottom-hud-match-target
kind: task
status: done
---
# Bottom HUD reworked in a capture loop until it matches the target image: doc

- goal: Docs describe the reworked bottom HUD and its sprites.
- agent: doc
- depends_on: task-467-06-capture
- inputs: wbp-health-status (vault/reference/tech/blueprints/ui/wbp-health-status.md), wbp-weapons (vault/reference/tech/blueprints/ui/wbp-weapons.md), wbp-xp-status (vault/reference/tech/blueprints/ui/wbp-xp-status.md), wbp-hud (vault/reference/tech/blueprints/ui/wbp-hud.md), art-prompts-hud-unified-mockup (vault/reference/gdd/art-audio/art-prompts-hud-unified-mockup.md)
- expected_output: see done_when
- done_when: The widget nodes list, for each changed top-level widget, anchors, alignment, offsets and size mode, and name the new sprites; art-prompts-hud-unified-mockup says which 461 sprites are replaced; lint clean; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-467
- usage: included in the executor run of task-467
- runs: 1
- milestone: M2
- started: 2026-11-06T10:21:06Z
- waiting_for: none
- result: wbp-health-status (HealthPanel 730 x 169 at 0.5,1 / -62, BlockCanvas with four canvas slots, sprites, why canvas, Box-border pixel-size finding), wbp-weapons (flat cells T_HUD_SlotCell, 162 x 96, centred content), wbp-xp-status (table: XPBarArea, ticks, XPFrame, LevelCap, LevelText offsets; rail and plate sprites), wbp-hud (Weapons -76, task-467 layout check seen at 1920 x 1080) updated; art-prompts-hud-unified-mockup lists which 461 sprites are replaced. Lint clean. Commit 7771aaff.

## Relations
- [[task-467-bottom-hud-match-target]] — parent global task.
