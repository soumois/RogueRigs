---
id: task-396-duskite-leftover-labels
description: "Global task: The last player-visible "ore" texts left by task-392 become "Duskite": shop card "Short on ore" and the end screen designer default."
parent: tasks-to-do-index
kind: task
status: todo
---
# Duskite: leftover on-screen "ore" labels

- request: Owner decision 2026-10-30, relayed by the main session (translated): task-392 is done (commits 97e53909, 874624a0, 464b5620, ec1d48af, 0d68fdd8) but left two player-visible "Ore" texts: (1) the ShortLabel "Short on ore" on the cards of WBP_ShopScreen (ShopCard0 to ShopCard4) and probably the default in WBP_ShopCard; (2) the designer default "Ore secured: 0" of EndOreText in WBP_AlertObjectives (only shown in the editor, overwritten at runtime). Standing rule: change every on-screen occurrence of ore to Duskite (Duskite is the provisional name; identifiers, logs and editor tooltips keep "ore"). Also check the texts built at runtime in the shop cards and any other player-facing string in Source/ or Content/RogueRigs/Data/Text containing "ore". Scope approved by the owner, no question; editor open for the bp parts; no capture, the owner tests himself; runs after task-393; number sources must not change. Planner inventory 2026-10-30 (strings in the UI assets and files): WBP_ShopScreen holds "Short on ore" 10 times (ShortLabel overrides of ShopCard0..4); WBP_ShopCard has no "Short on ore" string (its ShortLabel default and the runtime texts come from screen_texts labels such as "NEED {n} MORE", no ore word; tooltip "Ore price of the card" is editor-only and stays); WBP_AlertObjectives EndOreText default "Ore secured: 0"; WBP_Intermission only holds comments or tooltips ("ore secured this level", "Run Secured Ore" names), editor-only. Content/RogueRigs/Data/Text/*.json: no "ore" left. Source/: "ore" only in identifiers and layout keys (RRLayoutData "ore", RRRunLogSubsystem log type), not shown to the player.
- goal: No player-visible text, at design time or at runtime, says "ore": the shop card short label reads "Short on Duskite", the end screen designer default reads "Duskite secured: 0"; number sources, identifiers, logs and tooltips unchanged.
- acceptance: bp: WBP_ShopScreen ShopCard0..4 ShortLabel "Short on Duskite", WBP_ShopCard runtime text graph checked (any visible "ore" reported and fixed in the same subtask only if it lives in WBP_ShopCard default text); WBP_AlertObjectives EndOreText default "Duskite secured: 0"; no binding or number source changed; each compiled with warnings as errors and saved; docs updated; lint clean; committed by path; owner checks on screen himself.
- gdd_check: Read 2026-10-30: [[ore-blocks]] and [[gdd-glossary]] (on screen the ore is always "Duskite", provisional name; identifiers and vault ids keep "ore"): the request applies this rule, no conflict. Open values: none (wording follows the task-392 forms "Total Duskite:", "Duskite secured: N"). Tech: [[wbp-shop-screen]], [[wbp-shop-card]], [[wbp-alert-objectives]]; previous task [[task-392-duskite-on-screen]]. Scope M2. No capture (owner rule 2026-10-30). Repo state 2026-10-30: task-393 (bp, editor open) in progress at task-393-01; this task runs after it. Config/DefaultGame.ini and Config/DefaultEditor.ini hold uncommitted owner changes: never commit them (commit by path).
- confirmed_by_owner: yes (2026-10-30, scope approved by the owner, relayed by the main session: "change every occurrence of ore to Duskite" on screen)
- milestone: M2
- next_step: task-396-01-shop-short-label
- started: open
- waiting_for: none
- result: open

## Subtasks
1. task-396-01-shop-short-label (vault/reference/tasks/task-396-duskite-leftover-labels/task-396-01-shop-short-label.md) — bp: WBP_ShopScreen: ShopCard0..4 ShortLabel "Short on ore" becomes "Short on Duskite"; read WBP_ShopCard runtime texts
2. task-396-02-end-ore-text-default (vault/reference/tasks/task-396-duskite-leftover-labels/task-396-02-end-ore-text-default.md) — bp: WBP_AlertObjectives: EndOreText designer default "Ore secured: 0" becomes "Duskite secured: 0"
3. task-396-03-doc-duskite-leftovers (vault/reference/tasks/task-396-duskite-leftover-labels/task-396-03-doc-duskite-leftovers.md) — doc: update the shop screen, shop card and end screen docs with the Duskite labels

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-396-01-shop-short-label]] — step 1, bp.
- [[task-396-02-end-ore-text-default]] — step 2, bp.
- [[task-396-03-doc-duskite-leftovers]] — step 3, doc.
- [[task-392-duskite-on-screen]] — the task that renamed the first labels.
- [[ore-blocks]] — the on-screen Duskite rule.
