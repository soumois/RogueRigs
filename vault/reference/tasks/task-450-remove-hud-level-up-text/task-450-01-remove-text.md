---
id: task-450-01-remove-text
description: "Bp: WBP_XPStatus no longer has the LEVEL UP text or its timer; the XP bar and level text stay."
parent: task-450-remove-hud-level-up-text
kind: task
status: in-progress
---
# Remove the cyan LEVEL UP text from the HUD: bp

- goal: WBP_XPStatus no longer has the LEVEL UP text or its timer; the XP bar and level text stay.
- agent: bp
- depends_on: none
- inputs: wbp-xp-status (vault/reference/tech/blueprints/ui/wbp-xp-status.md), wbp-hud (vault/reference/tech/blueprints/ui/wbp-hud.md), sys-hud (vault/reference/tech/systems/ui/sys-hud.md), Art/Source/level-up-fx/20261105-owner-two-level-up-texts.webp
- expected_output: see done_when
- done_when: Compiled and saved with no error; the text, its timer and its hide function are gone; a level-up in PIE logs no runtime error; XP and level still update.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-05T22:51:16Z
- waiting_for: none
- result: open

## Relations
- [[task-450-remove-hud-level-up-text]] — parent global task.
