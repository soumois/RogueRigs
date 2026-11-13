---
id: tasks-to-do-index
description: "Open global tasks (in progress, wait-answers, todo, review, blocked), one line each with status. Read on every Orchestrator start."
parent: tasks-index
kind: note
---
# Tasks to do

A global task moves to [[tasks-done-index]] when it is done or cancelled (`vault/scripts/task-status.js` does it). A global waiting for the owner stays here as `Status wait-answers.`; its node's `waiting_for` says what it waits for.

## Relations
- [[tasks-index]] — parent index.




- [[task-540-main-menu-no-focus-outline]] — Remove the orange keyboard-focus outline drawn around the focused main menu button; focus and Enter unchanged. Status in-progress.
- [[task-541-hud-turret-count-removed]] — The turret count text above the health bar goes; the launcher charges stay shown in its weapon slot. Status todo.
- [[task-542-ground-particle-edges]] — The white ground particle sprites of image 2 (boss scene) show their quad edges; the edges must not show, look otherwise kept. Status todo.
- [[task-543-wall-debris-less-white]] — Rock destruction debris take the rock colours (lilac and violets) instead of near white, no glow. Status todo.
- [[task-544-horde-kill-gauge]] — The horde "!" moves to half the C position and no longer triggers a horde; it starts a kill gauge under the alert bar; full gauge = horde, full until the horde is over, then empties and refills. Status todo.
- [[task-545-pause-screen-bolts]] — Pause screen rebuilt to owner image 10: dividers from bar to bar, horizontal divider, bolts on every end and junction as a separate unstretched image. Status todo.
- [[task-546-level-cleared-look]] — Stats screen adjusted to owner image 6: vertical divider from a bolt junction on the horizontal divider to the bottom bar, shared unstretched bolts. Status todo.
- [[task-547-level-cleared-entrance]] — The death screen entrance becomes one shared effect; the level cleared screen (and its level 4 variant) uses it; the death screen looks the same. Status todo.
- [[task-548-shop-frame-layout]] — The shop card panel gets a frame image made for its ratio (no stretched button frame) and the layout of owner image 8. Status todo.
- [[task-549-ui-shared-patterns]] — Audit of the UI widgets and UI build scripts after this batch; repeated patterns factored into shared pieces with no visible change, documented. Status todo.
