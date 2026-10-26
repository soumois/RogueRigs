---
id: task-319-02-fix-levelup-after-intermission
description: "WBP_XPStatus UnbindXP removes only its own OnXPChanged and OnLevelUp handlers instead of clearing every listener; doc wbp-xp-status updated."
parent: task-319-levelup-choice-after-intermission
kind: task
status: in-progress
---
# Fix: WBP_XPStatus unbinds only its own handlers

- goal: in `WBP_XPStatus` (/Game/RogueRigs/UI/WBP_XPStatus), function `UnbindXP(M)`: replace the ClearDelegate (Unbind all) nodes on the progression component's `OnXPChanged` and `OnLevelUp` with nodes that remove only this widget's handlers (`OnXPChanged_Event_2`, `OnLevelUp_Event_2`, "Unbind Event from"). The C++ listener `URRLevelUpComponent::HandleLevelUp` must stay bound when an old HUD is destroyed. Cause confirmed by the result of task-319-01. Fix at the cause, in the widget (decided by the Planner 2026-10-26: the defect is the Blueprint clearing listeners it does not own; a C++ rebind at level start would leave other C++ listeners exposed; not a design choice, owner not asked). Only this asset. Runs right after step 1 if step 1 reproduced the bug; owner confirmed Q1 "Oui (Recommended)" (2026-10-26): fix in the same task, no capture, no other new test.
- agent: bp
- depends_on: task-319-01-levelup-choice-after-intermission
- inputs: task-319-01-levelup-choice-after-intermission (vault/reference/tasks/task-319-levelup-choice-after-intermission/task-319-01-levelup-choice-after-intermission.md, its result), wbp-xp-status (vault/reference/tech/blueprints/ui/wbp-xp-status.md), sys-progression (vault/reference/tech/systems/core/sys-progression.md), sys-hud-events (vault/reference/tech/systems/ui/sys-hud-events.md), agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: WBP_XPStatus saved with UnbindXP removing only its own two handlers; doc node wbp-xp-status updated (UnbindXP line, verified line)
- done_when: WBP_XPStatus compiles with 0 errors and is saved; the UnbindXP graph read back shows no ClearDelegate node, and one unbind of `OnXPChanged_Event_2` from OnXPChanged and one of `OnLevelUp_Event_2` from OnLevelUp, for a valid miner; BindToMiner and Destruct still call UnbindXP as before; no other asset changed; doc updated; lint clean; committed by explicit path. No capture.
- editor_state: open (MCP server answering on http://127.0.0.1:8000/mcp)
- token_ceiling: 40000
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-319-levelup-choice-after-intermission]] — parent global task.
- [[task-319-01-levelup-choice-after-intermission]] — the reproduction and cause it fixes.
- [[task-319-03-verify-levelup-after-intermission]] — reruns the test on the fixed widget.
