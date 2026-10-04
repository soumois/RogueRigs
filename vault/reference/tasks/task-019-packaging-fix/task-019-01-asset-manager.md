---
id: task-019-01-asset-manager
description: "Add the GameFeatureData primary asset type (or the right fix) in config so BuildCookRun exits 0; verify the staged exe runs."
parent: task-019-packaging-fix
kind: task
status: in-progress
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
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-019-packaging-fix]] — parent global task.
