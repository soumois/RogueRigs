---
id: task-002-04-delete-template-cpp
description: "Delete the Strategy variant and root top-down C++ classes, clean Build.cs include paths, rebuild, and update build-toolchain."
parent: task-002-cleanup-twinstick
kind: task
status: in-progress
---
# Delete template C++

- goal: only the TwinStick variant, the grid and the module files remain in `Source/RogueRigs`, and the project builds.
- agent: cpp
- depends_on: task-002-02-delete-template-content
- inputs: [[build-toolchain]], [[cpp-standard]], `Source/RogueRigs/RogueRigs.Build.cs`.
- expected_output: deleted: `Source/RogueRigs/Variant_Strategy/` (all), `RogueRigsCharacter.h/.cpp`, `RogueRigsGameMode.h/.cpp`, `RogueRigsPlayerController.h/.cpp`. Kept: `RogueRigs.h/.cpp` (module and `LogRogueRigs`), `Grid/`, `Variant_TwinStick/`. `RogueRigs.Build.cs`: remove the `Variant_Strategy` include paths and any module used only by deleted code (check before removing). Build.cs change allowed for those removals only. Remove the three `+ActiveClassRedirects` lines of `Config/DefaultEngine.ini` that map `TP_TopDown*` to the deleted root classes (config change allowed for those lines only). Add `[CoreRedirects]` only if a kept asset still points to a deleted class (report it). [[build-toolchain]] updated: Strategy variant deleted; from Git Bash, run `Build.bat` through `powershell.exe -NoProfile -Command "& '...Build.bat' ..."` (verified in task-001-03).
- done_when: command-line build exits 0 with the editor closed; no new warnings in touched files; no remaining include of a deleted header (grep in `Source/`); lint clean.
- editor_state: closed
- token_ceiling: 40000
- milestone: M1
- result: open

## Relations
- [[task-002-cleanup-twinstick]] — parent global task.
