---
id: task-030-01-director-speeds
description: "Wave director retimed (first robot at 1 s, first pack at 15 s, slower ramp), shooters blended into chaser packs, robot speeds −20 %, tests."
parent: task-030-pacing-feedback
kind: task
status: done
---
# Director timings, shooter mix and speeds

- goal: the director and robot speeds match the owner's play test feedback.
- agent: cpp
- depends_on: none
- inputs: [[enemies-index]], [[enemy-chaser]], [[enemy-kamikaze]], [[enemy-shooter]], [[enemy-heavy]], [[sys-spawner-director]], [[sys-spawner]], [[sys-enemies]], [[sys-automation-tests]], [[cpp-standard]]
- expected_output: FRRWaveDirectorConfig::MakeDefault: Calm phase removed or 0 s, first robot at about 1 s, lone chaser every 2 s, first pack 6–8 chasers at 15 s, waves of 1–2 packs until 90 s then 2–3 (shield chasers from 60 s kept), interval 6 → 3.5 s over 210 s, horde fronts unchanged (95 s, 175 s); shooter-only pack type removed; 1–2 shooters added as non-front members of some chaser packs so shooters are about 10 % of spawned robots (measure it in a test over a simulated run); EnemyStats.csv MoveSpeed chaser 288, shield chaser 288, kamikaze 240, shooter 240, heavy 160; tests updated (Spawner.Director*, ShieldChaser.DirectorMix, Integration.PressureDirectorPhases and any test timing robot travel); docs sys-spawner-director, sys-enemies.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 110000
- model: opus
- tokens: input 90, cache write 170774, cache read 4580024, output 52843
- runs: 1
- milestone: M2
- result: No calm; lone chaser at 1 s then every 2 s; first pack 6–8 chasers at 15 s; flow from 21 s with 1–2 packs, FlowShields at 60 s, FlowWide at 90 s with 2–3 packs; interval 6 → 3.5 s over 210 s; shooter-only pack removed, 1–2 shooters (bBehind, back half) in every flow pack: 9.2–9.8 % of robots outside hordes; weights 85 / 15; hordes unchanged. Speeds chaser and shield chaser 288, kamikaze and shooter 240, heavy 160. New test Spawner.DirectorShooterShare; build 0, 185/185. Commits 8af107d, 58a4d14, acfa4bd.

## Relations
- [[task-030-pacing-feedback]] — parent global task.
