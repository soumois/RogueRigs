---
id: task-210-04-loading-bp
description: "bp: WBP_Loading uses the validated English texts."
parent: task-210-transition-screens-english
kind: task
status: done
---
# loading-bp

- goal: WBP_Loading uses the validated English texts.
- agent: bp
- depends_on: task-210-02-transition-review
- inputs: wbp-loading (vault/reference/tech/blueprints/ui/wbp-loading.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-text-content (vault/reference/tech/systems/ui/sys-text-content.md), agent-bp (vault/reference/agents/agent-bp.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: Content/RogueRigs/UI/WBP_Loading, doc wbp-loading updated.
- done_when: widget compiles and saves; doc updated; committed.
- editor_state: open
- token_ceiling: 30000
- model: sonnet
- tokens: input 70, cache write 69491, cache read 1491941, output 974
- runs: 1
- milestone: M2
- result: WBP_Loading LoadScreenTexts reads loading_level via GetScreenText from its world; OnLoadingShown sets DROPPING IN and Level {N}; OnLoadingStageChanged sets Lines[0..2]; English tree defaults kept when the text is missing (e102048a). Unseen on screen.

## Relations
- [[task-210-transition-screens-english]] — parent global task.
