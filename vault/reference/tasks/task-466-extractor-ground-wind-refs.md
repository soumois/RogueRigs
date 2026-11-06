---
id: task-466-extractor-ground-wind-refs
description: "Global task: The extractor ground blast is redone after the owner sprite sheet and in-game mockup: flattened swirl rings, curved wind streaks, dust puffs pushed outward."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Extractor ground wind redone from the owner references

- request: Owner 2026-11-06 (verbatim): "Pour la particule sous l'extracteur, je t'ai donné des ref ." Two owner images, kept (owner references): image 1 Art/Source/extractor-fx/reference/owner-ground-wind-sheet-2026-11-06.webp (sprite sheet "Sprites - Effet de vent au sol (VTOL)", "Textures PNG avec transparence (exemples)": long and short curved wind-streak arcs, small cartoon dust puffs with debris dots, a large flattened elliptical swirl ring, a soft round radial dust disc; warm beige tint, painted cartoon style; the transparency is a painted grey checker, not a real alpha), image 2 Art/Source/extractor-fx/reference/owner-ground-wind-mockup-2026-11-06.webp (in-game mockup: the extractor hovering over the purple floor; under and around it several concentric translucent whitish swirl rings flattened on the floor, reaching about twice the extractor width, curved wind streaks, small dust puffs pushed outward, faint downwash columns under the four fans, orange glows at the feet). Follows task-462-extractor-blast-cartoon (done, look made from web references, unverified by the owner). Owner answer 2026-11-06 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q1, Q2, Q3 accepted.
- goal: The wind on the floor under the extractor looks like the owner mockup, built from the shapes of his sprite sheet.
- acceptance: A1 in game under the landed extractor the ground blast shows, as in Art/Source/extractor-fx/reference/owner-ground-wind-mockup-2026-11-06.webp: several concentric translucent whitish-beige swirl rings flattened on the floor around the extractor, the outer one about twice its width, slowly turning and spreading outward; curved wind streaks and small cartoon dust puffs with debris dots travelling outward; a faint round dust disc under the hull; the shapes are those of Art/Source/extractor-fx/reference/owner-ground-wind-sheet-2026-11-06.webp (made again, not cut from the image, whose checker background is painted); A2 rings and disc may have soft painted edges, puffs and streaks stay crisp (art-style-guide A10); the effect stays translucent and the floor, miner and shots remain readable through it; A3 strength still falls with the base height, full when landed, none at 400 uu, and it follows both takeoffs as today; A4 the rotor swirls under the rotors (task-442), rotor spin, the extractor without shadow, feet and timings unchanged; no gameplay effect, no Event Tick; Extraction tests pass; A5 no capture and no stress run: the owner validates the look in game himself; the final report lists what to look at, says plainly that the look is unverified until he has seen it, and gives the token cost per subtask and in total.
- gdd_check: Read extraction (ground blast: outward dust, height fade, pale grey-beige, low opacity, task-462 cartoon rework), art-style-guide (A10), extractor-blast-references, task-462. Conflict: task-462 wrote "sharp shapes, no blurry blobs", the owner references have soft painted rings and a soft dust disc; default Q1 lets rings and disc be soft, puffs and streaks crisp. Out of scope by default (Q2): the downwash columns under the fans (task-442 says soft swirls, no thin lines) and the orange glows at the feet seen in the mockup. Size of the rings: no value in the GDD; taken from the mockup, outer ring about twice the extractor width. Owner images kept in Art/Source/extractor-fx/reference/ (owner references, precedent task-462). extraction edited at confirmation. No code read.
- confirmed_by_owner: yes (2026-11-06, "ok")
- milestone: M2
- next_step: task-466-02-ground-wind
- started: 2026-11-06T05:27:11Z
- waiting_for: none
- executor_tokens: input 88, cache write 137458, cache read 3734365, output 34565
- executor_usage: runs 1, turns 42, context start 17k peak 137k, resumes 0, 7 min, units 0.72 M
- planning_tokens: input 22, cache write 51418, cache read 361734, output 10637
- planning_usage: a8acd422 "Plan extractor ground-wind particle" 1/1 (tasks 466), units 0.15 M
- result: open

## Questions
Decided 2026-11-06 (owner "ok"): every default below is accepted; values provisional, owner 2026-11-06.
- Q1 Rendu : Default: on suit ton mockup (image 2) avec les formes de ta planche (image 1), refaites proprement (le fond en damier de la planche est peint, on ne peut pas découper les sprites) : plusieurs anneaux de volutes translucides blanc-beige à plat au sol, le plus grand environ deux fois la largeur de l'extracteur, qui tournent lentement et s'élargissent vers l'extérieur ; traits de vent courbés et petits nuages de poussière avec débris qui partent vers l'extérieur ; un disque de poussière léger sous la coque. Les anneaux et le disque peuvent être doux (bords peints), les nuages et traits restent nets ; cela remplace la règle "pas de flou" de la tâche 462 pour les anneaux et le disque. La force baisse toujours avec la hauteur (nulle à 4 m). Autre choix : tout reste à bords nets.
- Q2 Périmètre : Default: seul l'effet au sol change. Les colonnes de souffle sous les hélices (aujourd'hui des volutes douces sans traits, tâche 442) et les lueurs orange aux pieds visibles sur le mockup ne sont pas touchées. Autre choix : les ajouter aussi (une tâche séparée chacune).
- Q3 Validation : Default: pas de capture, tu valides le rendu en jeu toi-même, comme pour la tâche 462 ; le rapport final liste quoi regarder. Autre choix : une capture en jeu comparée à ton mockup.

## Subtasks
1. task-466-01-wind-sprites (vault/reference/tasks/task-466-extractor-ground-wind-refs/task-466-01-wind-sprites.md) — bp: Material set for the ground wind shapes of the owner sheet: swirl ring, curved streaks, dust puffs with debris, dust disc.
2. task-466-02-ground-wind (vault/reference/tasks/task-466-extractor-ground-wind-refs/task-466-02-ground-wind.md) — bp: The extractor ground blast shows the mockup layout with these shapes, height fade and takeoffs kept.
3. task-466-03-doc (vault/reference/tasks/task-466-extractor-ground-wind-refs/task-466-03-doc.md) — doc: Docs describe the new ground wind and the owner references it follows.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-466-01-wind-sprites]] — step 1, bp.
- [[task-466-02-ground-wind]] — step 2, bp.
- [[task-466-03-doc]] — step 3, doc.
