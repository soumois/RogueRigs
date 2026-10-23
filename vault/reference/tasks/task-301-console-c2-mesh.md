---
id: task-301-console-c2-mesh
description: "Global task: Replace the console cylinder with the owner C2 uplink mast mesh: mobile dish head, three material slots, retouched dish back and foot lamps."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Console C2 uplink mast mesh

- request: "je vais garder l'image de base (pas d'antenne dépliable)." "Je t'ai mis en image, la partie mobile du mesh (image 1)." "L'arrière de l'antenne n'est pas tout à fait clean, retouche les avec l'image." "Au niveau des lumières sur les pieds (image 2) j'aimerais plutôt un résultat comme l'image 3." "isole avec un material pour l'orange de l'antenne (pour le faire clignoter) et un material pour les autres parties orange" (owner, 2026-10-23)
- goal: Replace the console cylinder with the owner C2 uplink mast mesh: mobile dish head, three material slots, retouched dish back and foot lamps.
- acceptance: BP_ObjectiveConsole shows the C2 mesh (base plus a separate head turning about the mast axis), three material slots (antenna orange, other orange, screen), clean dish back and round amber foot caps; -game frames before and after sent to the owner; committed.
- gdd_check: objective-console, art-style-guide, color-code read (2026-10-23). Conflict: red and green state lamps break A7 (enemy colours); owner default accepted: exception "console state lamps" (written by task-302-01). Drone dock dropped (no dock, no folding antenna), drone hack unchanged. Mobile part = image 1 (dish + head above the mast elbow), turns about the vertical mast axis. Foot lamps go to the "other orange" material, flat emissive, round caps like image 3. Dish back repainted in Blender after the sheet back view, render shown to the owner before going further.
- confirmed_by_owner: yes (2026-10-23, "ok" to the plan and every default; planned in one planning pass, each task run later by its own orchestrator)
- milestone: M2
- next_step: task-301-02-console-c2-cut
- result: open

## Subtasks
1. task-301-01-console-c2-files — art: Save the console C2 sheet, Meshy files and prompt
2. task-301-02-console-c2-cut — 3d: Cut the console C2 mobile head and set its material slots
3. task-301-03-console-c2-retouch — 3d: Repaint the console C2 dish back and foot lamps
4. task-301-04-bp-console-c2-import — bp: Import the console C2 into BP_ObjectiveConsole
5. task-301-05-capture-console-c2 — cpp: Capture the console C2 in game

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-301-01-console-c2-files]] — step 01, art.
- [[task-301-02-console-c2-cut]] — step 02, 3d.
- [[task-301-03-console-c2-retouch]] — step 03, 3d.
- [[task-301-04-bp-console-c2-import]] — step 04, bp.
- [[task-301-05-capture-console-c2]] — step 05, cpp.
