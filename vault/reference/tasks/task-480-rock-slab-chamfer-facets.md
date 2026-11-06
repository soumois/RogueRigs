---
id: task-480-rock-slab-chamfer-facets
description: "Global task: Rock slabs get a slight top chamfer carrying a thin orange-yellow emissive band and faceted, tinted low-poly sides; top, outline and heights kept."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Rock slabs: chamfered glowing top edge and faceted low-poly sides

- request: Owner 2026-11-06 (verbatim), two points after tasks 476-478 closed (478 result not explicitly validated; point 2 supersedes it for the walls). Point 1: "fait passé le temps avant de voir les cartes à 1.2 sec au lieu de 1.8 sec". Point 2 (pasted spec): """Amélioration du rendu des socles rocheux. Owner answer 2026-11-06 (relayed by the main session, verbatim): "Je valide, mais ne lance pas les tâches ( prépare les je vais passer sur un autre compte )" = the plan and the defaults of Q1, Q2, Q3, Q4, Q5, Q6 accepted.
Le rendu actuel des socles ne correspond pas encore à la direction artistique recherchée.
La forme générale est correcte, mais deux problèmes doivent être corrigés.
A. Bord lumineux orange
Le contour orange actuel ressemble à un trait lumineux posé sur le mesh.
Je souhaite un bord lumineux intégré à la géométrie :
* Créer un léger chanfrein sur l'arête supérieure du socle.
* Intégrer une bande émissive orange-jaune fine sur ce chanfrein.
* Ajouter une diffusion lumineuse subtile sur la surface supérieure, près du bord.
* Introduire de légères variations d'intensité.
* Éviter un contour trop épais, parfaitement régulier ou ressemblant à un néon.
Le résultat doit donner l'impression que la roche possède naturellement une bordure minérale lumineuse.
Ne pas utiliser un simple post-process outline pour produire cet effet.
B. Géométrie des flancs
Le flanc actuel est beaucoup trop lisse et cylindrique.
Je souhaite une apparence rocheuse stylisée low-poly :
* Grandes facettes verticales clairement visibles.
* Plans légèrement inclinés et changements subtils de profondeur.
* Arêtes verticales irrégulières.
* Variations de teintes entre les facettes : violet sombre, violet moyen et quelques zones légèrement plus claires.
* Alternance de zones éclairées et ombragées.
* Quelques reliefs verticaux subtils.
Attention :
* Ne pas ajouter de fissures.
* Ne pas donner l'impression d'une roche détruite ou abîmée.
* Ne pas créer de petits détails bruités partout.
* Ne pas rendre les facettes trop géométriques ou répétitives.
C. Cohérence avec le jeu
Conserver :
* La texture supérieure actuelle.
* La silhouette organique des socles.
* Le violet comme couleur dominante.
* Le contour lumineux orange.
* Les proportions et hauteurs actuelles.
Le dessus doit rester une surface continue, sans lignes visibles entre les dalles.
D. Avant implémentation
Inspecter le mesh et les matériaux actuellement utilisés.
Proposer la solution la plus simple pour obtenir :
1. Un chanfrein supérieur avec matériau émissif.
2. Des flancs avec une géométrie facettée et des variations de matériau.
Ne pas remplacer le système de génération des socles sans nécessité.
Présenter d'abord une proposition technique avant de modifier les ressources existantes.""". Attached image (current state after task-478) kept as the before reference: Art/Source/cartoon-look/reference/owner-2026-11-06-wall-after-478.png (top-down game view: lilac textured slab tops with pink crystals, thin bright orange outline along the top edges, smooth dark vertical-streaked sides). One owner point = one global task: task-479 card delay, task-480 rock slabs. This task: point 2, the pasted spec.
- goal: The rock slabs read as stylised low-poly rock with a natural glowing mineral border: a slight chamfer on the top edge carries a thin orange-yellow emissive band with a subtle glow onto the top, and the sides show large vertical facets with violet tint variations, lit and shaded, while the top texture, organic outline, proportions and heights stay as now. The technical proposal (Q1, Q2) is approved by the owner before any asset changes.
- acceptance: A1 a slight chamfer runs along the top edge of every intact slab (geometry, not post-process); a thin orange-yellow emissive band sits on it with slight intensity variations along the edge and a subtle glow on the top face near the edge; it is not thick, not perfectly regular, not neon-like; A2 sides show large clearly visible vertical facets, slightly tilted planes, subtle depth changes, irregular vertical edges and a few subtle vertical reliefs; no cracks, no damaged look, no small noisy detail everywhere, facets not too geometric nor visibly repeated from slab to slab; A3 facet tints vary between dark violet, mid violet and a few slightly lighter areas; lit and shaded facets alternate under the sun and the miner light; violet stays dominant; A4 the top keeps the current texture and lilac look, reads as one continuous surface with no visible line between slabs, and side facets meet without gaps or light seams at slab joints; A5 the organic outline, proportions and heights are unchanged (top level with the top of the miner, visual-style), wall shadows from the miner light still cast; A6 per Q3 broken variants, damage cracks and tilted plates still read as damaged rock; per Q4 heal deposits and ore rock share the look and keep their crystal colours; A7 the slab generation system is kept (the existing piece generator is extended, not replaced); triangle count before and after per piece reported against perf-rock-slab, game-thread and GPU frame time in a -game run not worse by more than 10 %; A8 per Q5 the material tuning runs as a loop (change, -game capture at 1920 x 1080 at the game camera plus a low side view, sheet side by side with the before image, list of failing points, rework), at most 4 rounds or until the owner says "je valide", each round in the loop result; past round 4 the owner gets the sheets and the gaps, no further round without him; A9 compiles, no new runtime error in PIE; A10 final sheets sent to the owner; the report gives the token cost per subtask and in total.
- gdd_check: Read visual-style (Walls line, task-478: dark sides, "no visible cut between facets or blocks, read as one continuous wall"), color-code (Breakable rock row, same), art-style-guide A1 (large flat facets stay visible: agrees with the request), rock-look-reference (dark crystalline columns on the sides in DRGS: agrees), bp-rock-slab-material and handover-2026-10-18-rock-slab (why: point D asks for a technical proposal, built here from the documented system, no asset read). Facts used: slabs are dual-grid display pieces (5 intact shapes + 10 broken FBX) from Tools/art/blender/rock_slab_pieces.py and rock_slab_broken.py, Nanite, instanced, vertex colour R marks rim triangles; M_RockSlab draws the top glow from the outline distance (EdgeParams) and, since 478, sides as emissive with almost no lighting (SideParams.x 0.03), which blocks the requested lit/shaded alternance; heal deposits share MI_RockSlab. Conflicts: 478 "no visible cut between facets" and smooth emissive sides vs visible facets and lit/shaded sides: superseded by the owner spec, edits on visual-style and color-code on confirmation. The top continuity rule of 478 is kept. No open value. Before image kept: Art/Source/cartoon-look/reference/owner-2026-11-06-wall-after-478.png. No target image: the spec text is the reference; image 3 of 478 is no longer the side target.
- confirmed_by_owner: yes (2026-11-06, "Je valide, mais ne lance pas les tâches ( prépare les je vais passer sur un autre compte )")
- milestone: M2
- next_step: task-480-02-broken-pieces-facets
- started: 2026-11-06T20:54:57Z
- waiting_for: none
- executor_tokens: input 22, cache write 162891, cache read 1067024, output 41721
- executor_usage: runs 1, turns 11, context start 17k peak 175k, resumes 0, 7 min, units 0.52 M
- planning_tokens: input 15, cache write 35116, cache read 306038, output 10554
- planning_usage: aeff9c58 "Plan card delay and rock slab look" 1/2 (tasks 479, 480), units 0.13 M
- result: open

## Questions
Decided 2026-11-06 (owner "Je valide, mais ne lance pas les tâches ( prépare les je vais passer sur un autre compte )"): every default below is accepted; values provisional, owner 2026-11-06.
- Q1 Proposition technique, chanfrein (A) : Default: on garde le générateur actuel des pièces de socle (script Blender) et on l'étend : l'arête du dessus de chaque pièce intacte reçoit un léger chanfrein (environ 8 uu de large sur 8 uu de haut, soit environ 5 % de la hauteur ; le dessus reste à la même hauteur, la silhouette vue de dessus ne change pas). Ce chanfrein est marqué dans le mesh comme l'est déjà le liseré. Dans le matériau M_RockSlab, la bande émissive orange-jaune fine est posée sur ce chanfrein, son intensité varie doucement le long du bord (bruit lent selon la position dans le monde, environ plus ou moins 30 %), et la lueur actuelle qui part du bord vers l'intérieur du dessus est gardée mais adoucie pour devenir la diffusion subtile. Pas de post-process outline. Autre : chanfrein peint uniquement dans le matériau, sans toucher au mesh (plus simple, mais ce n'est pas une vraie géométrie).
- Q2 Proposition technique, flancs (B) : Default: même générateur : le flanc de chaque pièce est découpé en grandes facettes verticales de largeur irrégulière (environ 20 à 45 uu), chaque facette légèrement avancée ou reculée (0 à 5 uu) et inclinée (jusqu'à environ 6 degrés), quelques arêtes en léger relief ; ombrage plat, aucun petit détail. Les facettes reviennent à zéro aux bords de chaque pièce pour que les pièces voisines se raccordent sans fente. Contre la répétition : 2 variantes par forme de pièce, tirées selon la case, et dans le matériau une teinte par facette (violet sombre, violet moyen, quelques facettes plus claires) tirée selon la position dans le monde. Les flancs redeviennent éclairés (le réglage de 478 qui les rendait presque uniquement émissifs est retiré), ce qui donne l'alternance éclairé/ombragé avec le soleil et la lumière du mineur. Ensuite : réimport des meshes, réglage du matériau, capture, doc. Autre : 1 seule variante par forme (plus simple, motif plus répétitif).
- Q3 Pièces cassées (dégâts) : Default: les 10 variantes cassées gardent leurs plaques inclinées et leurs fissures, et reçoivent le même flanc facetté là où elles ont un flanc extérieur, pour qu'un bloc abîmé se raccorde à ses voisins intacts ; pas de chanfrein lumineux sur les plaques cassées. Autre : variantes cassées inchangées.
- Q4 Gisements de soin et roche à minerai : Default: ils partagent le matériau de la roche et prennent donc la nouvelle teinte et l'éclairage des flancs ; leurs meshes ne sont pas modifiés et leurs cristaux gardent leurs couleurs ; on les vérifie à la capture. Autre : les exclure du nouveau rendu (matériau séparé).
- Q5 Boucle de réglage : Default: au plus 4 tours (modification, capture en jeu, comparaison avec l'image d'avant, correction), arrêt dès que tout passe ou que vous dites « je valide » ; après 4 tours vous recevez les planches et la liste des écarts. Autre : un autre nombre de tours.
- Q6 Validation avant modification (point D) : Default: votre « ok » sur Q1 et Q2 vaut accord sur la proposition ; l'exécutant inspecte d'abord le mesh et le matériau réels et ne s'arrête pour vous redemander que si un fait contredit cette proposition. Votre image (état actuel après 478) est gardée comme référence « avant » : Art/Source/cartoon-look/reference/owner-2026-11-06-wall-after-478.png. Autre : l'exécutant inspecte, vous envoie une proposition écrite et attend votre accord avant toute modification.

## Subtasks
1. task-480-01-slab-pieces-chamfer-facets (vault/reference/tasks/task-480-rock-slab-chamfer-facets/task-480-01-slab-pieces-chamfer-facets.md) — 3d: Extend the intact slab piece generator per Q1 and Q2: top chamfer marked for the emissive, faceted sides, variants; FBX and previews.
2. task-480-02-broken-pieces-facets (vault/reference/tasks/task-480-rock-slab-chamfer-facets/task-480-02-broken-pieces-facets.md) — 3d: Per Q3, give the broken variants the faceted outer sides of their intact shape, plates and tilt unchanged.
3. task-480-03-reimport-pieces (vault/reference/tasks/task-480-rock-slab-chamfer-facets/task-480-03-reimport-pieces.md) — bp: Reimport the slab meshes and wire the new variants into the existing piece choice.
4. task-480-04-material-loop (vault/reference/tasks/task-480-rock-slab-chamfer-facets/task-480-04-material-loop.md) — bp: Loop on M_RockSlab/MI_RockSlab: chamfer emissive band with intensity variation, subtle top glow, lit faceted sides with facet tints; capture and compare per Q5.
5. task-480-05-capture (vault/reference/tasks/task-480-rock-slab-chamfer-facets/task-480-05-capture.md) — capture: Final independent check of the slabs against the before image.
6. task-480-06-doc (vault/reference/tasks/task-480-rock-slab-chamfer-facets/task-480-06-doc.md) — doc: Docs describe the chamfer, the faceted pieces and the material changes.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-480-01-slab-pieces-chamfer-facets]] — step 1, 3d.
- [[task-480-02-broken-pieces-facets]] — step 2, 3d.
- [[task-480-03-reimport-pieces]] — step 3, bp.
- [[task-480-04-material-loop]] — step 4, bp.
- [[task-480-05-capture]] — step 5, capture.
- [[task-480-06-doc]] — step 6, doc.
