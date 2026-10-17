---
id: task-210-03-intermission-bp
description: "bp: WBP_Intermission uses the validated English texts."
parent: task-210-transition-screens-english
kind: task
status: done
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
- model: sonnet
- tokens: input 74, cache write 65652, cache read 1375149, output 1035
- runs: 1
- milestone: M2
- result: WBP_Intermission OnRecapShown reads intermission_recap via GetScreenText, literal replaces of {N}, {NEXT}, {TOTAL}, {ORE}; English tree defaults kept when the text is missing; ShopLabel and ContinueText made variables (8a5dda44). Unseen on screen.

## Relations
- [[task-210-transition-screens-english]] — parent global task.
