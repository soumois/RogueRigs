---
id: task-318-03-bp-loading-progress
description: "WBP_Loading gets a progress bar bound to the next-level progress and a collapsed image slot for later."
parent: task-318-preload-next-level
kind: task
status: in-progress
---
# WBP_Loading gets a progress bar bound to the next-level progress and a collapsed image slot for later

- goal: WBP_Loading gets a progress bar bound to the next-level progress and a collapsed image slot for later. No new test (owner rule): only fix existing tests the change breaks.
- agent: bp
- depends_on: task-318-02-fade-or-loading
- inputs: wbp-loading (vault/reference/tech/blueprints/ui/wbp-loading.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md)
- expected_output: Content/RogueRigs/UI/WBP_Loading updated; doc wbp-loading updated
- done_when: compiled with warnings as errors and saved; ProgressBar reads GetNextLevelProgress (or the event the C++ exposes), palette of wbp-alert-objectives; Image slot named for the later image, Collapsed, no texture; existing texts kept; doc updated; lint clean.
- editor_state: open
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-318-preload-next-level]] — parent global task.
