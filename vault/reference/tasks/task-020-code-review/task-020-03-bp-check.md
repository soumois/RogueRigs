---
id: task-020-03-bp-check
description: "Open the editor, check every Blueprint compiles and loads without broken references, run a PIE smoke test of the full loop."
parent: task-020-code-review
kind: task
status: todo
---
# Blueprint check after refactor

- goal: the refactor broke nothing in the editor.
- agent: bp
- depends_on: task-020-02-refactor
- inputs: [[unreal-mcp]], [[sys-cheats]], [[blueprints-index]], task-020-02 result.
- expected_output: compile every Blueprint under `/Game/RogueRigs` and report errors or warnings; output log scanned for redirector or missing-class errors at editor start; PIE smoke test with the cheat property route (level-up offer, console, shuttle) and no log error; fixes only if trivial (re-save, re-link), otherwise report precisely.
- done_when: all Blueprints compile; PIE smoke test passes with no error; lint clean.
- editor_state: open
- token_ceiling: 90000
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-020-code-review]] — parent global task.
