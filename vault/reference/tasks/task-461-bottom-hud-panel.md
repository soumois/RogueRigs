---
id: task-461-bottom-hud-panel
description: "Global task: health bar and weapon slots in one riveted panel, XP bar in a riveted frame with a Lv cap, as in the mockup."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Bottom HUD built as one riveted panel and a framed XP bar

- request: Owner 2026-11-05 (verbatim, whole message): "* Rajouter 1 sec lorsqu'on level up ( laisser le temps de voir les effets du level up ) * Voilà le rendu actuel de l'UI ( voir image 1 ): * Le problème avec le top où on voit les events, c'est que le point d'exclamation du swarm cache le "C" de l'event commande ( et le "B" Bonus quand le point d'exclamation bouge ) * Toute la partie en bas de l'écran n'est pas intégré comme sur l'image 2 * Sur l'image 3, l'image de vent au sol ne fonctionne pas du tout, essaye de trouver des références * Les particules d'explosion fonctionne super bien ! J'adore le style cartoon pour les particules !" Images kept (owner images): image 1 Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-05b.webp (current HUD: two red "!" on the alert bar, only B and S readable, the C hidden under a "!"; health bar, four separately framed slots linked by small connectors and a bare "Lv 1" XP bar, all floating apart), image 2 Art/Source/hud-unified/reference/owner-hud-target-2026-11-05.webp (target: health bar and the four slots inside one riveted metal panel, XP bar in its own riveted frame along the bottom edge with "Lv 1" in a framed cap; C, B, S readable, "!" not covering them; it matches the retained mockup Art/Source/hud-unified/20261105-152627-hud-unified-1.png), image 3 Art/Source/extractor-fx/reference/owner-ground-blast-2026-11-05.webp (the landed extractor at level start; the ground blast shows as large blurry white oval blobs on the floor). The last point is praise, recorded as a style preference in art-style-guide A10, no task. This task is the point "Toute la partie en bas de l'écran n'est pas intégré comme sur l'image 2".
- goal: The bottom of the screen reads as one built-in piece of the HUD, like image 2, instead of separate floating pieces.
- acceptance: A0 the target is Art/Source/hud-unified/reference/owner-hud-target-2026-11-05.webp = the retained mockup Art/Source/hud-unified/20261105-152627-hud-unified-1.png; A1 the health bar and the four weapon slots sit inside one continuous riveted metal panel, no separate slot frames with connectors as in Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-05b.webp; A2 the XP bar sits in its own riveted frame along the bottom edge, "Lv N" in a framed cap at its left; A3 health value and damage chunk, slot icons, reload bars, padlocks, the card-pick flight to a slot and to the XP bar, XP fill, 4 ticks at 20/40/60/80 % and the gain ghost work as before; A4 sprites per Q37, recorded in art-prompts-hud-unified-mockup with keep and temporary labels; A5 widgets follow umg-layout (Canvas root, anchors, no fixed pixel sizes), nothing leaves the screen at its four resolutions; they start from the assets as they are: an uncommitted owner edit is committed alone first; compiles with no runtime error in PIE; A6 No capture and no stress run in this task unless the owner asks: he validates the look in game himself; the final report lists what to look at and says plainly that the look is unverified until he has seen it. The report also gives the token cost of this task, per subtask and in total, from cost-log (units, turns, runs, resumes).
- gdd_check: Read xp-level-up, art-prompts-hud-unified-mockup (retained A direction; 452 health plate and 458 slots reused T_Shop_ButtonReroll and T_LevelEnd_Frame, the result the owner now says is not integrated), task-458. No conflict; xp-level-up edited at confirmation. Image 2 matches the retained mockup. Source of the frame sprites: Q37.
- confirmed_by_owner: no (plan and questions handed back 2026-11-06)
- milestone: M2
- next_step: task-461-01-art
- started: open
- waiting_for: owner answers to Q35 to Q38 and his go
- result: open

## Questions
Asked 2026-11-06, each with a default; open until the owner answers.
- Q37 Partie basse : Default: deux nouveaux sprites Kie.ai tirés de la maquette retenue (plaques rivetées A) : un grand cadre riveté qui englobe la vie et les 4 emplacements, et un cadre pour la barre d'XP avec sa pastille « Lv » ; 2 variantes chacun, environ 24 crédits ; le critique garde la plus proche de la maquette, tu juges en jeu. Autres choix : tu choisis toi-même entre les variantes (la tâche s'arrête en attendant) ; ou seulement les sprites déjà gardés, 0 crédit.

## Subtasks
1. task-461-01-art (vault/reference/tasks/task-461-bottom-hud-panel/task-461-01-art.md) — art: Frame sprites of the bottom panel and of the XP bar with its Lv cap, from the retained mockup.
2. task-461-02-health-slots-panel (vault/reference/tasks/task-461-bottom-hud-panel/task-461-02-health-slots-panel.md) — bp: The health bar and the four weapon slots sit inside one riveted panel.
3. task-461-03-xp-frame (vault/reference/tasks/task-461-bottom-hud-panel/task-461-03-xp-frame.md) — bp: The XP bar sits in its riveted frame with Lv in a framed cap.
4. task-461-04-doc (vault/reference/tasks/task-461-bottom-hud-panel/task-461-04-doc.md) — doc: Docs describe the bottom panel and the XP frame.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-461-01-art]] — step 1, art.
- [[task-461-02-health-slots-panel]] — step 2, bp.
- [[task-461-03-xp-frame]] — step 3, bp.
- [[task-461-04-doc]] — step 4, doc.
