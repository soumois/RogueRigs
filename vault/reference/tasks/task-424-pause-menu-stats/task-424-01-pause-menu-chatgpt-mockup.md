---
id: task-424-01-pause-menu-chatgpt-mockup
description: "Art: reference image + French prompt, ChatGPT mockup in the browser, owner validation gate"
parent: task-424-pause-menu-stats
kind: task
status: todo
---
# Pause menu: miner stats screen with Resume and Exit: art

- goal: Get from ChatGPT, in the browser, a mockup of the pause menu in the existing menu style, and have the owner validate it.
- agent: art
- depends_on: none
- inputs: task-424-pause-menu-stats (vault/reference/tasks/task-424-pause-menu-stats.md) (request, Questions), pause-menu (vault/reference/gdd/run/pause-menu.md), player-stats (vault/reference/gdd/player/player-stats.md), art-prompts-level-end-screen-mockup (vault/reference/gdd/art-audio/art-prompts-level-end-screen-mockup.md), art-prompts-level-end-screen (vault/reference/gdd/art-audio/art-prompts-level-end-screen.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), agent-art (vault/reference/agents/agent-art.md), reference image `Art/Source/level-end-screen/reference/chatgpt-level-end-mockup-2026-10-30.webp`
- expected_output: Prompt `Art/Source/pause-menu/prompts/chatgpt-pause-menu.md` (French: the reference image attached, Q3 content with plausible sample values, two buttons Resume and Exit, same frame, title bars, button and colours as the reference, 16:9 full screen over a dimmed paused arena); every ChatGPT image saved as received in `Art/Source/pause-menu/reference/` (labelled keep or temporary per AGENTS.md); node `vault/reference/gdd/art-audio/art-prompts-pause-menu-mockup.md` (new, written by subtask 01) under art-prompts (files, contents, round history); no Kie.ai credit spent.
- done_when: Prompt and mockup files exist and are listed in the new node; the Executor has shown the mockup to the owner (status wait-answers until his answer); any owner remark gave one more ChatGPT round in the same conversation; the owner validated a mockup and his words and the validated file path are in this subtask result and in the node; if the agent has no browser access, the Executor stopped with the prompt and the image for the owner (Q4) and the brought-back image is saved the same way; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.15 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-424-pause-menu-stats]] — parent global task.
