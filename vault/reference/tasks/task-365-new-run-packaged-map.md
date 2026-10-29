---
id: task-365-new-run-packaged-map
description: "Global task: Fix New Run on the main menu doing nothing: the packaged build cooks only L_MainMenu, so the travel to L_ArenaM1 fails."
parent: tasks-to-do-index
kind: task
status: todo
---
# New Run does nothing in the packaged game

- request: Owner, 2026-10-29, verbatim: "Lorsque je clique sur new run sur le main menu, mais rien ne se passe." Meaning: clicking New Run on the main menu does nothing. Planner diagnosis (2026-10-29, read-only): WBP_MainMenu.uasset does bind NewRunButton OnClicked to StartNewRun (BndEvt NewRunButton OnButtonClickedEvent, CallFunc_StartNewRun found in the asset); URRMainMenuWidget::StartNewRunInWorld calls ResetRun then UGameplayStatics::OpenLevel on URRRunSettings::NewRunMapName (/Game/RogueRigs/Maps/L_ArenaM1, no ini override). The only packaged build on disk, Saved/StagedBuilds/Windows (Shipping, staged 2026-10-29 06:36, packaged from the editor), lists one game map in Manifest_UFSFiles_Win64.txt: RogueRigs/Content/RogueRigs/Maps/L_MainMenu.umap; L_ArenaM1 is not cooked. Config/DefaultGame.ini [/Script/UnrealEd.ProjectPackagingSettings] has no MapsToCook, and L_ArenaM1 is referenced only by a string in C++, so the cooker took only GameDefaultMap (L_MainMenu, set by task-338). OpenLevel to a missing map fails silently in Shipping (no log), and bNewRunRequested stays true so later clicks are ignored: nothing happens. No editor or -game log on disk shows a menu click. Diagnosis hypothesis to confirm with Q1. Owner answer 2026-10-29 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q1, Q2, Q3 accepted.
- goal: A package of the game opens L_ArenaM1 at level 1 when New Run is pressed on the main menu: both maps are cooked through MapsToCook in Config/DefaultGame.ini, the package is rebuilt and the travel is verified in it.
- acceptance: Config/DefaultGame.ini [/Script/UnrealEd.ProjectPackagingSettings] lists MapsToCook L_MainMenu and L_ArenaM1; a fresh package in Saved/StagedBuilds/Windows contains both maps (staged manifest or cook log); launched with no map argument it logs `main menu shown (WBP_MainMenu_C).`, then rr.Menu.NewRun logs `Main menu New Run: run reset, opening /Game/RogueRigs/Maps/L_ArenaM1 (level 1)`, the L_ArenaM1 load and `run started, level 1/4`; build-toolchain and sys-main-menu document the cook rule and the packaging command; not seen by agents: a real mouse click on New Run in the package (owner check); lint clean
- gdd_check: Read main-menu (spec provisional, owner 2026-10-28), sys-main-menu, wbp-main-menu, bp-main-menu-game-mode, build-toolchain (2026-10-29). No conflict: main-menu Behaviour says New Run resets the run and opens L_ArenaM1 at level 1, and a launch outside the editor (-game, packaged) opens L_MainMenu; the package must therefore contain both maps. No value the work needs is open. sys-main-menu lists "packaged build" and "real mouse click" as not seen in task-338: this is that gap. build-toolchain documents a Development BuildCookRun with -map=/Game/RogueRigs/Maps/L_ArenaM1 only, which would now drop L_MainMenu. In milestone scope (M2).
- confirmed_by_owner: yes (2026-10-29, "ok")
- milestone: M2
- next_step: task-365-01-cook-arena-map
- started: open
- waiting_for: none
- result: open

## Questions
Decided 2026-10-29 (owner "ok"): every default below is accepted; values provisional, owner 2026-10-29.
- Q1 Where did you click New Run: in the packaged game Saved/StagedBuilds/Windows/RogueRigs.exe (packaged this morning 06:36), or in the editor (Play, or Standalone Game)? Default: in the packaged game; the plan above fixes that case. If it was in the editor, the cause is elsewhere and the planner re-plans.
- Q2 Package used to verify the fix: Development (documented command in build-toolchain, keeps logs and rr.* commands so the agent can prove the travel; it overwrites Saved/StagedBuilds/Windows), or Shipping like your build (no log, only you can check by clicking). Default: Development; you then click New Run in it yourself.
- Q3 Extra safety in C++ (not requested): if the map is missing, log an error and re-enable the button instead of ignoring later clicks. Default: no, cook fix only.

## Subtasks
1. task-365-01-cook-arena-map (vault/reference/tasks/task-365-new-run-packaged-map/task-365-01-cook-arena-map.md) — cpp: add MapsToCook L_MainMenu and L_ArenaM1, repackage Development, verify New Run travels in the package
2. task-365-02-packaging-doc (vault/reference/tasks/task-365-new-run-packaged-map/task-365-02-packaging-doc.md) — doc: update build-toolchain (MapsToCook, packaging command with both maps) and sys-main-menu (packaged New Run verified, cook rule)

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-365-01-cook-arena-map]] — step 1, cpp.
- [[task-365-02-packaging-doc]] — step 2, doc.
- [[main-menu]] — the design: New Run opens L_ArenaM1 at level 1.
- [[sys-main-menu]] — the C++ that travels on NewRunMapName.
- [[build-toolchain]] — the packaging command to update.
