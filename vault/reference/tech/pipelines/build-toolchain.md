---
id: build-toolchain
description: "Build requirements and commands: UE 5.8, MSVC 14.50, command-line build for agents, editor launch, plugins, regenerating project files."
parent: pipelines-index
kind: doc
---
# Build and toolchain

- Engine: Unreal Engine 5.8. Project from the Top Down template (C++); Twin Stick variant as the base; the Strategy variant is to be deleted once the Twin Stick build works.
- Compiler: MSVC 14.50 (Visual Studio 2026, workload "Game development with C++"). MSVC 14.40–14.43 are rejected by UE 5.8.
- After a toolchain change: right-click `RogueRigs.uproject` → Generate Visual Studio project files.
- Command-line build, editor closed (used by agents; exit code 0 = success, compiler errors on stdout):
  `"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" RogueRigsEditor Win64 Development -Project="C:\Users\soumo\Documents\Unreal Projects\RogueRigs\RogueRigs.uproject" -WaitMutex`
  Path follows the standard Epic launcher layout; not verified on the machine yet.
- Launch the editor on the project:
  `"C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe" "C:\Users\soumo\Documents\Unreal Projects\RogueRigs\RogueRigs.uproject"`
- Plugins: GameplayAbilities must be enabled ([[task-000-enable-gas]]). StateTree is already enabled by the template.
- `Source/RogueRigsEditor` does not exist (only `RogueRigsEditor.Target.cs`); create it only if the Slate debug tool returns ([[wishlist]]).
- Rendering settings: see [[perf-risks]].

## Relations
- [[pipelines-index]] — parent index.
- [[editor-workflow]] — when to compile with the editor closed.
