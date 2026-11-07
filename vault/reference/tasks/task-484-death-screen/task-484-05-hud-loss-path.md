---
id: task-484-05-hud-loss-path
description: "Bp: HUD side of a loss: HUD hidden, old GAME OVER text and RUN LOST / Play again panel no longer shown; run-won and level-success end screens unchanged."
parent: task-484-death-screen
kind: task
status: todo
---
# Death screen with Retry and Main Menu: bp

- goal: HUD side of a loss: HUD hidden, old GAME OVER text and RUN LOST / Play again panel no longer shown; run-won and level-success end screens unchanged.
- agent: bp
- depends_on: task-484-04-death-widget
- inputs: wbp-hud (vault/reference/tech/blueprints/ui/wbp-hud.md), wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), wbp-health-status (vault/reference/tech/blueprints/ui/wbp-health-status.md)
- expected_output: see done_when
- done_when: In PIE a death and a missed shuttle show only the new screen (no GAME OVER text, no RUN LOST panel, HUD hidden); a level extraction and the run-won screen still show their panel and buttons; compiled with warnings as errors, saved, no script error.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-484-death-screen]] — parent global task.
