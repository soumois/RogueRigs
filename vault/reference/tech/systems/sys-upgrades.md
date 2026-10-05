---
id: sys-upgrades
description: "Upgrades as built: URRUpgradeComponent on the miner, value and rarity CSVs, luck-weighted draw, owned-weapon targets, new-weapon cards, runtime GEs, reroll, replay."
parent: systems-index
kind: doc
---
# Upgrades

- responsibility: build the card offer and apply the picked upgrade. The level-up flow (pause, heal, chained screens) drives it: [[sys-level-up]].
- main_classes (`Source/RogueRigs/Upgrades/`): `URRUpgradeComponent` (default subobject `Upgrades` of `ARRMiner`, `GetUpgrades()`); `FRRUpgradeCard` (`CardType`, `WeaponClass`, `Stat`, `Tier`, `Target`, `TargetWeapon`, `Value`, `Operation`, UI texts `DisplayName`, `TargetText`, `ValueText`, `DescriptionText`) and enums `ERRUpgradeCardType` (`StatUpgrade`, `NewWeapon`), `ERRUpgradeStat`, `ERRUpgradeRarity` (rank = value), `ERRUpgradeTarget` (`Player`, `AllWeapons`, `Weapon`), `ERRUpgradeOperation` in `RRUpgradeCard.h`; rows `FRRUpgradeValueRow`, `FRRUpgradeRarityRow`.

## Rules
- Offer: each card picks a stat uniformly among offerable stats (at least one row; weapon stats only while the miner owns a weapon), then a target uniformly among that stat's targets (all weapons or one weapon of `ARRMiner::GetWeapons()`: a new weapon is a target automatically), then a tier. A stat without a row at the drawn tier (piercing below blue) is dropped from this offer and the card is drawn again, so piercing cards are rarer and only blue, purple or yellow (implementation choice). Two cards never share stat and target. Stat and target odds: provisional implementation choice.
- New-weapon cards (task-024-04, [[upgrades]]): before the stat cards, while the miner can acquire a weapon of its catalogue (`ARRMiner::GetAcquirableWeapons`: not owned, fewer than `MaxWeapons` 4 owned, [[sys-player]]), each slot in order rolls `NewWeaponCardChance` (0.3); a success makes it a "Nouvelle arme" card for an acquirable weapon picked uniformly, at most `MaxNewWeaponCardsPerOffer` (1) per offer; other slots get stat cards. No roll when the chance is 0 or nothing is acquirable (seeded stat draws unchanged). Card: `CardType` `NewWeapon`, `DisplayName` "Nouvelle arme", `TargetText` = CSV weapon name (`ARRWeapon::GetClassDisplayName`), `DescriptionText` = catalogue description, empty `ValueText`, `Tier` unused: the UI styles it by `CardType`, not by rarity. Picking calls `ARRMiner::AddWeapon` (acquisition replays the global cards); the picked card records the weapon in `TargetWeapon`; the new weapon becomes a target of later stat cards. Refused (false) when owned, at the limit or without a miner. A reroll never offers a weapon of the replaced offer. Measured: 66.4 % of offers hold one (expected 65.7 %).
- Tier draw: [[rarities]] formula, k per row (all 0.1); luck <= −10 floors growth at 0.
- Application: one infinite runtime GE per (attribute, operation), SetByCaller `Data.UpgradeValue`: `AddBase` = value, `MultiplyAdditive` = 1 + value, so percentages add ([[upgrades]] stacking). Player stats on the miner ASC; `MaxHealth` also adds the value to `Health` (instant GE). Weapon cards on that weapon ASC; all-weapons cards on every owned weapon ASC, replayed by `ARRWeapon::Acquire` after the base values. Never removed.
- Reroll: once per offer; the new cards share no (stat, target) with the replaced ones (only exact duplicates excluded when the pool is too small).
- Display: French stat names, "Toutes les armes" or the weapon `DisplayName`; values "+12 %", "+7,5 %", "+20" (crit stats in %); piercing "Perforation", "+1".

- entry_points: `BuildOffer(CardCount, Luck)`, `BuildOfferWithOwnerLuck(CardCount = 3, BonusLuck)` (chest bonus), `Reroll(OutCards)`, `CanReroll`, `HasOffer`, `GetCurrentOffer`, `ApplyUpgrade(Card)` (refuses unowned weapons; new-weapon cards equip; ends the offer; `OnUpgradeApplied`), `GetPickedUpgrades`, `ReplayGlobalUpgrades(Weapon)`, `GetUpgradeValue`, `GetTierWeights`, `DrawTier`, `MakeCard`, `MakeWeaponCard(Entry)` (static), `GetNewWeaponCardChance`, `GetMaxNewWeaponCardsPerOffer`, `SetRandomSeed`.
- data: `Content/RogueRigs/Data/Tables/UpgradeValues.csv` (row `<Stat>_<Tier>`: `Stat`, `Tier`, `GlobalValue` in attribute units, `TargetedMultiplier` 1.5, `Operation`; values from [[upgrades]]; no `MagazineSize` rows in M1; `Pierce_Blue/Purple/Yellow` +1 / +2 / +3 `AddBase` with `TargetedMultiplier` 1 (whole robots; interpretation, the 1.5 rule would give fractions)); `UpgradeRarities.csv` (row = tier: `BaseWeight`, `LuckFactor`); `WeaponStats.csv` column `DisplayName`. Tunables on the component (EditDefaultsOnly, provisional): `NewWeaponCardChance` 0.3, `MaxNewWeaponCardsPerOffer` 1.
- depends_on: [[sys-gas-core]], [[sys-weapons]], [[sys-player]].
- implements: [[upgrades]], [[rarities]], [[player-stats]].
- gotchas: runtime GEs are never replicated (single-player). A missing SetByCaller returns 0 and logs an error ([[gas-api-notes]]). Effect cards are not built (M2); piercing is a stat (`ERRUpgradeStat::Pierce`, weapon attribute `Pierce`), not an effect.

## Relations
- [[systems-index]] — parent index.
- [[sys-status-effects]] — stacking arithmetic.
- [[sys-automation-tests]] — RogueRigs.Upgrades.* prove these rules.
