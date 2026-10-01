---
id: task-002-cleanup-twinstick
description: "Global task: remove the TopDown and Strategy template code and content, play the M1 arena with the TwinStick character, rock coverage 60 percent."
parent: tasks-index
kind: task
status: in-progress
---
# Project cleanup, TwinStick character, denser arena

- request: "Avant, il faudrait faire un nettoyage du projet. [...] garder ce qui concerne Variant_TwinStick pour le déplacement (utiliser le BP_TwinStickCharacter pour l'instant au lieu du BP_TopDownCharacter). [...] quant à la couverture des rochers, passe-la à 60 %." Owner answers: keep all of Variant_TwinStick for now; delete TopDown and Strategy as proposed; keep HISM for the grid.
- goal: the project holds only the TwinStick template, the shared mannequin and LevelPrototyping assets, and RogueRigs content; `L_ArenaM1` is the startup map and plays with `BP_TwinStickCharacter` on a 60 % rock layout.
- acceptance: (1) no `Content/TopDown`, `Content/Variant_Strategy`, `Content/Cursor`, `Source/RogueRigs/Variant_Strategy` or root top-down classes (`RogueRigsCharacter`, `RogueRigsGameMode`, `RogueRigsPlayerController`); (2) command-line build exits 0 with the editor closed; (3) the editor opens on `L_ArenaM1` without missing-asset or missing-class errors; (4) `arena_m1` passes the validator with block coverage in [0.57, 0.63]; (5) in PIE, `BP_TwinStickCharacter` spawns on the spawn cell and moves with keyboard input; (6) lint clean, every subtask committed.
- gdd_check: read [[miner]] (moves freely, aims with mouse or right stick: matches TwinStick), [[build-toolchain]] (TwinStick is the base, Strategy to delete), [[sys-grid]] (HISM kept, owner confirmed). No conflict. Rock coverage 60 % of the asteroid: owner decision, replaces the 40 % of task-001.
- confirmed_by_owner: yes
- milestone: M1
- next_step: task-002-04-delete-template-cpp
- result: open

## Subtasks
1. task-002-01-twinstick-defaults — bp: `L_ArenaM1` and project defaults use `BP_TwinStickGameMode`; startup maps = `L_ArenaM1`.
2. task-002-02-delete-template-content — bp: delete TopDown, Strategy and Cursor content, fix redirectors.
3. task-002-03-layout-rock-60 — level-design: `arena_m1` at about 60 % rock.
4. task-002-04-delete-template-cpp — cpp: delete Strategy and root top-down C++, clean Build.cs, rebuild, update docs.
5. task-002-05-verify-pie — bp: editor opens clean on `L_ArenaM1`; PIE check of the TwinStick character on the new layout.

## Relations
- [[tasks-index]] — parent index.
- [[task-002-01-twinstick-defaults]] — step 1, bp, game mode and default maps.
- [[task-002-02-delete-template-content]] — step 2, bp, content deletion.
- [[task-002-03-layout-rock-60]] — step 3, level-design, denser layout.
- [[task-002-04-delete-template-cpp]] — step 4, cpp, code deletion and rebuild.
- [[task-002-05-verify-pie]] — step 5, bp, final editor and PIE check.
