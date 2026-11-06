---
id: task-465-01-laser-impact-slice
description: "Bp: Burst rifle impacts become the green flat cartoon burst of the owner reference (representative slice)"
parent: task-465-shot-impact-burst
kind: task
status: done
---
# Player shot impact burst: bp

- goal: Burst rifle impacts become the green flat cartoon burst of the owner reference (representative slice)
- agent: bp
- depends_on: task-464-player-shot-colour
- inputs: color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), task-432-player-shot-impact-sparks (vault/reference/tasks/task-432-player-shot-impact-sparks.md), task-464-player-shot-colour (vault/reference/tasks/task-464-player-shot-colour.md), Art/Source/impact-burst/owner-ref-impact-burst-2026-11-05.png, Art/Source/muzzle-flash/owner-ref-green-laser-muzzle-2026-11-05.webp
- expected_output: Expected result: at each burst rifle hit, a bright pale core, a radial star of sharp spikes in several greens, a soft dark green glow and a few small solid and outlined triangles flying out, gone in a fraction of a second, about the size of today's sparks (at most twice). Rules to preserve: flat graphic angular shapes as in the reference, never the Shooter acid green, no gameplay change. Starts after task-464 is done.
- done_when: In a -game run each burst rifle hit on a robot, rock or barrel shows the green cartoon burst described in expected_output instead of the old sparks; robots and damage numbers stay readable during a full volley; shotgun, turret and enemy impacts not yet changed; no gameplay value changed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: input 6, cache write 24093, cache read 28662, output 3645
- usage: turns 3, context start 9k peak 24k, resumes 0, 1 min, units 0.05 M
- runs: 1
- milestone: M2
- started: 2026-11-06T09:25:46Z
- waiting_for: none
- result: Done (ece043ab, ffcc353a). BP_Projectile (burst rifle shot) ImpactSparkSystem = new NS_ShotImpactBurstGreen (copy of NS_MuzzleFlash, CPU, local space): emitter Sparks = 5 double-spike sprites (velocity aligned, horizontal cone 85-90 deg from +Z, speed 35, drag 4, 9.8-15.4 x 50-90 uu, 0.12-0.18 s); emitter Core = core (42-50 uu, 0.12 s) + 1 solid + 1 outlined triangle (16-22 uu, flying out 154-266 uu/s, 0.2-0.24 s); 8 sprites, at most 0.24 s (art rule A8). New M_ImpactBurstGreen (copy of M_MuzzleFlashGreen, Custom code: ParticleColor B = kind, R tone, G seed, A progress; task-464 greens Main (0.45,3.2,0.12), Pale (2.8,4.4,1.9), Dark (0.02,0.35,0.01), Accent (0.2,1.6,0.1), black triangles kept; depth test off so the burst is not hidden inside the robot). Seen in slow-motion -game frames on a Heavy (Saved/Captures/task465_01_check6/crop.png): green radial star with pale centre, flying triangles, robot and damage numbers readable. Two corrections during build: spikes fanned upward (cone semantics, fixed to horizontal), 16 sprites over the A8 bound (style-guide fail, cut to 8). Rock, barrel and full-volley readability checked in task-465-02. Turret and shotgun unchanged (NS_ShotImpactSparkGreen, NS_ShotImpactSpark). No gameplay value changed.

## Relations
- [[task-465-shot-impact-burst]] — parent global task.
