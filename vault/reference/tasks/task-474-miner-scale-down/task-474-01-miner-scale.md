---
id: task-474-01-miner-scale
description: "Bp: The miner mesh is shown at 90 % with every attachment and effect following, collision and gameplay unchanged."
parent: task-474-miner-scale-down
kind: task
status: done
---
# Miner 10 % smaller: bp

- goal: The miner mesh is shown at 90 % with every attachment and effect following, collision and gameplay unchanged.
- agent: bp
- depends_on: none
- inputs: miner (vault/reference/gdd/player/miner.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), visual-style (vault/reference/gdd/art-audio/visual-style.md)
- expected_output: see done_when
- done_when: A1 to A7 hold in PIE and a -game frame; before and after height recorded in the result; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-474
- usage: included in the executor run of task-474
- runs: 1
- milestone: M2
- started: 2026-11-06T17:04:21Z
- waiting_for: none
- result: 39732c16. BP_Miner CharacterMesh0 RelativeScale3D 1 -> 0.9 (location z -88, yaw -90 kept; feet at the mesh origin stay on the floor). Height top of head at rest (SK_Miner.fbx vertices in Blender, bind pose and A_Miner_Idle / V2Grip frames): before 170.0 uu (idle 169.97-170.49), after 153.0 uu (x0.9 exact, uniform component scale). A2: capsule half height 88 radius 34 read back unchanged; no gameplay value touched. A3: RRMiner.cpp hand-socket attach SnapToTargetNotIncludingScale -> SnapToTargetIncludingScale (executor choice: the old rule kept the rifle at world scale 1 with relative 1/0.9, which also breaks the left-hand IK grip math that assumes relative 1); PIE read-back: BP_BurstRifle world scale 0.9, root relative 1, Muzzle about 110 uu above capsule bottom vs about 122 before (above ShotFloorClearance, no low-Muzzle warning); hand offset and counter-move scale with it. Damage, heal, level-up number heights are capsule based (40 uu over), unchanged. A4: camera, drone, BP_Drone untouched. A5: FloorWallLight untouched. A6: ABP unchanged. A7: build OK; RogueRigs.Weapons 16/17 pass, ImpactSparkAsset fails on the rifle projectile spark asset (not on attach or scale, pre-existing, not fixed); PIE log no new error. -game frames: Saved/Captures/MinerScale474Before, MinerScale474After.

## Relations
- [[task-474-miner-scale-down]] — parent global task.
