---
id: task-398-01-backdrop-25
description: "Bp: chosen upgrade screen widget (default WBP_UpgradeChoice): Backdrop black at 25 % opacity"
parent: task-398-upgrade-screen-backdrop
kind: task
status: todo
---
# Upgrade screen: black backdrop at 25 %: bp

- goal: In the widget chosen in Q12 (default WBP_UpgradeChoice; B WBP_ShopScreen; C WBP_Intermission), read the full-screen Backdrop (Border) colour and opacity, report them, set it to black with alpha 0.25 (add a full-screen black Border at the bottom of the Root canvas if none exists); nothing else changes (no animation, no graph change).
- agent: bp
- depends_on: task-391-04-recap-ore-balance
- inputs: agent-bp (vault/reference/agents/agent-bp.md), wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md), wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md), global request, Questions and gdd_check: vault/reference/tasks/task-398-upgrade-screen-backdrop.md
- expected_output: The widget saved, compiled with warnings as errors; commit `task-398-01-backdrop-25: ...`; change summary (before and after values) for the doc subtask.
- done_when: Backdrop read back black, alpha 0.25, full screen; no other property changed; compiled with warnings as errors, no error; saved; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-398-upgrade-screen-backdrop]] — parent global task.
