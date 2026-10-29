---
id: task-360-01-capture-agent-definition
description: "Doc: write the capture agent definition and its vault node, move capture rules out of agent-cpp, route visual checks to it in the crew nodes."
parent: task-360-capture-agent
kind: task
status: done
---
# Capture agent: definition

- goal: Create `.claude/agents/capture.md` (frontmatter like `.claude/agents/doc.md`, values of Q7) and `vault/reference/agents/agent-capture.md` (from `vault/templates/agent.md`, Q7 and Q8, report format per checklist point); move `capture_script`, `capture_rules` and `visual_check` from agent-cpp to agent-capture (cpp keeps stress and tests); route visual checks to `capture` in the listed nodes (planner: checklist in done_when per Q8). Decided values: global `## Questions` and `gdd_check`.
- agent: doc
- depends_on: none
- inputs: global task-360-capture-agent (vault/reference/tasks/task-360-capture-agent.md), agent-cpp (vault/reference/agents/agent-cpp.md), `.claude/agents/cpp.md`, `.claude/agents/doc.md`, `vault/templates/agent.md`, agents-index (vault/reference/agents/agents-index.md), agent-executor (vault/reference/agents/agent-executor.md), `.claude/agents/executor.md`, agent-planner (vault/reference/agents/agent-planner.md), orchestration-protocol (vault/reference/agents/orchestration-protocol.md) (step 3), orchestrator-lessons (vault/reference/agents/orchestrator-lessons.md) (Showing and verifying), tool-capture (vault/reference/tech/pipelines/tool-capture.md, created by task-359-02), _authoring (vault/_authoring.md)
- expected_output: the two new files, the listed nodes and files updated, one commit.
- done_when: agent-capture and capture.md state the same model, tools, turns and ceiling; agent-cpp no longer holds the capture fields and links agent-capture; agents-index lists agent-capture; agent-executor `agents` includes capture; agent-planner and orchestration-protocol step 3 describe the C1..Cn checklist; orchestrator-lessons says visual checks go to capture; lint clean; committed.
- editor_state: any
- token_ceiling: 0.4 M units
- model: sonnet
- tokens: input 24, cache write 41207, cache read 337104, output 12098
- usage: turns 12, context start 11k peak 46k, resumes 0, 2 min, units 0.15 M
- runs: 1
- milestone: M2
- started: 2026-10-29T14:25:15Z
- waiting_for: none
- result: capture agent defined (capture.md, agent-capture), capture rules moved from agent-cpp, routing updated; commits 04937f68, 4467bdee

## Relations
- [[task-360-capture-agent]] — parent global task.
