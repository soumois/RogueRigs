---
id: task-477-bottom-hud-back-to-mockup
description: "Global task: bottom HUD redone from the retained art mockup, reworked in at most 6 capture rounds until it matches."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Bottom HUD redone from the art proposal, up to 6 correction rounds

- request: Owner 2026-11-06 (verbatim): "* Le muzzle du joueur tir est très bien, mais il est un peu trop grand. * La partie du bas de l'écran est mieux, mais on s'est éloigné de la proposition de l'art. Repart de la proposition et je laisse 6 tours de correction ( c'était 5 avant ) pour arriver au même résultat. ( image 1 ) * Image 2 on voit des lignes noir sur la surface des murs, il faudrait retiré ça et avoir plus quelque chose comme l'image 3. Il a aussi l'emissif doit aussi être sur l'edge sur la partie du dessus. ( 4 tours de correction max )". Image 1 ("target", 1229 x 190): a crop of the bottom of the retained HUD mockup Art/Source/hud-unified/reference/owner-hud-target-2026-11-05.webp (= Art/Source/hud-unified/20261105-152627-hud-unified-1.png), same picture; labelled temporary (duplicate crop), kept only in Saved/Scratch/hud-477/owner-target-crop-2026-11-06-temporary.png. Image 2 kept (owner image): Art/Source/cartoon-look/reference/owner-2026-11-06-wall-black-lines.png (low view of the walls: purple wall faces with dark wavy horizontal lines and visible vertical facet cuts; orange glow along the top lip; an orange editor selection outline at the wall foot). Image 3 kept (owner image): Art/Source/cartoon-look/reference/owner-2026-11-06-wall-section-target.png (diagram "Coupe de principe": top surface with a continuous texture, glowing emissive edge "bord lumineux", wall "maille unique (sans découpe)" with soft vertical dark streaks). Split into three global tasks, one per owner point: task-476 muzzle flash, task-477 bottom HUD, task-478 walls. This task: "La partie du bas de l'écran est mieux, mais on s'est éloigné de la proposition de l'art. Repart de la proposition et je laisse 6 tours de correction ( c'était 5 avant ) pour arriver au même résultat. ( image 1 )". Follows task-467-bottom-hud-match-target (done in 2 of 5 rounds, judged matching by its own capture; the owner now says it drifted from the proposal). Owner answer 2026-11-06 (relayed by the main session, verbatim): "Ok, tout par défaut" = the plan and the defaults of Q1, Q2, Q3 accepted.
- goal: The bottom of the HUD in game looks like the art proposal itself, not an interpretation of it, judged closely against the mockup in at most 6 rounds.
- acceptance: A1 the target is Art/Source/hud-unified/20261105-152627-hud-unified-1.png (= Art/Source/hud-unified/reference/owner-hud-target-2026-11-05.webp; image 1 is its bottom crop); the look before is a -game capture at 1920 x 1080 taken at the start of the task; A2 per Q1 the work starts again from the proposal: the frame pieces come from the mockup itself (Q1), and the task-467 layout is only a starting point; A3 checklist, each point judged on a zoomed crop of the bottom HUD next to the mockup crop at the same scale: C1 one bottom block centred, its width, top, health strip height and XP rail height within +-5 % of the mockup values (Q2); C2 frame colour and wear as the mockup: weathered teal-green metal with rust streaks and chips, not blue-grey gunmetal; C3 rivets as the mockup: small rivets at the corners and along the edges, stepped corner plates at the block shoulders, count and size close to the mockup; C4 health strip: bright green fill in a framed strip a little narrower than the slot row, sitting on it, "100 / 100" white bold with a dark outline centred inside the green; C5 four equal framed cells, wider than tall, dark insides, rifle icon with its cream-yellow reload bar in cell 1, teal padlocks centred in cells 2-4; C6 XP rail: thin dark track along the bottom edge with riveted end caps, "Lv 1" in a riveted plate at its left end, four white ticks at 20/40/60/80 %; C7 nothing of the bottom HUD covers another piece or leaves the screen; C8 after damage and after an XP gain the fills, damage chunk and ghost show inside their bars; A4 the work runs as a loop: change, -game capture at 1920 x 1080, side-by-side sheet with the mockup, list of failing points, rework; it stops when every point passes, after 6 rounds (owner), or when the owner says "je valide"; each round is written in the loop subtask result (round, points failed, what changed); if points still fail after round 6, it stops and the owner gets the sheets and the list of gaps, no further round without him; A5 rules unchanged: health value and damage chunk, slot icons, reload bars, padlocks, card-pick flight to a slot and to the XP bar, XP fill, ticks and gain ghost; top HUD (radar, ore count, alert bar) untouched; A6 widgets follow umg-layout (Canvas root, anchors, no fixed pixel sizes); an uncommitted owner edit of a touched widget is committed alone first; compiles, no runtime error in PIE; A7 sprites per Q1, recorded in art-prompts-hud-unified-mockup with keep and temporary labels; A8 a final independent capture at the resolutions of Q3 gives each point a verdict and the owner receives the side-by-side sheets mockup / result; the report gives the token cost per subtask and in total.
- gdd_check: Read task-467-bottom-hud-match-target (same target, loop ended after 2 rounds with tolerance +-15 %, Kie.ai frames: blue-grey gunmetal, thinner block frame, Lv plate 4 rivets vs 8 on the mockup, per art-prompts-hud-unified-mockup critic notes), art-prompts-hud-unified-mockup, umg-layout (via task-467). Image 1 compared visually with Art/Source/hud-unified/reference/owner-hud-target-2026-11-05.webp: same picture, cropped. No GDD conflict: look only. Reading of "Repart de la proposition" and the stricter judging are asked (Q1, Q2); final check resolutions asked (Q3, same choice as task-467 Q4). No code read.
- confirmed_by_owner: yes (2026-11-06, "Ok, tout par défaut")
- milestone: M2
- next_step: task-477-03-slot-cells
- started: 2026-11-06T19:36:24Z
- waiting_for: none
- executor_tokens: input 144, cache write 194356, cache read 8760545, output 61953
- executor_usage: runs 1, turns 69, context start 17k peak 206k, resumes 1, 14 min, units 1.43 M
- planning_tokens: input 11, cache write 29692, cache read 298315, output 7823
- planning_usage: af36ec5b "Plan owner request: muzzle, HUD, walls" 1/3 (tasks 476, 477, 478), units 0.11 M
- result: open

## Questions
Decided 2026-11-06 (owner "Ok, tout par défaut"): every default below is accepted; values provisional, owner 2026-11-06.
- Q1 "Repart de la proposition" : Default: les cadres (bande de vie, cadre du bloc, cases, rail d'XP, plaque "Lv") sont découpés et nettoyés directement dans l'image de la proposition (0 crédit), au lieu des sprites Kie.ai de la tâche 467 qui ont dérivé (métal gris-bleu, cadre plus fin, moins de rivets) ; la mise en page actuelle sert seulement de point de départ. Autres : nouveaux sprites Kie.ai à partir de la proposition (environ 24 crédits) ; ou garder les sprites actuels et ne corriger que tailles et couleurs.
- Q2 Jugement plus strict : Default: à chaque tour, recadrage zoomé du bas de l'écran à côté du même recadrage de la proposition, même échelle ; positions et tailles à +-5 % (c'était +-15 %) ; couleur et usure du métal, rivets et plaques d'angle jugés pièce par pièce. Autre : une autre tolérance.
- Q3 Vérification finale : Default: comme la dernière fois, seulement en 1920 x 1080. Autre : les quatre résolutions (1920 x 1080, 1280 x 720, 1920 x 1200, 2560 x 1080).

## Subtasks
1. task-477-01-art (vault/reference/tasks/task-477-bottom-hud-back-to-mockup/task-477-01-art.md) — art: Frame pieces for the health strip, block frame, slot cell, XP rail and Lv plate, taken from the mockup per Q1.
2. task-477-02-health-block (vault/reference/tasks/task-477-bottom-hud-back-to-mockup/task-477-02-health-block.md) — bp: The health strip and the block frame use the new pieces, placed as on the mockup.
3. task-477-03-slot-cells (vault/reference/tasks/task-477-bottom-hud-back-to-mockup/task-477-03-slot-cells.md) — bp: The four weapon cells use the new cell piece as on the mockup, contents unchanged.
4. task-477-04-xp-rail (vault/reference/tasks/task-477-bottom-hud-back-to-mockup/task-477-04-xp-rail.md) — bp: The XP rail and Lv plate use the new pieces as on the mockup.
5. task-477-05-match-loop (vault/reference/tasks/task-477-bottom-hud-back-to-mockup/task-477-05-match-loop.md) — bp: Loop: -game capture, zoomed side-by-side with the mockup, rework failing points; at most 6 rounds.
6. task-477-06-capture (vault/reference/tasks/task-477-bottom-hud-back-to-mockup/task-477-06-capture.md) — capture: Final independent check of the bottom HUD against the mockup at the resolutions of Q3.
7. task-477-07-doc (vault/reference/tasks/task-477-bottom-hud-back-to-mockup/task-477-07-doc.md) — doc: Docs describe the bottom HUD pieces and layout.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-477-01-art]] — step 1, art.
- [[task-477-02-health-block]] — step 2, bp.
- [[task-477-03-slot-cells]] — step 3, bp.
- [[task-477-04-xp-rail]] — step 4, bp.
- [[task-477-05-match-loop]] — step 5, bp.
- [[task-477-06-capture]] — step 6, capture.
- [[task-477-07-doc]] — step 7, doc.
