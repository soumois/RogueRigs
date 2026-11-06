---
id: task-473-miner-light-calibration
description: "Global task: The miner light colour, intensity and pool size are tuned in game until the light around the miner matches the warm orange-pink pool of owner image 2."
parent: tasks-done-index
kind: task
status: done
---
# Miner light calibrated on the owner in-game reference

- request: Owner 2026-11-05 (verbatim): "J'aimerais faire des changements sur le design (donc ça va aller à l'encontre de ce qu'il y a dans le vault). - On va passer plus en cartoon * (image 1 au sol) change la texture du sol * Changer le système d'ombre (voilà ce qu'on va tester) : --- Each shadow must: * Be positioned directly beneath its associated actor. * Follow the actor's world position. * Remain independent of directional lights and other light sources. * Use a dark desaturated purple-grey colour, never pure black. * Have adjustable opacity and scale. * Have slightly softened edges, while retaining a clearly readable silhouette. * Avoid excessive transparency or overly blurred edges. --- * Essaye de calibrer la lumière comme la lumière du joueur sur l'image * Essaye de ré-obtenir le rendu sur l'image concernant les murs Ne lance aucune tâche". Image 1 kept (owner image): Art/Source/floor-texture/owner-2026-11-05-floor-cartoon.webp (seamless hand-painted cartoon magenta-purple flagstones, dark cracks with a pink rim highlight). Image 2 kept (owner image): Art/Source/cartoon-look/reference/owner-2026-11-05-ingame-light-walls.webp (in-game view: warm orange-pink light pool around the miner on the purple floor; raised purple plateaus with light-lilac tops, a glowing orange rim along the cliff lip, dark stratified cliff faces, pink crystals). The request is split into four global tasks (one per owner point): task-470 floor, task-471 walls, task-472 actor shadows, task-473 miner light. "Ne lance aucune tâche": planned only. This task: "Essaye de calibrer la lumière comme la lumière du joueur sur l'image". Owner answer 2026-11-06 (relayed by the main session, verbatim): "Mur : Q1 : voir image. Rélevé légerement les mur ( il faut que ça fasse la taille du joueur - Mais réduit la taille du joueur de 10 pourcent ). Ok pour tout le reste" = the plan and the defaults of Q1, Q2 accepted. Owner launch order (relayed by the main session, verbatim): "Lance les task to do dans l'ordre" = launch now; this lifts the earlier "do not launch" instruction.
- planner_note: runs last, after tasks 470, 474, 475, 471 and 472 (2026-11-06 owner points added); the owner light height in BP_Miner stays unchanged even though the miner is 10 % smaller.
- goal: In game the light pool around the miner looks like the one of image 2, judged on side-by-side captures.
- acceptance: A1 the target is the light pool around the miner on Art/Source/cartoon-look/reference/owner-2026-11-05-ingame-light-walls.webp; the look before is Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-06.webp; A2 hue: warm orange at the centre fading to pink-violet on the floor, as on the image; A3 the pool size and falloff on screen match the image (measured relative to the screen and the miner size); A4 brightness: the centre is about as bright as on the image, and the floor under the miner is never burnt (below about 205/255); A5 rules kept: the light touches only the floor and terrain blocks, the walls keep their miner-light shadows, robots, miner, drone and pickups receive none; the screen darkener and sun and sky are unchanged per Q1; A6 the work runs as a loop: change, -game capture at 1920 x 1080, side-by-side sheet with image 2, list of failing points, rework; it stops when every point passes, at the round cap of Q2, or when the owner says "je valide"; each round is written in the loop subtask result; A7 the owner-tuned light height in BP_Miner is not changed; A8 no new runtime error in PIE; A9 the final side-by-side sheets are sent to the owner; the report gives the token cost per subtask and in total.
- gdd_check: Read visual-style (miner light: floor and terrain only, rock shadows, owner height in BP_Miner, warm cream 255/194/158, pool about 400 uu, never burnt below about 205/255; screen darkener 0.2 / 1 with clear zone), handover-2026-10-21-lighting-floor (light history). Conflict: the warm cream colour and the 400 uu pool are replaced by values calibrated on image 2; edit in the spec applies on confirmation. Rules kept by default (Q1). No code read. Runs last, after tasks 470, 471 and 472, because the pool is judged on the new floor and walls (Q2).
- confirmed_by_owner: yes (2026-11-06, "Mur : Q1 : voir image. Rélevé légerement les mur ( il faut que ça fasse la taille du joueur - Mais réduit la taille du joueur de 10 pourcent ). Ok pour tout le reste")
- milestone: M2
- next_step: none
- started: 2026-11-06T18:22:25Z
- waiting_for: none
- executor_tokens: input 122, cache write 116931, cache read 4860016, output 26186
- executor_usage: runs 1, turns 58, context start 18k peak 129k, resumes 3, 14 min, units 0.76 M
- planning_tokens: input 9, cache write 16900, cache read 306744, output 7019
- planning_usage: adacf5eb "Plan cartoon art direction change" 1/6 (tasks 470, 471, 472, 473, 474, 475), units 0.09 M
- result: Done at the Q2 cap (3 rounds). FloorWallLight in BP_Miner: intensity 6.5, radius 700, falloff exponent 4->3, colour 255/175/60 (was 4 / 800 / 4 / 255,194,158); location (0,0,0) kept (A7). A2 pass with gap (outer ring redder than image 2 pink-violet), A3 pass, A4 gap: floor R p95 about 219 near the miner vs the 205 rule (image 2 itself about 252), owner call; A5 robots and pickups unlit seen, wall-shadow on floor ambiguous (castShadows true read back); A8 no new error lines. Sheets Saved/Captures/task473-capture/final_sheet.png, final_sheet_crop.png. Open: gdd visual-style still holds the old cream colour and 400 uu pool. Commits d2415904, 84ad9e80.

## Questions
Decided 2026-11-06 (owner "Mur : Q1 : voir image. Rélevé légerement les mur ( il faut que ça fasse la taille du joueur - Mais réduit la taille du joueur de 10 pourcent ). Ok pour tout le reste"): every default below is accepted; values provisional, owner 2026-11-06.
- Q1 Ce qui change : Default: seulement la couleur, l'intensité et la taille de la flaque de lumière du mineur, recalées sur l'image 2 (orange chaud au centre qui passe au rose-violet sur le sol) ; gardés : elle n'éclaire que le sol et les murs, les murs gardent leurs ombres, sol sous le mineur jamais brûlé, hauteur réglée par toi inchangée ; l'assombrisseur d'écran (0,2 / 1), le soleil et le ciel ne bougent pas.
- Q2 Ordre et boucle : Default: lancée en dernier, après le sol, les murs et les ombres (la lumière se juge sur le nouveau sol) ; au plus 3 tours capture -> comparaison côte à côte avec l'image 2 -> retouche, en 1920 x 1080 ; ton "je valide" arrête la boucle. Autre : un autre nombre de tours.

## Subtasks
1. task-473-01-light-match-loop (vault/reference/tasks/task-473-miner-light-calibration/task-473-01-light-match-loop.md) — bp: Loop: tune the miner light colour, intensity and radius, -game capture, side-by-side with image 2, rework until it matches or the Q2 cap.
2. task-473-02-capture (vault/reference/tasks/task-473-miner-light-calibration/task-473-02-capture.md) — capture: Final independent check of the miner light against image 2.
3. task-473-03-doc (vault/reference/tasks/task-473-miner-light-calibration/task-473-03-doc.md) — doc: Docs record the calibrated miner light values.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-473-01-light-match-loop]] — step 1, bp.
- [[task-473-02-capture]] — step 2, capture.
- [[task-473-03-doc]] — step 3, doc.
- [[bp-miner]] — holds the calibrated FloorWallLight values and their revert.
