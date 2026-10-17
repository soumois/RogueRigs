---
id: task-209-03-screen-texts-code
description: "cpp: text validator and text content loader accept a screen texts file; Blueprint getter for screen titles and lines."
parent: task-209-run-won-text
kind: task
status: in-progress
---
# screen-texts-code

- goal: Tools/content/validate_text.js and URRTextContentSubsystem accept a fifth content file for screen texts (end screens now, intermission and loading screens in task-210): rename Content/RogueRigs/Data/Text/end_screens.json to screen_texts.json if the name is better, keep item end_run_won (CLAIM STRIPPED / Gone before the paperwork landed.), define its item schema, and expose a BlueprintCallable getter (by id) the widgets use; the text pipeline docs list the new file.
- agent: cpp
- depends_on: task-209-02-run-won-review
- inputs: sys-text-content (vault/reference/tech/systems/ui/sys-text-content.md), text-content-schema (vault/reference/tech/pipelines/text-content-schema.md), text-content-files (vault/reference/tech/pipelines/text-content-files.md), text-content-events (vault/reference/tech/pipelines/text-content-events.md), Tools/content/validate_text.js, Content/RogueRigs/Data/Text/end_screens.json, agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: validator and C++ loader changes, the screen texts file, docs sys-text-content, text-content-schema and text-content-files updated.
- done_when: node Tools/content/validate_text.js exits 0 on every text file; build succeeds; existing tests pass except the 2 known pre-existing failures; a -game log shows the getter returning end_run_won; docs updated; committed.
- editor_state: closed
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-209-run-won-text]] — parent global task.
