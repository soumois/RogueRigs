---
id: task-206-05-wbp-loading
description: "bp: WBP_Loading."
parent: task-206-level-to-level
kind: task
status: in-progress
---
# wbp-loading

- goal: WBP_Loading: simple loading screen (dark background, "Chargement" text) bound to the C++ loading events, shown from Continue until OnLevelReady.
- agent: bp
- depends_on: task-206-03-transition-flow
- inputs: intermission (vault/reference/gdd/run/intermission.md), sys-run (vault/reference/tech/systems/world/sys-run.md, written by task-206-01), sys-hud (vault/reference/tech/systems/ui/sys-hud.md), agent-bp (vault/reference/agents/agent-bp.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: Content/RogueRigs/UI/WBP_Loading, doc node wbp-loading in tech/blueprints/ linked from blueprints-index.
- done_when: widget compiles and saves; bound to the C++ events; doc node written; committed.
- editor_state: open
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-206-level-to-level]] — parent global task.
