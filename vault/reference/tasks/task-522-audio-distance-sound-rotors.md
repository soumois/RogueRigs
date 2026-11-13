---
id: task-522-audio-distance-sound-rotors
description: "Global task: One looped rotor sound for the extractor fading with distance, built on a generic distance sound class with the explosion as a child."
parent: tasks-done-index
kind: task
status: done
---
# Son des hélices de l'extracteur et code générique de son à distance

- request: Owner 2026-11-13, play-test batch in PIE (12 points, family gros morceau split one point per global), with two screenshots (image 1 gameplay: HUD, alert bar C / B / ! / S, a chest light glow with no chest; image 2 Options, Sound tab, the "Sound" tab button drawn smaller than the other tabs). This point, verbatim: « il faudrait un son pour le bruit des hélices ( mais un seul pour les 4 et que ça soit par rapport à la distance aussi)- Géneralise bien cet idée de code, ça va être assez courant. ( et pour l'explosion qui est un peu différente, tu fais un enfant qui hérite ) ». Owner answer 2026-11-13 (relayed by the main session, verbatim): "tout par défaut" = the plan and the defaults of Q4, Q5, Q6, Q7 accepted.
- goal: One generic distance sound in C++ (Q7) carries every distance-faded sound, the explosion as an inheriting child; the extractor plays one rotor loop with distance per Q4 to Q6. task-523 and task-527 build on its first subtask.
- acceptance: A1 one base class for distance sounds (point, full level then linear fade, one-shot or loop) and one explosion child class measuring from the radius edge; Impact.Wall, EnemyDeath and Explosion go through them with the same level at the same distances as before (rr.Audio.Play with Verbose LogRRAudio at 0, 200, 600, 1075 uu, explosion at its edge); A2 rotor sound file chosen by the owner (Q6), kept and imported; A3 one rotor loop per extractor, never four, per Q4 and Q5, logged with its distance factor on arrival and extraction; A4 build green, existing audio tests pass; sys-audio documents the classes and how to add a distance sound. The agent hears nothing: the owner judges by ear.
- gdd_check: Read audio-direction, sys-audio, extraction, barrel. No conflict. Code read: RRAudioSubsystem GetFadeDistance and the Explosion call in RRExplosionSubsystem, to answer the owner's barrel question (distance = 2D player to centre minus Radius, floor 0: already from the edge). Open, asked: rotor play window and level (Q4), distances (Q5), sound source (Q6), shape of the generalization (Q7). Edit: audio-direction.
- verification_level: standard
- family: comportement
- confirmed_by_owner: yes (2026-11-13, "tout par défaut")
- milestone: M2
- next_step: none
- started: 2026-11-13T14:27:39Z
- waiting_for: none
- executor_tokens: input 184, cache write 300094, cache read 11169064, output 68273
- executor_usage: runs 1, turns 90, context start 16k peak 207k, resumes 1, 63 min, units 1.83 M, USD 5.10
- planning_tokens: input 6, cache write 18666, cache read 174110, output 3853
- planning_usage: a163c449 "Plan owner feedback batch" 1/9 (tasks 520, 521, 522, 523, 524, 525, 526, 527, 528), units 0.06 M, USD 0.21
- result: Done. A1: FRRDistanceSound (point, one-shot or loop) and child FRRExplosionSound (from the radius edge) in Audio/RRDistanceSound.h; Impact.Wall, EnemyDeath, Explosion go through them, -game levels unchanged at 0/200/600/1075 uu (7ee67e99). A2: owner ElevenLabs file (prompt P2) kept and prepared as a 4.38 s loop, manifest entry, imported looping as SW_ExtractorRotors (e35d9c9d). A3: ARRShuttle plays one ExtractorRotors distance loop (800 to 2150 uu, constant level) while its rotors spin, arrival and extraction logged with their factor, pause-proof during the takeoff (6408a143). A4: build green, no audio test family; LogRRAudio moved to Audio/RRAudioLog.h (unity collision of task-524 fixed, checked in one unity file); docs sys-audio-distance-sounds, sys-audio, sys-objectives-shuttle, bp-extraction-shuttle (02021de6). Owner retouch: ExtractorRotors line of Config/DefaultGame.ini (fades 0.15/0.3 s are Executor values); judged by ear. ALERT: executor run over the standard budget (90 turns, 1.83 M units): owner wait and resume for the sound, the coordinator's build fix, a pre-existing C4996 Cast warning (tests unity file) traced through a stash build, two -game checks.

## Questions
Decided 2026-11-13 (owner "tout par défaut"): every default below is accepted; values provisional, owner 2026-11-13.
- Q4 Un seul son en boucle pour les 4 hélices, au centre de l'extracteur, joué tant que ses rotors tournent dans le niveau (à l'arrivée jusqu'à sa sortie de vue ; à l'extraction de l'atterrissage à la fin du décollage derrière l'écran de succès), volume constant (pas de variation avec la vitesse des rotors). Default: oui.
- Q5 Distance : plein volume jusqu'à 800 uu du centre de l'extracteur (environ le bord de sa zone d'embarquement), puis baisse linéaire jusqu'au silence à 2150 uu (deux fois la distance bord d'écran) ; niveau réglé à l'oreille. Default: oui.
- Q6 Le son : tu le génères avec ElevenLabs à partir de 3 prompts anglais écrits par l'exécuteur, tu choisis à l'oreille ; placeholder. Default: oui.
- Q7 Généralisation : une classe C++ de base « son à distance » (un point, plein volume puis fondu linéaire, en un coup ou en boucle) utilisée par tous les sons à distance (impact mur, mort robot, hélices, et ceux des tâches suivantes), et l'explosion en classe enfant qui mesure depuis le bord de son rayon ; les volumes actuels aux mêmes distances ne changent pas. Default: oui.

## Subtasks
1. task-522-01-distance-sound-classes (vault/reference/tasks/task-522-audio-distance-sound-rotors/task-522-01-distance-sound-classes.md) — cpp: generic distance sound base class and explosion child; existing faded rows moved onto them
2. task-522-02-sound-prompts (vault/reference/tasks/task-522-audio-distance-sound-rotors/task-522-02-sound-prompts.md) — text: 3 English ElevenLabs prompts for the extractor rotor loop (four rotors, one sound, loopable), owner generates and picks by ear
3. task-522-03-sound-import (vault/reference/tasks/task-522-audio-distance-sound-rotors/task-522-03-sound-import.md) — bp: rotor loop sound (imported looping) kept in Art/Source/audio with a manifest entry (placeholder), imported
4. task-522-04-rotor-loop (vault/reference/tasks/task-522-audio-distance-sound-rotors/task-522-04-rotor-loop.md) — cpp: extractor plays one rotor loop with distance per Q4, Q5
5. task-522-05-doc (vault/reference/tasks/task-522-audio-distance-sound-rotors/task-522-05-doc.md) — doc: sys-audio: distance sound classes, rotor loop, how to add a distance sound

## Relations
- [[tasks-done-index]] — parent index.
- [[task-522-01-distance-sound-classes]] — step 1, cpp.
- [[task-522-02-sound-prompts]] — step 2, text.
- [[task-522-03-sound-import]] — step 3, bp.
- [[task-522-04-rotor-loop]] — step 4, cpp.
- [[task-522-05-doc]] — step 5, doc.
