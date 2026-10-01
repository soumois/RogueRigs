---
id: build-toolchain
description: "Build requirements and commands: UE 5.8, MSVC 14.50, command-line build for agents, editor launch, plugins, regenerating project files."
parent: pipelines-index
kind: doc
---
# Build and toolchain

- Engine: Unreal Engine 5.8. Project from the Top Down template (C++); Twin Stick variant as the base. The Strategy variant and the root top-down classes (`RogueRigsCharacter`, `RogueRigsGameMode`, `RogueRigsPlayerController`) are deleted ([[task-002-04-delete-template-cpp]]); `Source/RogueRigs` holds the module files and one folder per system: `Damage/`, `Debug/`, `Drone/`, `GAS/`, `Grid/`, `Pickups/`, `Player/`, `Projectiles/`, `Tests/`, `Weapons/`, plus `Variant_TwinStick/` (reference only). The `Niagara` module is no longer a C++ dependency (the plugin stays enabled).
- Compiler: MSVC 14.50 (Visual Studio 2026, workload "Game development with C++"). MSVC 14.40–14.43 are rejected by UE 5.8.
- After a toolchain change: right-click `RogueRigs.uproject` → Generate Visual Studio project files.
- Command-line build, editor closed (used by agents; exit code 0 = success, compiler errors on stdout):
  `"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" RogueRigsEditor Win64 Development -Project="C:\Users\soumo\Documents\Unreal Projects\RogueRigs\RogueRigs.uproject" -WaitMutex`
  From Git Bash, run it through PowerShell (verified in task-001-03 and task-002-04):
  `powershell.exe -NoProfile -Command "& 'C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat' RogueRigsEditor Win64 Development '-Project=C:\Users\soumo\Documents\Unreal Projects\RogueRigs\RogueRigs.uproject' -WaitMutex"`
  UBT currently picks MSVC 14.51 and warns it is not a preferred version; the build still succeeds. Expected engine-header warnings: C4996 from `Character.h` (`GetMovementBase`) and `HierarchicalInstancedStaticMeshComponent.h` (`BuildComponentInstanceData`).
- Headless automation tests, editor closed, after a successful build (verified in task-006-05; exit code 0 = all passed; results in `Saved/Logs/RogueRigs.log`, lines `Test Completed. Result={Success|Fail}` and `TEST COMPLETE. EXIT CODE`):
  `"C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "C:\Users\soumo\Documents\Unreal Projects\RogueRigs\RogueRigs.uproject" -ExecCmds="Automation RunTests RogueRigs.Dig;Quit" -unattended -nullrhi -nosplash -nop4 -log`
  From Git Bash, through PowerShell with a timeout:
  `timeout 900 powershell.exe -NoProfile -Command "& 'C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor-Cmd.exe' 'C:\Users\soumo\Documents\Unreal Projects\RogueRigs\RogueRigs.uproject' '-ExecCmds=Automation RunTests RogueRigs.Dig;Quit' -unattended -nullrhi -nosplash -nop4 -log; exit \$LASTEXITCODE"`
  Replace `RogueRigs.Dig` by `RogueRigs.Weapons` for the weapon tests, or by `RogueRigs` to run every project test. Tests: [[sys-automation-tests]].
- Launch the editor on the project:
  `"C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe" "C:\Users\soumo\Documents\Unreal Projects\RogueRigs\RogueRigs.uproject"`
- Plugins: GameplayAbilities is enabled in `RogueRigs.uproject`; `GameplayAbilities`, `GameplayTags`, `GameplayTasks` are public module dependencies of `RogueRigs.Build.cs` ([[task-004-01-enable-gas]]). StateTree is already enabled by the template.
- `Source/RogueRigsEditor` does not exist (only `RogueRigsEditor.Target.cs`); create it only if the Slate debug tool returns ([[wishlist]]).
- Rendering settings: see [[perf-risks]].

## Relations
- [[pipelines-index]] — parent index.
- [[editor-workflow]] — when to compile with the editor closed.
- [[sys-automation-tests]] — the tests the headless command runs.
