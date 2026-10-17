---
id: task-225-shield-colour-test
description: "Preview renders of the riot shield in charcoal and in light steel grey (stripes kept, yellow company logo), to choose a colour that stands out."
parent: tasks-to-do-index
kind: task
status: review
---
# Shield colour test

- request: "Je me demande si on ne devrait pas faire le bouclier d'une autre couleur que jaune (excepté pour les bandes jaune et noire) pour qu'il ressorte un peu plus." then "Oui, rends les deux, laisse par contre les bandes jaune et noire, le reste avec la couleur testée. Et n'oublie pas le logo de l'entreprise en jaune (il ressortira d'autant plus)." (owner 2026-10-17)
- goal: two variants of the shield rendered side by side, nothing changed in the game.
- acceptance: renders sent to the owner; the owner picks (or not) a colour.
- gdd_check: [[color-code]] (free colours: orange is explosions, acid green and steel blue are robots, cyan and magenta are XP, violet is ore, white and teal are the player side; charcoal and grey are free as a shield colour; the colour principle allows yellow stripes and a yellow logo as details), [[art-prompts-shield]], [[kessler-voss-logo]]. Variants (Orchestrator): A charcoal about #33363B; B light steel grey about #A9AFB5; both: stripes unchanged, grip and bolts dark as now, logo yellow without its dark plate.
- confirmed_by_owner: yes
- milestone: M1
- next_step: owner choice of the shield colour
- result: Done. Commit bd0837c4: Art/Source/shield-colour-test/ (sheet_shield_colours.png: columns current yellow, A charcoal #33363B, B light steel grey #A9AFB5; close-ups, robot mid and game-scale views; textures T_ChaserShield_BaseColor_A/_B and T_ChaserShield_Emblem_YellowOnly), script Tools/art/blender/shield_colour_test.py; stripes, grip and bolts unchanged, logo yellow without plate. A stands out clearly from robot and floor; B is close to the floor brightness, its logo faint; on A the bolts barely show. Game files untouched. Owner choice pending.

## Subtasks
- [[task-225-01-shield-colour-renders]] — 3D agent: the two textures and renders, preview only.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[art-prompts-shield]] — the shield.
- [[color-code]] — the palette.
