---
id: task-318-preload-next-level
description: "Global task: the next level loads during the cards screen; Continue fades into it, or shows a loading screen with progress bar."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Preload the next level during the cards screen

- request: "à l'écran de sélection des cartes, on doit déjà être en train de charger l'autre niveau [...] je m'attendais à un écran noir pour l'instant en fond [...] Si lorsque je clique sur le niveau suivant, le niveau est déjà chargé, on fait un fade sur le niveau suivant. Si le niveau n'est pas chargé, on fait une transition sur l'écran de chargement avec une barre de chargement et plus tard une image." Answers: Q4 "Le but c'est que la prochaine map commence déjà à se charger pendant qu'on est sur l'écran des cartes"; Q5 fade 0.3 s out, 0.5 s in, ok; Q6 loading screen kept, progress bar added, empty hidden image slot, ok (owner, 2026-10-26)
- goal: The next level loads in the background while the cards screen is shown; Continue fades into it when ready, else shows the loading screen with a progress bar.
- acceptance: opening the cards screen starts loading the next level in the background; Continue with the level ready fades into it (0.3 s out, 0.5 s in, provisional) with no loading screen; Continue before it is ready shows the loading screen with a progress bar and a hidden image slot, then fades in; carried state identical to before; -game captures of both paths sent to the owner; docs updated; lint clean; committed.
- gdd_check: intermission read (2026-10-26). Today the level change is a blocking OpenLevel of the same map (2.3-3.3 s) plus prewarm hold, only the layout is async: real preloading confirmed by the owner (Q4). Fade durations Q5 and loading screen with progress bar and empty image slot Q6 accepted; loading image open. Depends on task-317 (cards screen).
- confirmed_by_owner: yes (2026-10-26)
- milestone: M2
- next_step: task-318-02-fade-or-loading
- result: open

## Subtasks
1. task-318-01-preload-next-level — cpp
2. task-318-02-fade-or-loading — cpp
3. task-318-03-bp-loading-progress — bp
4. task-318-04-capture-preload — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-318-01-preload-next-level]] — step 1, cpp.
- [[task-318-02-fade-or-loading]] — step 2, cpp.
- [[task-318-03-bp-loading-progress]] — step 3, bp.
- [[task-318-04-capture-preload]] — step 4, cpp.
