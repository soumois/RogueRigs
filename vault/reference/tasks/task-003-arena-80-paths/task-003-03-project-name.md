---
id: task-003-03-project-name
description: "Rename the project display name from the template's Top Down Game Template to Rogue Rigs in DefaultGame.ini."
parent: task-003-arena-80-paths
kind: task
status: in-progress
---
# Project name

- goal: the project no longer carries the template's name.
- agent: cpp (config only: `Config/DefaultGame.ini`, the `ProjectName` line; no C++).
- depends_on: none
- inputs: `Config/DefaultGame.ini`, [[build-toolchain]].
- expected_output: `ProjectName=Rogue Rigs` under `[/Script/EngineSettings.GeneralProjectSettings]`, CRLF line endings kept, no other line changed. Report any other template name left in `Config/` or the `.uproject` (report only).
- done_when: `git diff Config/DefaultGame.ini` shows only that line; lint clean.
- editor_state: any
- token_ceiling: 10000
- milestone: M1
- result: open

## Relations
- [[task-003-arena-80-paths]] — parent global task.
