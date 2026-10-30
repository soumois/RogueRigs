---
id: task-402-02-doc-map-path-msys
description: "Doc: tool-capture, sys-stress, agent-capture, orchestrator-script-quirks and .claude/agents/capture.md state the repair and the MSYS_NO_PATHCONV=1 rule"
parent: task-402-capture-map-path-msys
kind: task
status: in-progress
---
# Game launches: /Game map path rewritten by Git Bash: doc

- goal: Document the fix of step 1 and the Bash rule: tool-capture (`--map` repair line), sys-stress (Run-StressTest.ps1 `-Map` repair), agent-capture (`capture_script`: always through capture.js; `visual_check`: no raw UnrealEditor.exe launch from Bash, else prefix `MSYS_NO_PATHCONV=1`), orchestrator-script-quirks `## Shells` (one bullet: Git Bash rewrites an argument starting with `/` into `C:/Program Files/Git/...`; use `MSYS_NO_PATHCONV=1` or the helpers), one line in `.claude/agents/capture.md` step 2.
- agent: doc
- depends_on: task-402-01-map-path-repair
- inputs: agent-doc (vault/reference/agents/agent-doc.md), _authoring (vault/_authoring.md), tool-capture (vault/reference/tech/pipelines/tool-capture.md), sys-stress (vault/reference/tech/systems/tools/sys-stress.md), agent-capture (vault/reference/agents/agent-capture.md), orchestrator-script-quirks (vault/reference/agents/orchestrator-script-quirks.md), `.claude/agents/capture.md`, task-402-capture-map-path-msys (vault/reference/tasks/task-402-capture-map-path-msys.md) (request, gdd_check, Q answers), Brief: commit hash and change summary of task-402-01-map-path-repair
- expected_output: The five files updated, lint clean, one commit `task-402-02-doc-map-path-msys: ...`.
- done_when: tool-capture and sys-stress describe the repair and its printed line; agent-capture `visual_check` no longer shows a raw UnrealEditor.exe launch without `MSYS_NO_PATHCONV=1` and `capture_script` says every capture goes through capture.js; orchestrator-script-quirks `## Shells` has the MSYS bullet; .claude/agents/capture.md step 2 mentions it; `node vault/scripts/lint.js --write` reports 0 problems; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-30T23:52:58Z
- waiting_for: none
- result: open

## Relations
- [[task-402-capture-map-path-msys]] — parent global task.
