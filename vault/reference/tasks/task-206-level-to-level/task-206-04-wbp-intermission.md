---
id: task-206-04-wbp-intermission
description: "bp: WBP_Intermission."
parent: task-206-level-to-level
kind: task
status: todo
---
# wbp-intermission

- goal: WBP_Intermission: placeholder recap screen bound to the C++ intermission events: title "Niveau N terminé", ore secured this level and run total, an empty shop area reserved, a Continue button calling the C++ entry point.
- agent: bp
- depends_on: task-206-03-transition-flow
- inputs: intermission (vault/reference/gdd/run/intermission.md), run-shop (vault/reference/gdd/run/run-shop.md), sys-run (vault/reference/tech/systems/world/sys-run.md, written by task-206-01), sys-hud (vault/reference/tech/systems/ui/sys-hud.md), wbp-hud (vault/reference/tech/blueprints/ui/wbp-hud.md), hud-refactor-audit (vault/reference/tech/blueprints/ui/hud-refactor-audit.md), agent-bp (vault/reference/agents/agent-bp.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: Content/RogueRigs/UI/WBP_Intermission, doc node wbp-intermission in tech/blueprints/ linked from blueprints-index.
- done_when: widget compiles and saves; bound to the C++ events listed in sys-run; doc node written; committed.
- editor_state: open
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-206-level-to-level]] — parent global task.
