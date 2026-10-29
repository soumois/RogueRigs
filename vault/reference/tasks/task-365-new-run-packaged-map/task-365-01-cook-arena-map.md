---
id: task-365-01-cook-arena-map
description: "Add both maps to MapsToCook in DefaultGame.ini, rebuild the Development package, check New Run opens L_ArenaM1 in it."
parent: task-365-new-run-packaged-map
kind: task
status: in-progress
---
# Cook L_ArenaM1 into the package and verify New Run

- goal: In Config/DefaultGame.ini, section [/Script/UnrealEd.ProjectPackagingSettings], add `+MapsToCook=(FilePath="/Game/RogueRigs/Maps/L_MainMenu")` and `+MapsToCook=(FilePath="/Game/RogueRigs/Maps/L_ArenaM1")`. Rebuild the staged Development game with the build-toolchain BuildCookRun command, but with `-map=/Game/RogueRigs/Maps/L_MainMenu+/Game/RogueRigs/Maps/L_ArenaM1` instead of L_ArenaM1 alone. Launch `Saved/StagedBuilds/Windows/RogueRigs.exe -windowed -log -ExecCmds="rr.Menu.NewRun"` with no map argument (it starts on L_MainMenu) and read the log in Saved/StagedBuilds/Windows/RogueRigs/Saved/Logs/. No C++ change unless the owner asks for it (Q3). Do not touch, revert or commit Content/RogueRigs/UI/WBP_ShopCard.uasset and WBP_ShopScreen.uasset (owner hand edits, uncommitted). Return a change summary (files, command used, cook time, log lines) for the doc subtask; do not write vault doc nodes.
- agent: cpp
- depends_on: none
- inputs: sys-main-menu (vault/reference/tech/systems/ui/sys-main-menu.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), main-menu (vault/reference/gdd/run/main-menu.md), `Config/DefaultGame.ini`, `Source/RogueRigs/Menu/RRMainMenuWidget.cpp`, `Source/RogueRigs/Run/RRRunSettings.h`, `Saved/StagedBuilds/Windows/Manifest_UFSFiles_Win64.txt` (evidence: the 06:36 build lists only L_MainMenu)
- expected_output: Config/DefaultGame.ini with the two MapsToCook lines, committed by path; a fresh Development package in Saved/StagedBuilds/Windows (not committed); a change summary with the log lines.
- done_when: BuildCookRun exits 0; the new Saved/StagedBuilds/Windows/Manifest_UFSFiles_Win64.txt (or the cook log) lists L_MainMenu and L_ArenaM1; the staged game launched with no map argument logs `main menu shown (WBP_MainMenu_C).`, then `Main menu New Run: run reset, opening /Game/RogueRigs/Maps/L_ArenaM1 (level 1)`, a LoadMap of L_ArenaM1 and `run started, level 1/4`, with no travel failure line; DefaultGame.ini committed by path.
- editor_state: closed (packaging and ini edit; the editor can rewrite config files)
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-29T18:25:40Z
- waiting_for: none
- result: open

## Relations
- [[task-365-new-run-packaged-map]] — parent global task.
