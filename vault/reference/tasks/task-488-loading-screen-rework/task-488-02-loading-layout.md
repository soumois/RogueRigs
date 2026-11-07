---
id: task-488-02-loading-layout
description: "Bp: WBP_Loading rebuilt to the intermission look: image, bottom gradient, riveted frame holding the texts and the restyled bar or spinner."
parent: task-488-loading-screen-rework
kind: task
status: todo
---
# Richer loading screen (illustration and framed texts): bp

- goal: WBP_Loading rebuilt to the intermission look: image, bottom gradient, riveted frame holding the texts and the restyled bar or spinner.
- agent: bp
- depends_on: task-488-01-loading-image
- inputs: intermission (vault/reference/gdd/run/intermission.md), wbp-loading (vault/reference/tech/blueprints/ui/wbp-loading.md), wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md), wbp-run-lost (vault/reference/tech/blueprints/ui/wbp-run-lost.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), sys-run-transition-fade (vault/reference/tech/systems/world/sys-run/sys-run-transition-fade.md)
- expected_output: see done_when
- done_when: The widget shows the image full screen, the gradient and the riveted frame at bottom left with the three texts, the bar as an orange fill in a dark riveted track and the spinner in the frame when progress is not measured; events, texts lookup, progress, spinner and Collapsed logic unchanged; compiled with warnings as errors and saved.
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
- [[task-488-loading-screen-rework]] — parent global task.
