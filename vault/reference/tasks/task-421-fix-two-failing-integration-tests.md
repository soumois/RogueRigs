---
id: task-421-fix-two-failing-integration-tests
description: "Global task: Fix Integration.JuiceKillDropsStreamingCubes and Integration.LevelUpUpgradeLoop: both tests predate owner decisions; the asset and widget are right."
parent: tasks-to-do-index
kind: task
status: todo
---
# Fix the two pre-existing failing integration tests

- request: Owner 2026-11-02 (relayed by the main session): "Oui, planifier" a task fixing the 2 failing integration tests, failing before task-420 and independent of any CSV value: (1) `RogueRigs.Integration.JuiceKillDropsStreamingCubes`: "BP_PickupXP PickupMesh has no collision" expected 0, got 2 (`RRJuiceIntegrationTests.cpp:408`), the Blueprint mesh has a collision; (2) `RogueRigs.Integration.LevelUpUpgradeLoop`: "first offer: RerollButton enabled = 1" false (`RRIntegrationTests.cpp:1458`), reroll button state in WBP_HUD. Last full run (274 pass, 2 fail) in [[sys-automation-tests]] and task-420-02 result. Small subtasks (task-420-01 and 02 overran their cost ceiling). Decide first whether the test or the asset/widget is wrong per the GDD. Owner answer 2026-11-02 (relayed by the main session, verbatim): "Tout par défaut" = the plan and the defaults of Q1, Q2, Q3 accepted.
- goal: Both tests pass by matching the decided behaviour: the XP cube body is a physics-only body that hits only floor and walls (task-229), and the reroll button is disabled during the 0.8 s card deal then enabled when the reroll is affordable (task-243-05, task-315). No asset, Blueprint or gameplay code changes.
- acceptance: A1: `RogueRigs.Integration.JuiceKillDropsStreamingCubes` passes; for each live BP_PickupXP cube, PickupMesh has collision profile `Pickup` (PhysicsOnly, Pawn ignored) and every other primitive NoCollision; every primitive keeps CanCharacterStepUpOn = ECB_No. A2: `RogueRigs.Integration.LevelUpUpgradeLoop` passes; RerollButton enabled state is checked after the deal end (0.8 s real time) and equals bCanReroll for the first offer (true) and the rerolled offer (false). A3: only `Source/RogueRigs/Tests/` files change; BP_PickupXP, WBP_UpgradeChoice, WBP_HUD and gameplay C++ untouched. A4: build ok; each fixed test and the other tests of its file pass. A5: sys-automation-tests known-failures line removed, test doc nodes updated; lint clean; committed.
- gdd_check: Checked 2026-11-02. Test 1: the test check dates from task-028-03 (no pickup component may collide, the trail blocked the miner). task-229 (owner 2026-10-17, "Collision uniquement sur mur et sol", done) made PickupMesh the only body: profile `Pickup` in Config/DefaultEngine.ini = PhysicsOnly (value 2, the observed one), blocks WorldStatic, ignores Pawn and every other channel; `ARRPickup::BeginPlay` sets every other primitive NoCollision ([[sys-pickups-lifecycle]]). The miner still cannot be blocked or carried. Verdict: the test is wrong, the asset is right. Test 2: [[xp-level-up]] card animation (owner 2026-10-19, provisional): cards dealt face down, about 0.8 s; reroll (owner 2026-10-26): button greyed when unaffordable. [[wbp-upgrade-choice]] (task-243-05): StartDeal disables RerollButton, AnimStep (widget tick, real time) enables it to RerollAllowed at 0.8 s. The test reads the button right after the offer, during the deal; headless, the widget tick does not run. Verdict: the test is wrong, the widget is right. No conflict, no open value, no CSV value involved. Owner said "une tâche": one global with one cpp subtask per test (different files, disjoint) and a doc subtask.
- confirmed_by_owner: yes (2026-11-02, "Tout par défaut")
- milestone: M3
- next_step: task-421-01-juice-cube-collision-test
- started: open
- waiting_for: none
- result: open

## Questions
Decided 2026-11-02 (owner "Tout par défaut"): every default below is accepted; values provisional, owner 2026-11-02.
- Q1 Test 1 (cubes XP) : c'est le test qui est faux. Depuis task-229 (« Collision uniquement sur mur et sol »), le mesh du cube est un corps physique qui touche seulement sol et murs et ignore le mineur ; le test (task-028-03) exige encore « aucune collision ». Correction : le test accepte pour PickupMesh le profil `Pickup` (physique seule, ignore le mineur), garde « aucune collision » pour les autres composants et « on ne peut pas monter dessus » pour tous. BP_PickupXP n'est pas touché. Default: oui.
- Q2 Test 2 (bouton Relancer) : c'est le test qui est faux. Le bouton est grisé pendant la distribution des cartes (0,8 s, task-243-05) puis activé s'il est payable ; le test le lit tout de suite, et sans écran le tick du widget ne tourne pas. Correction : le test fait avancer la distribution (tick du widget WBP_UpgradeChoice sur au moins 0,8 s de temps réel) puis vérifie activé = payable (1re offre activé, offre relancée grisé). Si ce tick ne peut pas tourner sans écran, repli : vérifier grisé pendant la distribution et la variable RerollAllowed du widget. Les widgets ne sont pas touchés. Default: oui.
- Q3 Découpage : une tâche task-421, 3 sous-tâches : 01 cpp test cubes XP, 02 cpp test Relancer, 03 doc (retire les 2 échecs connus de sys-automation-tests). Chaque cpp compile et lance seulement les tests de son fichier, pas la suite complète ; plafond 0,6 M unités par sous-tâche cpp. Default: oui.

## Subtasks
1. task-421-01-juice-cube-collision-test (vault/reference/tasks/task-421-fix-two-failing-integration-tests/task-421-01-juice-cube-collision-test.md) — cpp: JuiceKillDropsStreamingCubes accepts the physics-only PickupMesh (profile Pickup), other components NoCollision
2. task-421-02-levelup-reroll-button-test (vault/reference/tasks/task-421-fix-two-failing-integration-tests/task-421-02-levelup-reroll-button-test.md) — cpp: LevelUpUpgradeLoop checks RerollButton after the 0.8 s deal
3. task-421-03-doc (vault/reference/tasks/task-421-fix-two-failing-integration-tests/task-421-03-doc.md) — doc: test doc nodes: the two fixes, known failures removed

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-421-01-juice-cube-collision-test]] — step 1, cpp.
- [[task-421-02-levelup-reroll-button-test]] — step 2, cpp.
- [[task-421-03-doc]] — step 3, doc.
- [[task-229-pickup-physics]] — the owner decision that made PickupMesh a physics body.
- [[task-315-paid-reroll]] — the reroll button cost and greyed state.
