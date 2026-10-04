---
id: task-019-01-asset-manager
description: "Add the GameFeatureData primary asset type (or the right fix) in config so BuildCookRun exits 0; verify the staged exe runs."
parent: task-019-packaging-fix
kind: task
status: done
---
# Asset manager config

- goal: packaging works end to end.
- agent: cpp
- depends_on: none
- inputs: [[build-toolchain]], task-018-04 result (staging rerun workaround), `Config/DefaultGame.ini`.
- expected_output: config fix (e.g. `+PrimaryAssetTypesToScan` for GameFeatureData in `[/Script/Engine.AssetManagerSettings]`, or disabling an unused plugin if that is the real cause; explain the choice); full BuildCookRun (build, cook, stage, pak, Win64 Development) exits 0; staged exe launched on L_ArenaM1 for a few seconds with no error in its log; [[build-toolchain]] updated (single command, no rerun workaround). Build outputs not committed.
- done_when: BuildCookRun exits 0; staged run log clean; `RogueRigs.*` tests still pass if any C++ or config affecting the editor changed; lint clean.
- editor_state: closed
- token_ceiling: 50000
- model: opus
- tokens: input 38, cache write 35622, cache read 390184, output 7630
- runs: 1
- milestone: M1
- result: DefaultGame.ini: GameFeatureData primary asset type pointing at an empty /Game/Unused folder (engine-suggested fix; the GameFeatures plugin is pulled in by an enabled plugin, probably the MCP toolsets). Full BuildCookRun (build, cook, stage, pak, Win64 Development) exit 0; staged exe ran L_ArenaM1 20 s with no error; 101/101 tests. build-toolchain: single command, rerun workaround removed. Commit 2b46961.

## Relations
- [[task-019-packaging-fix]] — parent global task.
