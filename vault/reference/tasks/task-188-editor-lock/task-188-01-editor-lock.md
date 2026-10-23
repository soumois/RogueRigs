---
id: task-188-01-editor-lock
description: "Editor lock shared by sessions: lock file, take and release script, runbook and protocol rules."
parent: task-188-editor-lock
kind: task
status: blocked
---
# Editor lock between sessions

- goal: Editor lock shared by sessions: a lock file, a node script to take, release and read it, and the runbook and protocol rules to use it.
- agent: claude
- depends_on: none
- inputs: orchestrator-runbook (vault/reference/agents/orchestrator-runbook.md), orchestration-protocol (vault/reference/agents/orchestration-protocol.md), orchestrator-script-quirks (vault/reference/agents/orchestrator-script-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: Saved/EditorLock.json (not in git: session, task, need open|closed, since); vault/scripts/editor-lock.js take|release|status (refuses a take held by another session, reports locks older than 2 h as stale); the runbook and orchestration-protocol (rule node, owner approval required) say: take before launching, closing, building or running -game, release when the subtask no longer needs the editor, message the other open sessions on take and release; optional PreToolUse hook to block bp/cpp delegation while another session holds the lock (owner to choose)
- done_when: Script works for take, status, release and stale; docs updated; lint clean; committed by path
- editor_state: any
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-188-editor-lock]] — parent global task.
