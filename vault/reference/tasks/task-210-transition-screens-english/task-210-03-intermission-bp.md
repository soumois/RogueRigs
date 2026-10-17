---
id: task-210-03-intermission-bp
description: "bp: WBP_Intermission uses the validated English texts."
parent: task-210-transition-screens-english
kind: task
status: in-progress
---
# intermission-bp

- goal: WBP_Intermission uses the validated English texts.
- agent: bp
- depends_on: task-210-02-transition-review
- inputs: wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-text-content (vault/reference/tech/systems/ui/sys-text-content.md), agent-bp (vault/reference/agents/agent-bp.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: Content/RogueRigs/UI/WBP_Intermission, doc wbp-intermission updated.
- done_when: widget compiles and saves; doc updated; committed.
- editor_state: open
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-210-transition-screens-english]] — parent global task.
