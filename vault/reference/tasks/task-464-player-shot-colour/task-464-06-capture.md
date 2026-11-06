---
id: task-464-06-capture
description: "Capture: Before/after capture of a fight with rifle, turret, shotgun and enemy shots and flashes, Shooter present, sent to the owner"
parent: task-464-player-shot-colour
kind: task
status: done
---
# Player shot colour: capture

- goal: Before/after capture of a fight with rifle, turret, shotgun and enemy shots and flashes, Shooter present, sent to the owner
- agent: capture
- depends_on: task-464-05-doc
- inputs: color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), Art/Source/muzzle-flash/owner-ref-green-laser-muzzle-2026-11-05.webp
- expected_output: see done_when
- done_when: Frames before and after in the same fight show: C1 rifle laser shots and impact sparks in varied green; C2 turret shots and impact sparks in the same green; C3 rifle and turret muzzle flashes green with several tones, compared with the reference, the turret flash at its barrel; C4 shotgun pellets and shotgun flash white-gold as before; C5 enemy shots and targeting lasers red; C6 the acid green Shooter stands out among the green shots; C7 mining and aim lasers white-gold; C8 green shots distinct from heal, XP and ore pickups. Frames sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-464
- usage: included in the executor run of task-464
- runs: 4
- milestone: M2
- started: 2026-11-06T09:18:29Z
- waiting_for: none
- result: Done, 4 capture launches (task464_after, task464_after_slow, task464_after_shotgun, task464_after_slow2) against the before run task464_before taken at the start with the same fight commands (rr.Debug.AddWeapon shotgun and two turrets, 2 Shooters, 8 Chasers, aim yaw 0). Sheets for the owner: Saved/Captures/task464_compare_fight.png, task464_compare_crop.png, task464_after_details.png. C1 seen for the shots (green dashes with pale centre, were gold), sparks not seen at game size (1-2 uu, as before; one green spark seen in 464-01). C2 seen (turret bolts green). C3 seen (rifle and turret flashes green, pale core, bright blades, seven-spike star, soft dark green glow; turret flash at its barrel). C4 seen: shotgun pellets and flash white-gold; shotgun sparks not seen. C5 seen: red targeting lasers, red robot shot. C6 seen but for the owner to judge: Shooter yellow-lime body stands apart from the small pure green shots. C7 aim laser white-gold seen (task-469 not run yet); drone mining laser not in frame. C8 green shots distinct from cyan XP pickups; heal and ore pickups not in frame. Defect found and fixed once (commit d719a4cf): the dark green glow of M_MuzzleFlashGreen showed the square sprite edge; the glow now fades before the border, rechecked in task464_after_slow2.

## Relations
- [[task-464-player-shot-colour]] — parent global task.
