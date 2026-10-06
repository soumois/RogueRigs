---
id: task-032-02-bp-fixes
description: "WBP_HUD edge arrow to the console then the shuttle, Blueprint texts in English, turret drawn 50 % larger; PIE check."
parent: task-032-third-test
kind: task
status: todo
---
# Third test: Blueprint fixes

- goal: the Blueprint side of the owner's third play test.
- agent: bp
- depends_on: task-032-01-cpp-fixes
- inputs: [[wbp-hud]], [[sys-hud]], [[bp-turret]], [[bp-miner]], [[objective-console]], [[readability-rules]], [[unreal-mcp]], [[sys-cheats]]
- expected_output: WBP_HUD locator arrow on the screen edge driven by the task-032-01 getter (hidden when the target is on screen or there is none; console colour, then shuttle colour); every Blueprint text listed by task-032-01 translated to English (WBP_HUD, BP_Miner weapon catalogue descriptions, end screens, any other listed asset); BP_Turret visual scale × 1.5 (mesh only, no gameplay range change); docs wbp-hud, bp-turret, bp-miner and any other changed Blueprint; PIE: no log error, arrow visible with the console off screen (skip-to-console cheat), turret scale read back.
- done_when: PIE check with no error; docs updated; lint clean.
- editor_state: open
- token_ceiling: 110000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-032-third-test]] — parent global task.
