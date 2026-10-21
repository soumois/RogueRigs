---
id: task-273-04-floor-capture
description: "-game captures of the floor before and after the new material at the task-272 frames, ring luminance measured against the DRGS profile."
parent: task-273-floor-texture
kind: task
status: done
---
# Floor capture

- goal: Side by side before and after `-game` captures of the floor at the same frames as task-272, with the ring luminance (far floor and 350 uu) measured against the DRGS ring profile.
- agent: cpp
- depends_on: task-273-03-floor-material
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), the task-272 capture procedure given in the delegation
- expected_output: `Saved/LookTune/floor_273_*.png` before, after and side by side; numbers in the result.
- done_when: Captures exist; far floor and 350 uu values reported; no light, exposure or post-process changed; images sent to the owner by the Orchestrator.
- editor_state: closed
- token_ceiling: 40k
- model: opus
- tokens: input 64, cache write 91777, cache read 1690613, output 6264
- runs: 1
- milestone: M1
- result: No new run: r9 (old floor, old light) vs r11 (new floor, task-274 light). Side by sides Saved/LookTune/floor_273_{A_idle,F_deposits,B_wall,E_xp,H_after}.png (r9 | r11 on | r11 Enable 0), crop floor_273_crop.png (no tiling seam). r11 A_idle_on floor only: 350 uu 37.4/255 (target 39, ok), far floor beyond 600 px 1.8/255 (target 9-12, too dark: darkener falloff and exposure, handled in task-274). Normal sign: HLSL of M_FloorPlates ported and registered on r11 Enable 0 frames (corr 0.68); sun direction (-0.639, 0.116, 0.760); as-built normal correlates positively, red flip negatively on A and F; green agrees on A, inconclusive on F; bake and material chain give (-dH/dX, -dH/dY, 1): no flip. Relief signal weak (corr 0.06), most crack shading is painted in the albedo.

## Relations
- [[task-273-floor-texture]] — parent global task.
