---
id: task-509-01-merge-old-options
description: "Executor: bring the old-workflow options screen onto master, remove the new-workflow one, renumber the old task to 510"
parent: task-509-options-old-workflow-on-master
kind: task
status: todo
---
# Écran Options : version de l'ancien workflow sur master, puis push: executor

- goal: Code, Blueprints, assets, images and task nodes of the old version on master in place of the new version, nothing else of master lost.
- agent: executor
- depends_on: none
- inputs: task-504-options-screen-look (vault/reference/tasks/task-504-options-screen-look.md), task-505-options-screen-sound-graphics (vault/reference/tasks/task-505-options-screen-sound-graphics.md), task-506-options-gameplay-and-pause (vault/reference/tasks/task-506-options-gameplay-and-pause.md), options-menu (vault/reference/gdd/run/options-menu.md), C:/Workspace/RogueRigs-old (branch exp/old-workflow-options, e0968216, base c77dc729)
- expected_output: see done_when
- done_when: A1, A2 (code and assets), A3 and A4 hold; editor build succeeds, automation tests pass, the game starts without the audio crash (A5); committed.
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
- [[task-509-options-old-workflow-on-master]] — parent global task.
