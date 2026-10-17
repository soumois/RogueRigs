---
id: task-206-05-wbp-loading
description: "bp: WBP_Loading."
parent: task-206-level-to-level
kind: task
status: done
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
- model: sonnet
- tokens: input 64, cache write 61211, cache read 1314435, output 1060
- runs: 1
- milestone: M2
- result: WBP_Loading (7976bfa6): opaque dark backdrop, Chargement, Niveau N from OnLoadingShown, stage line from OnLoadingStageChanged (Préparation du terrain, Chargement de la carte, Préparation du niveau); LoadingScreenWidgetClass set on BP_RRPlayerController; doc wbp-loading under blueprints-ui-index. Unseen on screen.

## Relations
- [[task-206-level-to-level]] — parent global task.
