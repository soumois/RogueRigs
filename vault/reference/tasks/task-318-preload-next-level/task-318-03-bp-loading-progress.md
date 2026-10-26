---
id: task-318-03-bp-loading-progress
description: "WBP_Loading gets a progress bar bound to the next-level progress and a collapsed image slot for later."
parent: task-318-preload-next-level
kind: task
status: done
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
- model: sonnet
- tokens: input 44, cache write 59483, cache read 841586, output 11063
- runs: 1
- milestone: M2
- result: WBP_Loading: LoadingBarBox (600x24) with LoadingBar ProgressBar (fill 1/0.35/0.1, AlertBar orange) set by OnLoadingProgressChanged; LoadingImage Collapsed, no texture; texts kept; compiled warnings as errors, saved; doc wbp-loading updated. Not seen on screen. Commit 57c2892e.

## Relations
- [[task-318-preload-next-level]] — parent global task.
