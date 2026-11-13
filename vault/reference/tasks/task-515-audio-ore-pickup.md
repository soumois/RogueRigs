---
id: task-515-audio-ore-pickup
description: "Global task: Ore pickup plays the XP pickup sound for now, through its own event line so it can be swapped later."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Son du minerai ramassé (son d'XP pour l'instant)

- request: « ok pour les questions.

il faut aussi ce même systéme pour la mort des ennemis.

Applique le son d'xp au minerai ramassé pour l'instant.

Il faudrait aussi le prompt pour les explosions et qui changerait aussi selon la distance, sachant que la distance se calculerait par rapport au bord de l'explosion.

Il faudrait aussi le son quand on creuse. » (owner 2026-11-13, answer to task-512/513 questions). Just before: « Je ne publierais pas le jeu avec ces sons, c'est juste pour avoir des sons placeholder qui seront remplacé plus tard. Ou est-ce que je peux avoir le son coupé, car je n'ai pas entendu le level up. » (level up: open diagnosis by the owner, not planned). Point: "Applique le son d'xp au minerai ramassé pour l'instant."
- goal: Picking up ore plays the XP pickup sound, through its own event so a later sound is one table line.
- acceptance: A1 an ore pickup event has its own table line pointing to the XP pickup sound with the XP pickup settings (cap 3, ±5 % pitch, centred); A2 in a -game run with Verbose LogRRAudio picking up ore logs one play of that event, XP pickup unchanged; A3 build green, audio tests pass; A4 sys-audio lists the event and its call site.
- gdd_check: Read audio-direction, sys-audio. Event list of task-511 Q1 had no ore pickup: added by the owner. No open value (default settings copied from XP pickup, provisional). No code read.
- verification_level: light
- family: comportement
- confirmed_by_owner: yes (owner 2026-11-13: direct instruction "Applique le son d'xp au minerai ramassé pour l'instant", no choice left; the relay may stop it before launch)
- milestone: M2
- next_step: task-515-01-ore-pickup-sound
- started: 2026-11-13T08:54:25Z
- waiting_for: none
- result: open

## Subtasks
1. task-515-01-ore-pickup-sound (vault/reference/tasks/task-515-audio-ore-pickup/task-515-01-ore-pickup-sound.md) — cpp: ore pickup event, table line on the XP sound, call; sys-audio line

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-515-01-ore-pickup-sound]] — step 1, cpp.
