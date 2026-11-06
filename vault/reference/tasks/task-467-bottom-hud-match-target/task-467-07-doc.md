---
id: task-467-07-doc
description: "Doc: Docs describe the reworked bottom HUD and its sprites."
parent: task-467-bottom-hud-match-target
kind: task
status: in-progress
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-06T10:21:06Z
- waiting_for: none
- result: open

## Relations
- [[task-467-bottom-hud-match-target]] — parent global task.
