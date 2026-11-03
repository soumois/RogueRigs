---
id: task-424-02-pause-menu-elements
description: "Art: retrieve the screen elements from ChatGPT (asset sheet), map reuse versus new, make the new PNGs"
parent: task-424-pause-menu-stats
kind: task
status: todo
---
# Pause menu: miner stats screen with Resume and Exit: art

- goal: Retrieve the elements of the validated mockup from ChatGPT and turn the new ones into UI PNGs, reusing existing sprites where they match.
- agent: art
- depends_on: task-424-01-pause-menu-chatgpt-mockup
- inputs: task-424-pause-menu-stats (vault/reference/tasks/task-424-pause-menu-stats.md) (Questions Q6), `vault/reference/gdd/art-audio/art-prompts-pause-menu-mockup.md` (new, written by subtask 01), art-prompts-level-end-screen-mockup (vault/reference/gdd/art-audio/art-prompts-level-end-screen-mockup.md), art-prompts-shop-screen-mockup (vault/reference/gdd/art-audio/art-prompts-shop-screen-mockup.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), agent-art (vault/reference/agents/agent-art.md), umg-layout (vault/reference/tech/conventions/umg-layout.md)
- expected_output: ChatGPT asset sheet saved in `Art/Source/pause-menu/reference/`; in the mockup node an Elements section mapping every mockup element to a reused texture (path) or a new sprite; new sprites as RGBA PNGs in `Art/UI/PauseMenu/` (`T_Pause_*.png`), frames in 3x3 with measured Draw As Box margins (px and fraction), cut from the sheet when its resolution suffices, otherwise Kie.ai GPT Image 2 image-to-image from the sheet and the validated mockup (2 variants, 6 credits each, 36 credits at most); prompts, run logs and review files in `Art/Source/pause-menu/`.
- done_when: Every mockup element is mapped (reuse or new file); each new PNG exists with its margins in the node; credits spent within 36 and recorded; no unlabelled file left untracked; lint clean; committed by path.
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
