---
id: miner
description: "The player character: free movement, mouse or stick aim, auto-firing weapons, turret button, damage rules, look and asset source."
parent: player-index
kind: spec
status: approved
---
# Miner

- Moves freely; aims with the mouse or the right stick.
- Every equipped weapon fires automatically toward the aim, each at its own rate ([[upgrades]] change the rates).
- A button places a turret on the aimed spot, using the turret launcher ([[turret]]).
- Cannot shoot through or over rock blocks ([[destruction-rules]]).
- Standing against a block makes the [[drone]] dig it.
- Explosions never one-shot the miner ([[explosions]]).
- Invulnerability frames after a hit: 0.25 s, the miner blinks (provisional, owner 2026-10-02).
- Hit shake (provisional, owner 2026-11-02, [[task-417-camera-shake-on-damage]]): every hit that takes health from the miner, the lethal one included, shakes the camera: 5 uu, 0.2 s, same strength whatever the damage; a hit refused by the i-frames does not shake. Mixing: [[visual-style]].
- Aim assist (provisional, owner 2026-11-05, [[task-463-aim-assist]]): light; a burst rifle bullet (each bullet of the burst, main shot only, not the 3 extra shots of 4-direction fire) is deflected at most 5 degrees toward a robot; candidates are robots within the weapon range, with no rock block between the Muzzle and the robot, within 5 degrees of the shot direction; the one needing the smallest correction is chosen and the bullet leaves straight toward its current position (no lead). Barrels are never assist targets. The assist does nothing when the shot as fired (after spread, with its real collision size, within range) would first hit a robot or a barrel. Same for mouse and stick; always on, no menu option; the angle is a per-weapon data value, 0 = off (shotgun and turret 0). The aim laser keeps showing the raw aim ([[aim-indicator]]); no visual feedback of the assist.
- Stats: see [[player-stats]].
- Healing: only from heal pickups dropped by heal ore ([[heal-ore]]).
- Death ends the run ([[run-structure]]). M1 (single arena): controls cut, "Game Over" text, the level restarts after 3 s (owner 2026-10-02).
- Look: plate armor, white + teal + rust, patched salvage ([[color-code]]).
- Asset: the owner makes the 3D mesh himself from a new turnaround sheet generated in ChatGPT (owner 2026-10-29, task-364; prompt in art-prompts-miner); rig: Meshy auto-rig made by the owner, Mixamo-named humanoid skeleton (`mixamorig:*`, fingers included), delivered 2026-10-30 as `Meshy_AI_Character_output.fbx` (task-382, files in art-prompts-miner); animation: Mixamo clips on this skeleton, played in place by a new ABP_Miner, movement from gameplay (owner 2026-10-30, task-386); Unreal import and replacement of the Quinn mannequin in task-386; triangle budget 25,744 as delivered (provisional, owner 2026-10-30); weapon hand socket `WeaponSocket_R` on the right hand bone of SKEL_Miner, burst rifle only, left hand placed by IK on the rifle handguard (provisional, owner 2026-10-30, tasks 400 and 401); upper body from bone `Spine` up plays the idle rifle pose over the Mixamo locomotion legs (Layered blend per bone), its source a single value to swap for another weapon later (owner 2026-10-31, task-406); the upper body plays the created animation A_Miner_UpperRifle_V2Grip (V2, chosen by the owner 2026-10-31 in task-408, with the right hand moved onto the rifle pistol grip and the right elbow kept at its V2 height as far as the arm allows, provisional, owner 2026-10-31, task-409) over the locomotion legs; reference `vault/assets/rigs-lineup.jpg`.
- Milestone: M1.

## Relations
- [[player-index]] — parent index.
- [[drone]] — the miner's helper.
- [[aim-indicator]] — aim laser from the rifle muzzle showing the aim direction.
