---
id: task-274-01-mask-writer-cleanup
description: "URRPlayerLightMaskComponent no longer names CenterBrightness, TintStrength, WarmTint or FarTint; it pushes Enable, FarBrightness, Radius, Exponent only when bDriveTunables."
parent: task-274-flat-light-darkening-shader
kind: task
status: done
---
# Light mask writer cleanup

- goal: URRPlayerLightMaskComponent no longer names CenterBrightness, TintStrength, WarmTint or FarTint; it pushes Enable, FarBrightness, Radius, Exponent only when bDriveTunables.
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-player (vault/reference/tech/systems/core/sys-player.md), Source/RogueRigs/World/RRPlayerLightMaskComponent.h and .cpp
- expected_output: C++ change, build, sys-player updated.
- done_when: No removed name left in Source; build succeeds; tests touching the component pass; sys-player updated; committed by path.
- editor_state: closed
- token_ceiling: 30k
- model: opus
- tokens: input 18, cache write 28382, cache read 164889, output 3667
- runs: 1
- milestone: M1
- result: Commit 3544b952: CenterBrightness, TintStrength, WarmTint, FarTint removed from URRPlayerLightMaskComponent; console tunables Enable 1, FarBrightness 0.005, Radius 200, Exponent 3 (pushed only with bDriveTunables); PlayerPosition always written; build succeeded; no test touches the component; sys-player updated.

## Relations
- [[task-274-flat-light-darkening-shader]] — parent global task.
