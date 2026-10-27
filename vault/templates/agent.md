---
id: agent-<name>
description: "<one sentence: what the agent produces and when to invoke it>"
parent: agents-index
kind: spec
status: open
---
# <Agent name>

- role: <one job>
- invoked_by: orchestrator | owner
- trigger: <when it runs>
- reads: <vault node ids it must load, nothing else>
- produces: <what>
- output_format: <exact format, path, schema>
- definition_of_done: <checks before the task is closed>
- must_not: <explicit limits>
- token_ceiling: <x> M units per task — bound on the weighted units of one run (see the task template)

## Relations
- [[agents-index]] — parent index.
