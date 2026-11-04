---
id: task-425-02-miner-point-light
description: "Bp: point light on BP_Miner, lighting channel 1 only"
parent: task-425-miner-floor-wall-light
kind: task
status: done
---
# Miner light that lights only the floor and the walls: bp

- goal: Add a PointLightComponent `FloorWallLight` to BP_Miner under the capsule: LightingChannels channel0 false, channel1 true; castShadows per Q1 (default false); colour per Q2 (default 255/194/158); mobility Movable; attenuation radius and intensity chosen for Q3 (start from the bp-miner task-272-02 recipe, height about 450 uu, adjust so the pool is about 400 uu radius and the centre floor stays below about 205/255). Compile, save, read the values back.
- agent: bp
- depends_on: task-425-01-grid-lighting-channels
- inputs: bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), vault/reference/tasks/task-425-miner-floor-wall-light.md (request, Questions, acceptance)
- expected_output: BP_Miner.uasset with the light, values read back, change summary (before and after values) for the doc subtask, commit.
- done_when: Component present on BP_Miner with channel0 false, channel1 true and the decided colour and shadow flag, read back after compile and save; committed.
- editor_state: Unreal editor open with the MCP server.
- token_ceiling: 1.0 M units
- model: sonnet
- tokens: input 50, cache write 91776, cache read 847902, output 13737
- usage: turns 25, context start 11k peak 55k, resumes 0, 3 min, units 0.27 M
- runs: 1
- milestone: M2
- started: 2026-11-04T16:58:08Z
- waiting_for: none
- result: Done: FloorWallLight PointLight on BP_Miner (channel 1 only, no shadow, 255/194/158, Movable, intensity 4 unitless, radius 650, falloff exp 4, z+360); values read back; commit f2c79f4c. Intensity is a first guess, render unverified.

## Relations
- [[task-425-miner-floor-wall-light]] — parent global task.
