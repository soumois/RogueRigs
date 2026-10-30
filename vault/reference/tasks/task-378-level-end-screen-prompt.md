---
id: task-378-level-end-screen-prompt
description: "Global task: Write the ChatGPT image prompt of the end-of-level stats screen mockup, in the palette and look of the Spare Parts cards screen."
parent: tasks-to-do-index
kind: task
status: todo
---
# Level-end screen prompt for ChatGPT

- request: Owner, 2026-10-30, verbatim: "Il me faudrait un prompt pour l'écran de fin d'un niveau, sachant que je [veux] la palette de l'écran des cartes entre les niveaux et aussi l'écran de carte entre les niveaux." Meaning (Planner reading, to confirm in Q1): the end-of-level screen is the stats screen shown after each extraction (intermission screen 1, WBP_Intermission); "l'écran des cartes" and "l'écran de carte" both name the Spare Parts cards screen between levels (WBP_ShopScreen, redesigned in task-348): its palette and its look are the reference. Only a prompt is asked: no image generation, no widget change. Owner answer 2026-10-30 (relayed by the main session, verbatim): "Non quand je parle de l'écran des cartes, c'est l'écran où on peut acheter des améliorations entre les niveaux. Je vais donner les screens de cet écran comme référence. Là on va travailler l'écran qui apparait lorsqu'on atteint l'extrateur." Meaning: Q1 decided. "L'écran des cartes" = the screen where upgrades are bought between levels (Spare Parts, WBP_ShopScreen); "l'écran de carte" is not a separate map screen. The screen to work on = the one shown when the miner reaches the extractor (end-of-level stats screen, WBP_Intermission, shown after a successful extraction). The palette and look references are screenshots of the Spare Parts screen that the owner will give himself; they are saved in `Art/Source/level-end-screen/reference/` before the art subtask runs. Whether the ChatGPT mockup file is attached as well is not assumed (Q2). Q2 to Q6 not answered yet. Owner answer 2026-10-30 (relayed by the main session, verbatim): "et ok pour le reste" = the plan and the defaults of Q2, Q3, Q4, Q5, Q6 accepted. Owner correction of Q2 2026-10-30 (relayed by the main session, verbatim): "J'ai juste besoin du prompt, je vais utiliser le mockup que tu m'as proposé plus haut" = the only image attached is the ChatGPT mockup `Art/Source/shop-screen/reference/chatgpt-shop-mockup-2026-10-28.webp` as the palette and look reference; no owner screenshots are expected and `Art/Source/level-end-screen/reference/` is no longer a prerequisite. The rest is unchanged.
- goal: A prompt node art-prompts-level-end-screen, child of art-prompts, points to a prompt file in the repo that the owner pastes into ChatGPT, with the image to attach (the ChatGPT Spare Parts mockup chatgpt-shop-mockup-2026-10-28.webp), to get a 16:9 mockup of the screen shown when the miner reaches the extractor (end-of-level stats screen) in the palette and look of the Spare Parts upgrade shop screen.
- acceptance: Art/Source/level-end-screen/prompts/chatgpt-level-end-mockup.md exists with the prompt verbatim (language per Q3), the attachments to give ChatGPT (per Q2) and the content of the stats screen as in intermission and wbp-intermission (title LEVEL N CLEARED, subtitle, ore secured this level and run total, robots destroyed per type as text, damage per weapon, Next level button; background per Q4); the prompt names the Spare Parts palette and look explicitly (gunmetal and teal riveted panels, rust, cream Continue button, violet ore, title bar with side bars) and states that no new rule or stat is invented; node vault/reference/gdd/art-audio/art-prompts-level-end-screen.md lists the file, the attachments and the decisions; art-prompts lists it in its Relations; no image generated, no Kie.ai credit spent; lint clean; committed.
- gdd_check: Read 2026-10-30: intermission (spec provisional: two screens, stats screen over the paused finished level, then the cards screen on opaque black), level-flow, run-index, wbp-intermission (current stats screen tree: black backdrop alpha 0.85, Roboto Bold, teal subtitle, Next level button; level 4 variant "Cash out"), art-prompts (method and locked phrases), art-prompts-shop-screen (task-341 text prompt), art-prompts-shop-screen-mockup (ChatGPT mockup and the retained T_Shop sprites, gunmetal and teal look without white plates accepted on purpose, A6), agent-art, handover-2026-10-29-shop-screen. No conflict: a prompt only changes no rule (rules versus rendering). Not in the GDD: no "map screen between levels" exists (only the minimap in the HUD), hence Q1. Gap noted, not decided: the stats screen sits over the dimmed paused level while the cards screen is opaque black (Q4). Open values touching the screen, reported, not decided: robot type icons (GDD: "type names as text for now, no icons"), loading screen image (open, not this screen). In milestone scope (M2 UI art).
- confirmed_by_owner: yes (2026-10-30, "et ok pour le reste")
- milestone: M2
- next_step: task-378-01-prompt-node
- started: open
- waiting_for: none
- result: open

## Questions
Decided 2026-10-30 (owner "et ok pour le reste"): the defaults of Q2, Q3, Q4, Q5, Q6 are accepted; values provisional, owner 2026-10-30.
- Q1 DECIDED 2026-10-30 (owner answer in request): the screen is the one shown when the miner reaches the extractor (stats screen, levels 1 to 3: LEVEL N CLEARED, Next level); "l'écran des cartes" = the Spare Parts upgrade shop screen, the palette and look reference; no map screen.
- Q2 DECIDED 2026-10-30, owner correction (see request): image prompt with only Art/Source/shop-screen/reference/chatgpt-shop-mockup-2026-10-28.webp attached; no owner screenshots. The default below is replaced. Asked: Kind of prompt and attachments: a ChatGPT image prompt for a full 16:9 mockup of the screen (as the Spare Parts mockup was), with your Spare Parts screenshots attached as the palette and look reference (saved in Art/Source/level-end-screen/reference/ before the subtask runs); or a text prompt to discuss design and game feel (as task-341). Also attach Art/Source/shop-screen/reference/chatgpt-shop-mockup-2026-10-28.webp or not. Default: image prompt, your screenshots only, the ChatGPT mockup not attached.
- Q3 Language of the prompt: English (the locked style phrases of art-prompts are English) or French. Default: English.
- Q4 Background: the GDD rule stays, the stats screen is drawn over the dimmed paused level (the cards screen is opaque black). Default: dimmed paused arena behind, as the GDD says.
- Q5 Variant: only the levels 1 to 3 screen (LEVEL N CLEARED, Next level), sample values in English; the level 4 run-won variant (CLAIM EMPTIED, Cash out) is not in the prompt. Default: levels 1 to 3 only.
- Q6 Who and generation: the art agent writes the prompt file and its vault node; no Kie.ai generation, you paste it into ChatGPT yourself. Default: yes, no generation.

## Subtasks
1. task-378-01-prompt-node (vault/reference/tasks/task-378-level-end-screen-prompt/task-378-01-prompt-node.md) — art: write the ChatGPT prompt of the end-of-level stats screen mockup and its vault node art-prompts-level-end-screen; no generation

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-378-01-prompt-node]] — step 1, art.
- [[intermission]] — the spec of the stats screen the prompt depicts.
- [[wbp-intermission]] — current widget of the stats screen.
- [[art-prompts-shop-screen-mockup]] — the Spare Parts screen mockup and retained sprites; its mockup image is the palette and look reference attached to the prompt.
- [[art-prompts]] — parent of the prompt node to create.
