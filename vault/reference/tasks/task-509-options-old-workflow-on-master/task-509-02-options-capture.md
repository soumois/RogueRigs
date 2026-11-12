---
id: task-509-02-options-capture
description: "Capture: capture the merged Options screen from main menu and pause at the four resolutions"
parent: task-509-options-old-workflow-on-master
kind: task
status: in-progress
---
# Écran Options : version de l'ancien workflow sur master, puis push: capture

- goal: capture the merged Options screen from main menu and pause at the four resolutions
- agent: capture
- depends_on: task-509-01-merge-old-options
- inputs: umg-layout (vault/reference/tech/conventions/umg-layout.md), wbp-options (vault/reference/tech/blueprints/ui/wbp-options.md), C:/Workspace/RogueRigs-old (task-500-10 captures, for comparison)
- expected_output: see done_when
- done_when: C1 Options opens from the main menu and shows the old-version layout and images; C2 Options opens from the pause menu and returning resumes the pause menu; C3 the four resolutions show no clipped or overlapping element; C4 the frames match the old-version captures; frames sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-12T12:33:00Z
- waiting_for: none
- result: open

## Relations
- [[task-509-options-old-workflow-on-master]] — parent global task.
