---
id: task-551-01-bp-divider
description: "Bp: Options widget, Graphics tab: replace the column divider by the shared bolted bar, bolts on the frame borders as on the pause screen."
parent: task-551-options-graphics-divider-bolts
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-551
- usage: included in the executor run of task-551
- runs: 1
- milestone: M2
- started: 2026-11-14T08:38:10Z
- waiting_for: none
- result: ead4fde1: WBP_Options GraphicsPage child GraphicsDividerV (Image T_LevelEnd_DividerV) replaced by WBP_UI_BoltedBarV of the same name, index 0, HitTestInvisible; slot x point 0.5, y 0.255-0.7991 (frame span), offsets (0,20,34,25), align (0.5,0), the pause values. Compiled and saved through mcp-client.js (RemoveWidget, AddWidget, set_properties). Not referenced by C++ or graph. Seen at 1920x1080: bolts on the top and bottom frame bars, 100% text clear of the bar.

## Relations
- [[task-551-options-graphics-divider-bolts]] — parent global task.
