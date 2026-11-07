---
id: task-488-loading-screen-rework
description: "Global task: replace the plain dark loading screen (New Run and between levels) with an illustrated, framed screen in the game style. Open to run or review it."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Richer loading screen (illustration and framed texts)

- request: Owner, verbatim (French), 2026-11-06: "Il faudrait retravailler les boutons du main menu pour que ça s'incorpore mieux et aussi le menu de chargement beaucoup trop basique." Two separate owner points, two globals: task-487 (main menu buttons) and task-488 (loading screen). This global covers "le menu de chargement beaucoup trop basique" (the WBP_Loading screen shown on New Run and between levels). Options and defaults are Q1 to Q5; his answers are appended here at confirmation.
- goal: The loading screen looks like a finished game screen in the key art style: a full-screen illustration, the existing texts and progress (bar or spinning ore icon) in a riveted frame; its behaviour (when it shows, texts, progress, spinner, fade) unchanged.
- acceptance: A1 The loading screen shows one full-screen illustration in the key art style (art-style-guide, color-code), no text in the image, as intermission says, on New Run and between levels. A2 A dark gradient at the bottom and a riveted 9-slice frame in the stats screen style at bottom left hold "DROPPING IN", "Level N" and the stage line, unchanged texts, readable over the image. A3 Between levels with measured progress the bar is an orange fill in a dark riveted track inside the frame (no engine default bar look) and advances as before; on New Run the spinning ore icon shows instead, inside the frame. A4 Behaviour unchanged: shown and removed at the same moments, texts from screen_texts.json, fade in after. A5 At 1920 x 1080, 1280 x 720, 1920 x 1200 and 2560 x 1080 the image fills the screen and nothing overlaps or is clipped. A6 The image is kept in the repo and listed in a vault art-prompts node with its verbatim prompt; build clean, widget compiled with warnings as errors.
- gdd_check: Read: intermission (loading screen spec, image slot marked open), main-menu (New Run loading), art-audio-index, umg-layout, verification-levels; tech doc wbp-loading read because the questions depend on what the screen is now (opaque dark backdrop, centred texts, engine-style bar 600 x 24, collapsed reserved image slot, spinning T_Icon_Ore). Open value: the loading image (intermission "image open") and the whole look; decided by Q1 to Q4 defaults, written in intermission by the edit. No conflict: texts, stages, progress API and spinner behaviour unchanged (rules versus rendering). The reported slow start of the spinner (wbp-loading, unverified) is not part of this request and not planned. No tips line or new texts: not requested. Milestone: M2 polish of existing screens. Verification exploratory: a look to find with a new image asset; 3 capture rounds by default (Q5).
- verification_level: exploratory
- confirmed_by_owner: no (plan and questions handed back 2026-11-07)
- milestone: M2
- next_step: task-488-01-loading-image
- started: open
- waiting_for: owner answers to Q1 to Q5 of task-488 (or "ok")
- result: open

## Questions
Asked 2026-11-07, each with a default; open until the owner answers.
- Q1 Contenu de l'écran de chargement. A: une illustration plein écran (agent art, Kie.ai GPT Image 2 en image-to-image depuis la key art, comme le fond du menu) + un cadre riveté façon écran de stats en bas à gauche avec les textes et la barre. B: seulement le cadre riveté et une barre stylée sur le fond sombre actuel, sans image. C: seulement l'image, textes et barre restent tels quels au centre. Défaut: A.
- Q2 Sujet de l'image : la navette de la run qui plonge vers l'astéroïde de l'arène dans le vide (indigo, violet, magenta, étoiles), le mineur et son drone à la trappe, sans texte. Une seule image pour New Run et tous les niveaux. Défaut: oui (sinon donne ton sujet, ou une image par niveau).
- Q3 Textes et progression : « DROPPING IN », « Level N » et la ligne d'étape inchangés, dans le cadre ; la barre devient un remplissage orange dans une piste sombre rivetée ; sur New Run l'icône de Duskite qui tourne reste, dans le cadre à la place de la barre. Défaut: oui.
- Q4 Image fixe, pas de musique, pas d'animation de l'image (zoom lent, parallaxe). Défaut: oui.
- Q5 Vérification niveau exploratoire (un rendu à trouver) : 3 tours max de capture -game et correction, aux 4 résolutions (1920 x 1080, 1280 x 720, 1920 x 1200, 2560 x 1080), les deux cas (barre entre deux niveaux, icône qui tourne sur New Run) ; au bout des 3 tours je te montre où on en est. Tu peux aussi valider l'image seule avant le montage. Défaut: 3 tours, 4 résolutions, pas d'arrêt sur l'image.

## Subtasks
1. task-488-01-loading-image (vault/reference/tasks/task-488-loading-screen-rework/task-488-01-loading-image.md) — art: One full-screen loading illustration made and kept as intermission says (subject Q2), prompt listed in a new art-prompts node.
2. task-488-02-loading-layout (vault/reference/tasks/task-488-loading-screen-rework/task-488-02-loading-layout.md) — bp: WBP_Loading rebuilt to the intermission look: image, bottom gradient, riveted frame holding the texts and the restyled bar or spinner.
3. task-488-03-capture (vault/reference/tasks/task-488-loading-screen-rework/task-488-03-capture.md) — capture: -game capture of the loading screen in both cases at the four resolutions, up to 3 rounds of capture and correction.
4. task-488-04-doc (vault/reference/tasks/task-488-loading-screen-rework/task-488-04-doc.md) — doc: Doc node of the loading widget updated.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-488-01-loading-image]] — step 1, art.
- [[task-488-02-loading-layout]] — step 2, bp.
- [[task-488-03-capture]] — step 3, capture.
- [[task-488-04-doc]] — step 4, doc.
