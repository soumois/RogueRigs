---
id: task-209-04-run-won-bp
description: "bp: WBP_AlertObjectives shows the run-won title and subtitle from the text content subsystem when the run is won at level 4."
parent: task-209-run-won-text
kind: task
status: done
---
# run-won-bp

- goal: WBP_AlertObjectives reads the validated run-won title and subtitle (CLAIM STRIPPED / Gone before the paperwork landed.) through the text content subsystem getter of task-209-03 and shows them on the end screen when URRRunTransitionSubsystem IsRunWon is true at level 4; other end screens unchanged.
- agent: bp
- depends_on: task-209-03-screen-texts-code
- inputs: wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-run (vault/reference/tech/systems/world/sys-run.md), sys-text-content (vault/reference/tech/systems/ui/sys-text-content.md), agent-bp (vault/reference/agents/agent-bp.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: Content/RogueRigs/UI/WBP_AlertObjectives, doc wbp-alert-objectives updated.
- done_when: widget compiles with warnings as errors and saves; doc updated; committed.
- editor_state: open
- token_ceiling: 40000
- model: sonnet
- tokens: input 48, cache write 51846, cache read 747173, output 711
- runs: 1
- milestone: M2
- result: WBP_AlertObjectives ShowRunEnd: when bSuccess and IsRunWon, EndTitle from GetScreenTitle(end_run_won) and new EndSubtitle (Roboto Regular 28, collapsed otherwise) from GetScreenSubtitle; empty text keeps the old title; other end screens unchanged (3423ca03). Unseen on screen.

## Relations
- [[task-209-run-won-text]] — parent global task.
