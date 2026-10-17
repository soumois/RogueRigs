---
id: task-209-03-run-won-bp
description: "bp: WBP_AlertObjectives shows the validated run-won text on the end screen when URRRunTransitionSubsystem IsRunWon is true at level 4"
parent: task-209-run-won-text
kind: task
status: todo
---
# run-won-bp

- goal: WBP_AlertObjectives shows the validated run-won text on the end screen when URRRunTransitionSubsystem IsRunWon is true at level 4; other end screens unchanged.
- agent: bp
- depends_on: task-209-02-run-won-review
- inputs: wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-run (vault/reference/tech/systems/world/sys-run.md), sys-text-content (vault/reference/tech/systems/ui/sys-text-content.md), agent-bp (vault/reference/agents/agent-bp.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: Content/RogueRigs/UI/WBP_AlertObjectives, doc wbp-alert-objectives updated.
- done_when: widget compiles with warnings as errors and saves; doc updated; committed.
- editor_state: open
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-209-run-won-text]] — parent global task.
