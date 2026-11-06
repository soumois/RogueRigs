---
id: task-337-main-menu-background
description: "Global task: Main menu background image: one 16:9 scene in the validated key art style, generated through Kie.ai, picked by the owner."
parent: tasks-done-index
kind: task
status: done
---
# Main menu background image

- request: "Ok maintenant, on va travailler au design du main menu. Travaille avec ce que tu as et les images que tu as et chatpgpt pour me faire un bon background, un menu ( new run , option qu'on fera plus tard et exit )." (relayed verbatim by the main session, owner 2026-10-28). Main session reading: "chatgpt" = the GPT Image 2 route through Kie.ai used by the art agent; "les images que tu as" = the validated images in vault/assets/. This task covers the point "un bon background"; the menu itself is [[task-338-main-menu-screen]]. Owner answer (2026-10-28, relayed by the main session): "ok" to the whole list = every default Q1 to Q11 is DECIDED as written (provisional values); the alternatives are not taken. Written in [[main-menu]] (vault/reference/gdd/run/main-menu.md).
- goal: Main menu background image: one 16:9 scene in the validated key art style, generated through Kie.ai, picked by the owner.
- acceptance: images sent to the owner; style-guide review done; the owner picks or rejects; the chosen image is copied to vault/assets/ only on his word; prompt node written; committed.
- gdd_check: no main menu node exists in the GDD, tech or wishlist (descent gdd-index > run-index and art-audio-index; tech > blueprints-ui-index, systems-ui-index). Read [[visual-style]] (flat-shaded low-poly, arena = asteroid with a cliff into space; in-game void has no planet yet, a later request), [[art-style-guide]] (A1 to A14: facets, faction contrast, A12 steep camera for scene art, A14 no UI and no text in reference art), [[color-code]] (void clouds indigo, violet, magenta, ultramarine; Rigs white + teal + rust; robots red eyes, yellow logo plate), [[art-prompts]] (locked phrases, keep-this-image loop, validated images keyart.jpg, rigs-lineup.jpg, enemies-lineup.jpg), [[agent-art]] and [[kie-image-workflow]] (GPT Image 2, 16:9 allowed, resolution capped at 1K by the owner 2026-10-11, about 6 credits per image), [[milestones]] (a main menu is in no milestone). Conflicts: A12 asks a steep near-vertical camera for scene art, a menu backdrop may want a lower angle (Q3); the 1K cap gives a soft image at 1920 x 1080 (Q5). Open values: Q1 to Q6 and Q11 of the plan, asked 2026-10-28 with defaults; answered "ok" 2026-10-28, see request. Questions Q1 to Q11 (shared with [[task-338-main-menu-screen]]), each with its proposed default:
  - Q1 split: two global tasks, 337 background image (art), 338 menu screen (cpp, bp, capture, doc); 338 step 2 waits for the image picked in 337. Alternative: one global task.
  - Q2 image content: key art style scene; the miner and his drone at the cliff edge of the arena asteroid, facing the void (indigo, violet, magenta nebula, stars, floating asteroids, a distant planet as in keyart.jpg, image only, the in-game void stays without planet); violet Duskite crystals in the rocks; a few corporate robots (yellow chasers, red eyes) coming over the rocks behind; left third calmer and darker for the menu column; no text, no UI.
  - Q3 camera: three-quarter view lower than the game camera (about 35 degrees), more cinematic; the A12 steep camera stays the rule for gameplay scene art. Alternative: steep near-vertical camera as keyart.jpg.
  - Q4 route: art agent, Kie.ai GPT Image 2 image-to-image from keyart.jpg, rigs-lineup.jpg, enemies-lineup.jpg, 16:9; one run of 3 variants then at most 2 correction passes of one image (about 30 credits at most). Alternative: prompts handed to the owner for ChatGPT.
  - Q5 resolution: 1K cap kept, image stretched to the screen (slightly soft behind the menu). Alternative: lift the cap to 2K for this image if Kie.ai offers it.
  - Q6 title: "ROGUE RIGS" written by the widget (Roboto Bold), never in the image; no game logo in this request.
  - Q7 layout: left column, title then New Run, Options, Exit; English labels exactly these, hardcoded in the widget (not screen_texts.json); white text with outline, orange 1/0.35/0.1 on hover and focus as WBP_Loading; full-screen static image with a dark gradient behind the column; no animation, no music.
  - Q8 buttons: New Run resets the run and opens L_ArenaM1 at level 1; Options shown greyed out, does nothing; Exit quits the game; mouse cursor shown, keyboard focus on New Run.
  - Q9 start-up: the -game launch and the packaged game open L_MainMenu (GameDefaultMap); the editor and PIE keep L_ArenaM1 and start a run directly; no "back to menu" button on end screens or pause in this request.
  - Q10 check: the cpp agent adds rr.Menu.NewRun and rr.Menu.Exit debug commands, used by the final -game capture (menu at 1920 x 1080 sent to the owner).
  - Q11 milestone: in no milestone; filed under M3 (polish, November 9 submission).
- confirmed_by_owner: yes (2026-10-28, "ok" to Q1 to Q11, relayed by the main session)
- milestone: M3
- next_step: none
- started: 2026-10-28T01:53:18Z
- waiting_for: none
- executor_tokens: input 54, cache write 133036, cache read 886101, output 12505
- executor_usage: runs 1, turns 24, context start 16k peak 57k, resumes 2, 29 min, units 0.32 M
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Done. Owner kept variant 1 (run 20261027-172530) as vault/assets/main-menu-background.png; 24 credits over 2 runs (18 + 6 unused correction pass 1 made on variant 3). Open style-guide fails of variant 1 accepted for now: A3 squat chasers, A13 floor, cracks on rock tops, A5 worn chasers, lit slope in left third, drone thruster flames.

## Subtasks
1. task-337-01-main-menu-background (vault/reference/tasks/task-337-main-menu-background/task-337-01-main-menu-background.md) — art: Main menu background image: one 16:9 scene in the validated key art style, generated through Kie.ai, picked by the owner.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-337-01-main-menu-background]] — step 1, art.
- [[task-338-main-menu-screen]] — the menu screen that shows the picked image.
