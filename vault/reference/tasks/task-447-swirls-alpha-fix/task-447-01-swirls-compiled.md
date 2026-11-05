---
id: task-447-01-swirls-compiled
description: "Bp: The compiled Swirls emitter gives particles the alpha the asset declares."
parent: task-447-swirls-alpha-fix
kind: task
status: todo
---
# Fix the invisible extractor swirls: bp

- goal: The compiled Swirls emitter gives particles the alpha the asset declares.
- agent: bp
- depends_on: none
- inputs: task-442-extractor-wash-no-rain (vault/reference/tasks/task-442-extractor-wash-no-rain.md), task-443-extractor-ground-blast (vault/reference/tasks/task-443-extractor-ground-blast.md), bp-extractor-rotor-wash (vault/reference/tech/blueprints/fx/bp-extractor-rotor-wash.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: see done_when
- done_when: Particle dump in PIE: Swirls alpha above 0 and equal to the expression value at the current Intensity; GroundBlast values unchanged; no LogNiagara error; dump figures given in the result.
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
- [[task-447-swirls-alpha-fix]] — parent global task.
