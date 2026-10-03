---
id: sys-upgrades
description: "Upgrades as built: URRUpgradeComponent on the miner, value and rarity CSVs, luck-weighted draw, owned-weapon targets, runtime SetByCaller GEs, reroll, replay."
parent: systems-index
kind: doc
---
# Upgrades

- responsibility: build the card offer and apply the picked upgrade. The level-up flow (pause, heal, chained screens) is [[task-014-03-levelup-flow]].
- main_classes (`Source/RogueRigs/Upgrades/`): `URRUpgradeComponent` (default subobject `Upgrades` of `ARRMiner`, `GetUpgrades()`); `FRRUpgradeCard` (`Stat`, `Tier`, `Target`, `TargetWeapon`, `Value`, `Operation`, UI texts `DisplayName`, `TargetText`, `ValueText`) and enums `ERRUpgradeStat`, `ERRUpgradeRarity` (rank = value), `ERRUpgradeTarget` (`Player`, `AllWeapons`, `Weapon`), `ERRUpgradeOperation` in `RRUpgradeCard.h`; rows `FRRUpgradeValueRow`, `FRRUpgradeRarityRow`.

## Rules
- Offer: each card picks a stat uniformly among offerable stats (a row for every tier; weapon stats only while the miner owns a weapon), then a target uniformly among that stat's targets (all weapons or one weapon of `ARRMiner::GetWeapons()`: a new weapon is a target automatically), then a tier. Two cards never share stat and target. Stat and target odds: provisional implementation choice.
- Tier draw: [[rarities]] formula, k per row (all 0.1); luck <= −10 floors growth at 0.
- Application: one infinite runtime GE per (attribute, operation), SetByCaller `Data.UpgradeValue`: `AddBase` = value, `MultiplyAdditive` = 1 + value, so percentages add ([[upgrades]] stacking). Player stats on the miner ASC; `MaxHealth` also adds the value to `Health` (instant GE). Weapon cards on that weapon ASC; all-weapons cards on every owned weapon ASC, replayed by `ARRWeapon::Acquire` after the base values. Never removed.
- Reroll: once per offer; the new cards share no (stat, target) with the replaced ones (only exact duplicates excluded when the pool is too small).
- Display: French stat names, "Toutes les armes" or the weapon `DisplayName`; values "+12 %", "+7,5 %", "+20" (crit stats in %).

- entry_points: `BuildOffer(CardCount, Luck)`, `BuildOfferWithOwnerLuck(CardCount = 3, BonusLuck)` (chest bonus), `Reroll(OutCards)`, `CanReroll`, `HasOffer`, `GetCurrentOffer`, `ApplyUpgrade(Card)` (refuses unowned weapons; ends the offer; `OnUpgradeApplied`), `GetPickedUpgrades`, `ReplayGlobalUpgrades(Weapon)`, `GetUpgradeValue`, `GetTierWeights`, `DrawTier`, `MakeCard`, `SetRandomSeed`.
- data: `Content/RogueRigs/Data/Tables/UpgradeValues.csv` (row `<Stat>_<Tier>`: `Stat`, `Tier`, `GlobalValue` in attribute units, `TargetedMultiplier` 1.5, `Operation`; values from [[upgrades]]; no `MagazineSize` rows in M1); `UpgradeRarities.csv` (row = tier: `BaseWeight`, `LuckFactor`); `WeaponStats.csv` column `DisplayName`.
- depends_on: [[sys-gas-core]], [[sys-weapons]], [[sys-player]].
- implements: [[upgrades]], [[rarities]], [[player-stats]].
- gotchas: runtime GEs are never replicated (single-player). A missing SetByCaller returns 0 and logs an error ([[gas-api-notes]]). Effect cards are not built (M2).

## Relations
- [[systems-index]] — parent index.
- [[sys-status-effects]] — stacking arithmetic.
- [[sys-automation-tests]] — RogueRigs.Upgrades.* prove these rules.
