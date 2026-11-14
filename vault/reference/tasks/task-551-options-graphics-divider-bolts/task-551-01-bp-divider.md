---
id: task-551-01-bp-divider
description: "Bp: Options widget, Graphics tab: replace the column divider by the shared bolted bar, bolts on the frame borders as on the pause screen."
parent: task-551-options-graphics-divider-bolts
kind: task
status: in-progress
---
# Options Graphics divider: bolts on the frame borders like the pause screen: bp

- goal: Options widget, Graphics tab: replace the column divider by the shared bolted bar, bolts on the frame borders as on the pause screen.
- agent: bp
- depends_on: none
- inputs: wbp-ui-bolted-bar (vault/reference/tech/blueprints/ui/wbp-ui-bolted-bar.md), ui-shared-pieces (vault/reference/tech/ui-shared-pieces.md), options-screen (vault/reference/gdd/run/options-screen.md), pause-menu (vault/reference/gdd/run/pause-menu.md), umg-layout (vault/reference/tech/conventions/umg-layout.md)
- expected_output: see done_when
- done_when: The Graphics tab divider is an instance of the shared bolted bar, placed so its bolts sit on the top and bottom frame borders like the pause divider; the widget compiles and saves; Display and Quality content is not moved or overlapped; no other tab or screen changed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-14T08:38:10Z
- waiting_for: none
- result: open

## Relations
- [[task-551-options-graphics-divider-bolts]] — parent global task.
