---
id: task-341-shop-design-prompt
description: "Global task: A ready-to-paste ChatGPT prompt giving the full context of the shop screen so the owner can discuss design and game feel proposals."
parent: tasks-done-index
kind: task
status: done
---
# ChatGPT prompt for the shop screen design and game feel

- request: Point 2 of the owner message: "Ok voilà l'écran de sélection des cartes entre deux niveaux, il va falloir faire son design et son gamefeeling. Pour cela: * Il me faut le prompt pour chatgpt qui me fera des propositions et je discuterais avec lui" (owner, 2026-10-28, with a screenshot; full message: "* Crée une tâche pour modifier le GDD et le code, les cristaux mauve ne sont plus reset entre les niveaux. Cette ressource ne sert que pour la run en cours. (et c'est frustrant de ne pas pouvoir cumuler les cristaux sur deux niveaux pour faire de gros achat) / Ok voilà l'écran de sélection des cartes entre deux niveaux, il va falloir faire son design et son gamefeeling. Pour cela: * Il me faut le prompt pour chatgpt qui me fera des propositions et je discuterais avec lui * Lorsque j'achète une carte, je veux mes cristaux qui partent du compte et vont vers le bouton * Il faut un côté juicy aussi lorsqu'on clique sur le bouton (autre que l'effet d'avant) * Il faut le même effet que lorsque je clique sur la carte (sauf l'effet qui bouge jusqu'à l'arme ou l'xp"). Owner answer 2026-10-28 (relayed by the main session): "OK, tout par défaut" = every default of Q1 to Q11 accepted (listed in gdd_check).
- goal: A ready-to-paste ChatGPT prompt giving the full context of the shop screen so the owner can discuss design and game feel proposals.
- acceptance: one prompt file the owner pastes into ChatGPT with the screenshot attached; it gives the game pitch and pillars, the visual style and colour code, the shop rules (5 cards, prices by rarity, reroll, Continue, balance), the 1920x1080 frame and UMG limits, the three purchase effects already planned (task-342 to 344), and asks for design and game-feel proposals to discuss; the prompt and the screenshot are saved in the repo and listed in a vault prompt node; lint clean; committed.
- gdd_check: read 2026-10-28: [[run-shop]], [[intermission]], [[rarities]], [[art-prompts]] (prompt nodes branch), [[agent-art]], [[agents-index]]. No conflict. No crew agent writes a text prompt for an external chat; the closest is [[agent-art]] (prompt writing, prompt node under art-prompts); no image generated, no Kie.ai run. owner screenshot of the shop screen saved as `Art/Source/shop-screen/reference/shop-screen-2026-10-28.png` (SPARE PARTS, 4 cards greyed "Short on ore", slot 5 sold, balance 7, Reroll stock 20, Continue). Questions (defaults): Q4 the prompt is written in French (the language the owner uses with ChatGPT); Q5 the [[agent-art]] agent writes it, without image generation. Owner answer 2026-10-28: "OK, tout par défaut": every default above is DECIDED (values provisional, owner 2026-10-28); GDD updated by the Planner the same day.
- confirmed_by_owner: yes (2026-10-28, "OK, tout par défaut" to Q1-Q11 and the plan, relayed by the main session)
- milestone: M2
- next_step: none
- started: 2026-10-28T06:28:01Z
- waiting_for: none
- executor_tokens: input 12, cache write 20435, cache read 135249, output 2661
- executor_usage: runs 1, turns 6, context start 16k peak 32k, resumes 0, 2 min, units 0.05 M
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Done. ChatGPT prompt (French, 7 sections) saved at Art/Source/shop-screen/prompts/chatgpt-shop-design.md; node art-prompts-shop-screen holds a summary (prompt over node size limit); no image generated; lint clean.

## Subtasks
1. task-341-01-shop-design-prompt (vault/reference/tasks/task-341-shop-design-prompt/task-341-01-shop-design-prompt.md) — art: write the ChatGPT prompt and its vault prompt node.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-341-01-shop-design-prompt]] — step 1, art.
