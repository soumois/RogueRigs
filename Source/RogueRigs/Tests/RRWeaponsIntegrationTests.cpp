// Rogue Rigs - integration tests of the weapons rework with the final Blueprints (RogueRigs.Integration.*, task-024-06): BP_Miner starts with the rifle only, a forced new-weapon card equips BP_Shotgun through the WBP_HUD level-up screen and its stat cards follow, BP_ShotgunPellet pierces a line of BP_Chaser, the place-turret input places BP_Turret that kills a robot, and the BP_Miner LevelUpDisc animation ends hidden after chained level-ups.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Enemies/RRChaser.h"
#include "Engine/GameInstance.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GAS/RRHealthAttributeSet.h"
#include "GAS/RRPlayerAttributeSet.h"
#include "Player/RRPlayerController.h"
#include "Progression/RRLevelUpComponent.h"
#include "Progression/RRProgressionComponent.h"
#include "Projectiles/RRShotgunPellet.h"
#include "Projectiles/RRTurretBullet.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestHarness.h"
#include "Tests/RRTestStatsRows.h"
#include "UI/RRHUDWidget.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
#include "Upgrades/RRUpgradeComponent.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRShotgun.h"
#include "Weapons/RRTurret.h"
#include "Weapons/RRTurretLauncher.h"
#include "Weapons/RRWeaponAttributeSet.h"

namespace RRWeaponsIntegrationTests
{
	using namespace RRTestHarness;

	/** Final Blueprints of the weapons loop. */
	const TCHAR* const GameModeClassPath = TEXT("/Game/RogueRigs/Blueprints/Player/BP_RRGameMode.BP_RRGameMode_C");
	const TCHAR* const PlayerControllerClassPath = TEXT("/Game/RogueRigs/Blueprints/Player/BP_RRPlayerController.BP_RRPlayerController_C");
	const TCHAR* const HUDClassPath = TEXT("/Game/RogueRigs/UI/WBP_HUD.WBP_HUD_C");
	const TCHAR* const ChaserClassPath = TEXT("/Game/RogueRigs/Blueprints/Enemies/BP_Chaser.BP_Chaser_C");
	const TCHAR* const BurstRifleClassPath = TEXT("/Game/RogueRigs/Blueprints/Weapons/BP_BurstRifle.BP_BurstRifle_C");
	const TCHAR* const ShotgunPelletClassPath = TEXT("/Game/RogueRigs/Blueprints/Weapons/BP_ShotgunPellet.BP_ShotgunPellet_C");
	const TCHAR* const TurretLauncherClassPath = TEXT("/Game/RogueRigs/Blueprints/Weapons/BP_TurretLauncher.BP_TurretLauncher_C");
	const TCHAR* const TurretClassPath = TEXT("/Game/RogueRigs/Blueprints/Weapons/BP_Turret.BP_Turret_C");
	const TCHAR* const TurretBulletClassPath = TEXT("/Game/RogueRigs/Blueprints/Weapons/BP_TurretBullet.BP_TurretBullet_C");

	/** Open row 42 of arena_m1 (floor from x 32 to 45): the miner on its west end, aiming +X. */
	const FIntPoint OpenMinerCell(32, 42);

	/** Robot cell for the turret test: 4 cells east of the turret placed 200 uu ahead of the miner (800 uu, inside Range 1500). */
	const FIntPoint TurretTargetCell(37, 42);

	/** Seed of the upgrade draws of the card test. */
	constexpr int32 IntegrationCardSeed = 20261005;

	/** Last step of the BP_Miner level-up disc (bp-miner, kept behaviour): DiscStep_Tick hides the disc and stops re-arming on the tick that sets step 5, pause or not. */
	constexpr int32 DiscLastStep = 5;

	/** Frame of the third level-up of the disc test: restarts the animation 0.5 s into the 0.8 s choice delay, so its 0.1 s timer is mid-run when the screens pause the world. */
	constexpr int32 ThirdLevelFrame = 30;

	/** Name of the BP_Miner function the disc timer runs (set by function name). */
	const TCHAR* const DiscTimerFunction = TEXT("DiscStep_Tick");

	// Protected-member accessors: Tests/RRTestAccess.h.
	using namespace RRTestAccess;

	/** Game instance of a test world. Declare it before the FTestWorld: it outlives the world, then drops its context pointer. */
	struct FTestGameInstance
	{
		/** Clears the (destroyed) world context pointer of the game instance and releases it. */
		~FTestGameInstance()
		{
			if (GameInstance.IsValid())
			{
				FGameInstanceAccess::WorldContextOf(GameInstance.Get()) = nullptr;
			}
		}

		/** The game instance, kept alive for the test. */
		TStrongObjectPtr<UGameInstance> GameInstance;
	};

	/** Gives the test world a game instance and BP_RRGameMode as authority game mode (real pause, widgets with a world). */
	bool StartGameMode(FAutomationTestBase& Test, const FTestWorld& TestWorld, FTestGameInstance& OutGameInstance)
	{
		UClass* GameModeClass = LoadTestClass<AGameModeBase>(Test, GameModeClassPath);
		UWorld* World = TestWorld.Get();
		FWorldContext* Context = GEngine->GetWorldContextFromWorld(World);
		if (!GameModeClass || !Test.TestNotNull(TEXT("test world has a world context"), Context))
		{
			return false;
		}
		UGameInstance* GameInstance = NewObject<UGameInstance>(GEngine);
		OutGameInstance.GameInstance.Reset(GameInstance);
		Context->OwningGameInstance = GameInstance;
		FGameInstanceAccess::WorldContextOf(GameInstance) = Context;
		World->SetGameInstance(GameInstance);
		World->GetWorldSettings()->DefaultGameMode = GameModeClass;
		World->SetGameMode(FURL());
		const AGameModeBase* GameMode = World->GetAuthGameMode();
		return Test.TestTrue(TEXT("authority game mode is BP_RRGameMode"), GameMode && GameMode->IsA(GameModeClass));
	}

	/** Spawns BP_RRPlayerController at the miner and possesses it. */
	ARRPlayerController* PossessMiner(FAutomationTestBase& Test, const FTestWorld& TestWorld, ARRMiner* Miner)
	{
		UClass* ControllerClass = LoadTestClass<ARRPlayerController>(Test, PlayerControllerClassPath);
		ARRPlayerController* Controller = ControllerClass ? TestWorld.Spawn<ARRPlayerController>(ControllerClass, Miner->GetActorLocation()) : nullptr;
		if (Test.TestNotNull(TEXT("BP_RRPlayerController spawned"), Controller))
		{
			Controller->Possess(Miner);
			Test.TestTrue(TEXT("controller possesses the miner"), Controller->GetPawn() == Miner);
		}
		return Controller;
	}

	/** Stops the auto-fire of every weapon of Miner. */
	void StopMinerWeapons(const ARRMiner* Miner)
	{
		for (ARRWeapon* Weapon : Miner->GetWeapons())
		{
			if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
			{
				Weapon->GetAutoFireComponent()->StopFiring();
			}
		}
	}

	/** Points the miner's stick aim along Direction (camera yaw 0: screen up = world +X, screen right = world +Y). */
	void AimAlong(ARRMiner* Miner, const FVector& Direction)
	{
		const FVector Dir = Direction.GetSafeNormal2D();
		Miner->ApplyStickAimInput(FVector2D(Dir.Y, Dir.X));
	}

	/** Spawns BP_Chaser with its feet on the floor at FloorLocation, actor and movement ticks off (it stays where placed). */
	ARRChaser* SpawnFrozenChaser(FAutomationTestBase& Test, const FTestWorld& TestWorld, UClass* ChaserClass, const FVector& FloorLocation)
	{
		const float HalfHeight = ChaserClass->GetDefaultObject<ARRChaser>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		const FTransform SpawnTransform(FloorLocation + FVector(0.f, 0.f, HalfHeight + 2.f));
		ARRChaser* Chaser = TestWorld.Get()->SpawnActorDeferred<ARRChaser>(ChaserClass, SpawnTransform, nullptr, nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (!Test.TestNotNull(TEXT("BP_Chaser spawned"), Chaser))
		{
			return nullptr;
		}
		Chaser->FinishSpawning(SpawnTransform);
		Chaser->SetActorTickEnabled(false);
		Chaser->GetCharacterMovement()->SetComponentTickEnabled(false);
		return Chaser;
	}

	/** Current Health of an actor with an ASC. */
	float GetHealth(const IAbilitySystemInterface* Actor)
	{
		return Actor->GetAbilitySystemComponent()->GetNumericAttribute(URRHealthAttributeSet::GetHealthAttribute());
	}

	/** Visibility of the widget named Name in Widget (Hidden when missing, with a test failure). */
	ESlateVisibility GetWidgetVisibility(FAutomationTestBase& Test, UUserWidget* Widget, const FString& Name)
	{
		const UWidget* Child = URRHUDWidget::FindWidgetInTree(Widget, *Name);
		Test.TestNotNull(FString::Printf(TEXT("WBP_HUD has %s"), *Name), Child);
		return Child ? Child->GetVisibility() : ESlateVisibility::Hidden;
	}

	/** Text of the text block named Name in Widget (empty when missing, with a test failure). */
	FString GetWidgetText(FAutomationTestBase& Test, UUserWidget* Widget, const FString& Name)
	{
		const UTextBlock* Text = Cast<UTextBlock>(URRHUDWidget::FindWidgetInTree(Widget, *Name));
		Test.TestNotNull(FString::Printf(TEXT("WBP_HUD has text %s"), *Name), Text);
		return Text ? Text->GetText().ToString() : FString();
	}

	/** Ticks until the level-up screen is open (MaxFrames at most); the frame count, or -1 when it never opened. */
	int32 TickUntilScreen(const FTestWorld& TestWorld, const URRLevelUpComponent* LevelUp, int32 MaxFrames = 120)
	{
		for (int32 Frame = 1; Frame <= MaxFrames; ++Frame)
		{
			TestWorld.Tick();
			if (LevelUp->IsAwaitingChoice())
			{
				return Frame;
			}
		}
		return -1;
	}

	/** XPGain of the miner owning Progression (its ASC attribute, read the way the component multiplies raw XP); at least KINDA_SMALL_NUMBER. */
	float GetXPGain(const URRProgressionComponent* Progression)
	{
		const IAbilitySystemInterface* Owner = Cast<IAbilitySystemInterface>(Progression->GetOwner());
		const UAbilitySystemComponent* ASC = Owner ? Owner->GetAbilitySystemComponent() : nullptr;
		const float Gain = ASC && ASC->HasAttributeSetForAttribute(URRPlayerAttributeSet::GetXPGainAttribute())
			? ASC->GetNumericAttribute(URRPlayerAttributeSet::GetXPGainAttribute()) : 1.f;
		return FMath::Max(Gain, KINDA_SMALL_NUMBER);
	}

	/** Adds exactly the XP missing for the next level. */
	void GrantOneLevel(URRProgressionComponent* Progression)
	{
		Progression->AddXP((Progression->GetXPToNextLevel() - Progression->GetXP()) / GetXPGain(Progression));
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRIntegrationStartLoadoutRifleOnlyTest, "RogueRigs.Integration.StartLoadoutRifleOnly",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRIntegrationStartLoadoutRifleOnlyTest::RunTest(const FString& Parameters)
{
	using namespace RRWeaponsIntegrationTests;

	FTestWorld TestWorld(TEXT("RRIntegrationLoadout"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	ARRMiner* Miner = Grid ? SpawnMiner(*this, TestWorld, Grid, OpenMinerCell) : nullptr;
	UClass* RifleClass = LoadTestClass<ARRWeapon>(*this, BurstRifleClassPath);
	UClass* ShotgunClass = LoadTestClass<ARRShotgun>(*this, ShotgunClassPath);
	UClass* LauncherClass = LoadTestClass<ARRTurretLauncher>(*this, TurretLauncherClassPath);
	if (!Miner || !RifleClass || !ShotgunClass || !LauncherClass)
	{
		return false;
	}
	StopMinerWeapons(Miner);

	// The Blueprint loadout as saved: the burst rifle alone, nothing guaranteed, three catalogue weapons, four slots.
	if (TestEqual(TEXT("BP_Miner starts with one weapon"), Miner->GetWeapons().Num(), 1))
	{
		TestTrue(TEXT("the starting weapon is BP_BurstRifle"), Miner->GetWeapons()[0] && Miner->GetWeapons()[0]->GetClass() == RifleClass);
	}
	TestNull(TEXT("no shotgun at start"), FindShotgun(Miner));
	TestNull(TEXT("no turret launcher at start"), Miner->FindTurretLauncher());
	TestEqual(TEXT("no guaranteed weapon class"), FMinerAccess::GuaranteedWeaponClassesOf(Miner).Num(), 0);
	TestEqual(TEXT("MaxWeapons 4"), FMinerAccess::MaxWeaponsOf(Miner), 4);
	const TArray<FRRWeaponCatalogueEntry>& Catalogue = Miner->GetWeaponCatalogue();
	TArray<UClass*> CatalogueClasses;
	for (const FRRWeaponCatalogueEntry& Entry : Catalogue)
	{
		CatalogueClasses.Add(Entry.WeaponClass.Get());
		TestFalse(FString::Printf(TEXT("catalogue entry %s has a description"), *GetNameSafe(Entry.WeaponClass.Get())), Entry.Description.IsEmpty());
	}
	TestEqual(TEXT("catalogue holds three weapons"), CatalogueClasses.Num(), 3);
	TestTrue(TEXT("catalogue lists BP_BurstRifle"), CatalogueClasses.Contains(RifleClass));
	TestTrue(TEXT("catalogue lists BP_Shotgun"), CatalogueClasses.Contains(ShotgunClass));
	TestTrue(TEXT("catalogue lists BP_TurretLauncher"), CatalogueClasses.Contains(LauncherClass));

	// Acquirable now: the shotgun and the launcher (the rifle is owned).
	TArray<UClass*> Acquirable;
	for (const FRRWeaponCatalogueEntry& Entry : Miner->GetAcquirableWeapons())
	{
		Acquirable.Add(Entry.WeaponClass.Get());
	}
	TestEqual(TEXT("two acquirable weapons"), Acquirable.Num(), 2);
	TestTrue(TEXT("BP_Shotgun acquirable"), Acquirable.Contains(ShotgunClass));
	TestTrue(TEXT("BP_TurretLauncher acquirable"), Acquirable.Contains(LauncherClass));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRIntegrationWeaponCardEquipsShotgunTest, "RogueRigs.Integration.WeaponCardEquipsShotgun",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRIntegrationWeaponCardEquipsShotgunTest::RunTest(const FString& Parameters)
{
	using namespace RRWeaponsIntegrationTests;

	// Declared before the world: released after it.
	FTestGameInstance GameInstance;
	FTestWorld TestWorld(TEXT("RRIntegrationWeaponCard"));
	UWorld* World = TestWorld.Get();
	UClass* ShotgunClass = LoadTestClass<ARRShotgun>(*this, ShotgunClassPath);
	UClass* PelletClass = LoadTestClass<ARRShotgunPellet>(*this, ShotgunPelletClassPath);
	UClass* HUDClass = LoadTestClass<URRHUDWidget>(*this, HUDClassPath);
	if (!ShotgunClass || !PelletClass || !HUDClass || !StartGameMode(*this, TestWorld, GameInstance))
	{
		return false;
	}

	// BP_Miner with its Blueprint loadout (rifle only); the weapon card is forced: catalogue reduced to its BP_Shotgun entry,
	// level interval 1 (every level is a weapon level: the first slot becomes the shotgun card while it is not owned).
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	ARRMiner* Miner = Grid ? SpawnMiner(*this, TestWorld, Grid, OpenMinerCell, [](ARRMiner* NewMiner)
	{
		FMinerAccess::WeaponCatalogueOf(NewMiner).RemoveAll([](const FRRWeaponCatalogueEntry& Entry)
		{
			return !Entry.WeaponClass || !Entry.WeaponClass->IsChildOf(ARRShotgun::StaticClass());
		});
		FUpgradeAccess::WeaponCardLevelIntervalOf(NewMiner->GetUpgrades()) = 1;
	}) : nullptr;
	if (!Miner || !TestEqual(TEXT("catalogue forced to the BP_Shotgun entry"), Miner->GetWeaponCatalogue().Num(), 1))
	{
		return false;
	}
	StopMinerWeapons(Miner);
	const ARRPlayerController* Controller = PossessMiner(*this, TestWorld, Miner);
	URRHUDWidget* HUD = CreateWidget<URRHUDWidget>(World, HUDClass);
	URRProgressionComponent* Progression = Miner->GetProgression();
	URRLevelUpComponent* LevelUp = Miner->GetLevelUp();
	URRUpgradeComponent* Upgrades = Miner->GetUpgrades();
	if (!Controller || !TestNotNull(TEXT("WBP_HUD created"), HUD) || !Progression || !LevelUp || !Upgrades)
	{
		return false;
	}
	// The world tick may run a garbage collection: keep the unreferenced widget alive.
	TStrongObjectPtr<URRHUDWidget> HUDRoot(HUD);
	HUD->BindToMiner(Miner);
	Upgrades->SetRandomSeed(IntegrationCardSeed);
	TestEqual(TEXT("rifle only before the card"), Miner->GetWeapons().Num(), 1);

	// Level 2: the screen opens after the choice delay with the shotgun card in slot 0, shown as a weapon card on WBP_HUD.
	GrantOneLevel(Progression);
	TestEqual(TEXT("level 2"), Progression->GetLevel(), 2);
	const int32 OpenFrame = TickUntilScreen(TestWorld, LevelUp);
	if (!TestTrue(TEXT("level-up screen opened"), OpenFrame > 0))
	{
		return false;
	}
	TestTrue(TEXT("world paused by the screen"), World->IsPaused());
	const TArray<FRRUpgradeCard> Offer = LevelUp->GetOfferCards();
	if (!TestEqual(TEXT("3 cards"), Offer.Num(), 3))
	{
		return false;
	}
	int32 WeaponCardIndex = INDEX_NONE;
	int32 WeaponCards = 0;
	for (int32 Index = 0; Index < Offer.Num(); ++Index)
	{
		if (Offer[Index].IsNewWeapon())
		{
			WeaponCardIndex = WeaponCardIndex == INDEX_NONE ? Index : WeaponCardIndex;
			++WeaponCards;
		}
	}
	TestEqual(TEXT("exactly one new-weapon card"), WeaponCards, 1);
	if (!TestEqual(TEXT("the weapon card is the first slot"), WeaponCardIndex, 0))
	{
		return false;
	}
	const FRRUpgradeCard& WeaponCard = Offer[WeaponCardIndex];
	TestTrue(TEXT("the weapon card offers BP_Shotgun"), WeaponCard.WeaponClass.Get() == ShotgunClass);
	TestEqual(TEXT("card title"), WeaponCard.DisplayName.ToString(), FString(TEXT("New weapon")));
	TestEqual(TEXT("card weapon name"), WeaponCard.TargetText.ToString(), FString(TEXT("Shotgun")));
	TestFalse(TEXT("card has the catalogue description"), WeaponCard.DescriptionText.IsEmpty());
	TestTrue(TEXT("UpgradeScreen visible"), GetWidgetVisibility(*this, HUD, TEXT("UpgradeScreen")) == ESlateVisibility::Visible);
	for (int32 Index = 0; Index < Offer.Num(); ++Index)
	{
		const bool bWeapon = Offer[Index].IsNewWeapon();
		TestEqual(FString::Printf(TEXT("CardName%d shows the card name"), Index), GetWidgetText(*this, HUD, FString::Printf(TEXT("CardName%d"), Index)), Offer[Index].DisplayName.ToString());
		TestEqual(FString::Printf(TEXT("CardTarget%d shows the target"), Index), GetWidgetText(*this, HUD, FString::Printf(TEXT("CardTarget%d"), Index)), Offer[Index].TargetText.ToString());
		const FString DescName = FString::Printf(TEXT("CardDesc%d"), Index);
		const ESlateVisibility DescVisibility = GetWidgetVisibility(*this, HUD, DescName);
		const ESlateVisibility ValueVisibility = GetWidgetVisibility(*this, HUD, FString::Printf(TEXT("CardValue%d"), Index));
		const bool bDescShown = DescVisibility == ESlateVisibility::Visible || DescVisibility == ESlateVisibility::SelfHitTestInvisible || DescVisibility == ESlateVisibility::HitTestInvisible;
		if (bWeapon)
		{
			// New-weapon card (task-169-04, wbp-upgrade-choice): CardDesc holds the lore, visible only when LoreText is not empty.
			const FString Lore = Offer[Index].LoreText.ToString();
			TestEqual(FString::Printf(TEXT("CardDesc%d %s"), Index, Lore.IsEmpty() ? TEXT("collapsed on a weapon card without lore") : TEXT("visible on a weapon card with lore")),
				bDescShown, !Lore.IsEmpty());
			if (bDescShown)
			{
				TestEqual(FString::Printf(TEXT("CardDesc%d shows the weapon lore"), Index), GetWidgetText(*this, HUD, DescName), Lore);
			}
		}
		else
		{
			// Stat card (task-080/084, wbp-hud card flavor line): CardDesc shows the flavor, the text after the first newline of
			// DescriptionText ("stat name\nflavor" from the text loader), visible only when that flavor is not empty.
			FString StatName;
			FString Flavor;
			Offer[Index].DescriptionText.ToString().Split(TEXT("\n"), &StatName, &Flavor);
			TestEqual(FString::Printf(TEXT("CardDesc%d %s"), Index, Flavor.IsEmpty() ? TEXT("collapsed on a stat card without flavor") : TEXT("visible on a stat card with a flavor line")),
				bDescShown, !Flavor.IsEmpty());
			if (bDescShown)
			{
				TestEqual(FString::Printf(TEXT("CardDesc%d shows the stat card flavor line"), Index), GetWidgetText(*this, HUD, DescName), Flavor);
			}
		}
		TestEqual(FString::Printf(TEXT("CardValue%d %s"), Index, bWeapon ? TEXT("hidden on the weapon card") : TEXT("shown on a stat card")),
			ValueVisibility == ESlateVisibility::Collapsed || ValueVisibility == ESlateVisibility::Hidden, bWeapon);
		if (bWeapon)
		{
			// The weapon variant (WeaponGroup) shows the header, the weapon name and the explanation.
			TestEqual(FString::Printf(TEXT("WeaponHeader%d shows the card name"), Index), GetWidgetText(*this, HUD, FString::Printf(TEXT("WeaponHeader%d"), Index)), Offer[Index].DisplayName.ToString());
			TestEqual(FString::Printf(TEXT("WeaponName%d shows the weapon name"), Index), GetWidgetText(*this, HUD, FString::Printf(TEXT("WeaponName%d"), Index)), Offer[Index].TargetText.ToString());
			TestEqual(FString::Printf(TEXT("WeaponDesc%d shows the description"), Index), GetWidgetText(*this, HUD, FString::Printf(TEXT("WeaponDesc%d"), Index)), Offer[Index].DescriptionText.ToString());
		}
	}

	// Picked through the HUD card button path: BP_Shotgun equipped with its Blueprint pellet, screen closed, world resumed.
	if (!TestTrue(TEXT("HUD ChooseUpgrade on the weapon card"), HUD->ChooseUpgrade(WeaponCardIndex)))
	{
		return false;
	}
	ARRShotgun* Shotgun = FindShotgun(Miner);
	if (!TestNotNull(TEXT("miner holds a shotgun"), Shotgun))
	{
		return false;
	}
	StopMinerWeapons(Miner);
	TestTrue(TEXT("the shotgun is BP_Shotgun"), Shotgun->GetClass() == ShotgunClass);
	TestEqual(TEXT("two weapons"), Miner->GetWeapons().Num(), 2);
	TestTrue(TEXT("BP_Shotgun fires BP_ShotgunPellet"), Shotgun->GetProjectileClass() && Shotgun->GetProjectileClass()->IsChildOf(PelletClass));
	TestEqual(TEXT("BP_Shotgun stats row Shotgun: CSV PelletCount"), Shotgun->GetPelletCount(), RRTestStats::WeaponRow(*this, RRTestStats::ShotgunRow).PelletCount);
	TestFalse(TEXT("screen closed"), LevelUp->IsAwaitingChoice());
	TestTrue(TEXT("UpgradeScreen collapsed"), GetWidgetVisibility(*this, HUD, TEXT("UpgradeScreen")) == ESlateVisibility::Collapsed);
	TestFalse(TEXT("world resumed"), World->IsPaused());
	TestTrue(TEXT("shotgun no longer acquirable"), Miner->GetAcquirableWeapons().Num() == 0);

	// Its cards appear: level up until a real offer holds a card targeting the shotgun, shown on WBP_HUD; picking it raises the shotgun only.
	const FGameplayAttribute RifleProbe = URRWeaponAttributeSet::GetDamageAttribute();
	int32 LevelsTried = 0;
	bool bShotgunCardPicked = false;
	FString Trace;
	for (; LevelsTried < 12 && !bShotgunCardPicked; ++LevelsTried)
	{
		GrantOneLevel(Progression);
		if (!TestTrue(FString::Printf(TEXT("screen opened at level %d"), Progression->GetLevel()), TickUntilScreen(TestWorld, LevelUp) > 0))
		{
			return false;
		}
		const TArray<FRRUpgradeCard> Cards = LevelUp->GetOfferCards();
		int32 ShotgunCardIndex = INDEX_NONE;
		for (int32 Index = 0; Index < Cards.Num(); ++Index)
		{
			TestFalse(FString::Printf(TEXT("level %d card %d is not a weapon card (shotgun owned)"), Progression->GetLevel(), Index), Cards[Index].IsNewWeapon());
			if (ShotgunCardIndex == INDEX_NONE && Cards[Index].Target == ERRUpgradeTarget::Weapon && Cards[Index].TargetWeapon.Get() == Shotgun)
			{
				ShotgunCardIndex = Index;
			}
		}
		Trace += FString::Printf(TEXT(" L%d:%s"), Progression->GetLevel(), ShotgunCardIndex == INDEX_NONE ? TEXT("-") : TEXT("shotgun"));
		if (ShotgunCardIndex == INDEX_NONE)
		{
			TestTrue(TEXT("HUD ChooseUpgrade(0)"), HUD->ChooseUpgrade(0));
			continue;
		}
		const FRRUpgradeCard Card = Cards[ShotgunCardIndex];
		TestEqual(TEXT("shotgun card target text"), GetWidgetText(*this, HUD, FString::Printf(TEXT("CardTarget%d"), ShotgunCardIndex)), Shotgun->GetDisplayName().ToString());
		const FGameplayAttribute Attribute = URRUpgradeComponent::GetStatAttribute(Card.Stat);
		const float ShotgunBefore = Shotgun->GetAbilitySystemComponent()->GetNumericAttribute(Attribute);
		const float RifleBefore = FindRifle(Miner)->GetAbilitySystemComponent()->GetNumericAttribute(Attribute);
		TestTrue(TEXT("HUD ChooseUpgrade on the shotgun card"), HUD->ChooseUpgrade(ShotgunCardIndex));
		const float ShotgunAfter = Shotgun->GetAbilitySystemComponent()->GetNumericAttribute(Attribute);
		AddInfo(FString::Printf(TEXT("WEAPONS shotgun card at level %d: %s %s %s, %.4f -> %.4f"), Progression->GetLevel(), *Card.DisplayName.ToString(),
			*Card.TargetText.ToString(), *Card.ValueText.ToString(), ShotgunBefore, ShotgunAfter));
		TestTrue(TEXT("the shotgun stat rose"), ShotgunAfter > ShotgunBefore);
		TestEqual(TEXT("the rifle stat is untouched"), FindRifle(Miner)->GetAbilitySystemComponent()->GetNumericAttribute(Attribute), RifleBefore);
		bShotgunCardPicked = true;
	}
	AddInfo(FString::Printf(TEXT("WEAPONS offers after the shotgun:%s"), *Trace));
	TestTrue(TEXT("a shotgun-targeted card appeared on the level-up screen"), bShotgunCardPicked);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRIntegrationShotgunPelletsPierceTest, "RogueRigs.Integration.ShotgunPelletsPierceRobots",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRIntegrationShotgunPelletsPierceTest::RunTest(const FString& Parameters)
{
	using namespace RRWeaponsIntegrationTests;

	FTestWorld TestWorld(TEXT("RRIntegrationPellets"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	ARRMiner* Miner = Grid ? SpawnMiner(*this, TestWorld, Grid, OpenMinerCell) : nullptr;
	UClass* ShotgunClass = LoadTestClass<ARRShotgun>(*this, ShotgunClassPath);
	UClass* PelletClass = LoadTestClass<ARRShotgunPellet>(*this, ShotgunPelletClassPath);
	UClass* ChaserClass = LoadTestClass<ARRChaser>(*this, ChaserClassPath);
	if (!Miner || !ShotgunClass || !PelletClass || !ChaserClass)
	{
		return false;
	}
	// The Blueprint shotgun as a new-weapon card adds it (ARRMiner::AddWeapon).
	ARRShotgun* Shotgun = Cast<ARRShotgun>(Miner->AddWeapon(ShotgunClass));
	if (!TestNotNull(TEXT("BP_Shotgun added"), Shotgun))
	{
		return false;
	}
	StopMinerWeapons(Miner);
	AimAlong(Miner, FVector::ForwardVector);
	Shotgun->GetAbilitySystemComponent()->SetNumericAttributeBase(URRWeaponAttributeSet::GetCritChanceAttribute(), 0.f);
	const int32 Pierce = Shotgun->GetPierceCount();
	TestEqual(TEXT("CSV Pierce"), Pierce, FMath::RoundToInt32(RRTestStats::WeaponRow(*this, RRTestStats::ShotgunRow).Pierce));
	const float PelletDamage = Shotgun->GetAbilitySystemComponent()->GetNumericAttribute(URRWeaponAttributeSet::GetDamageAttribute());

	// A line of Pierce + 1 frozen BP_Chaser on the center pellet ray, 55 uu apart from 460 uu: the +/-5 deg pellets pass beside them.
	const FVector Origin = Shotgun->GetShotOrigin();
	const FVector Aim = Miner->GetAimDirection().GetSafeNormal2D();
	TArray<ARRChaser*> Line;
	TArray<int32> Hits;
	Hits.Init(0, Pierce + 1);
	for (int32 Index = 0; Index <= Pierce; ++Index)
	{
		const FVector Point = Origin + Aim * (460.f + 55.f * Index);
		ARRChaser* Chaser = SpawnFrozenChaser(*this, TestWorld, ChaserClass, FVector(Point.X, Point.Y, Grid->GetCellCenter(OpenMinerCell.X, OpenMinerCell.Y).Z));
		if (!Chaser)
		{
			return false;
		}
		int32& Counter = Hits[Index];
		Chaser->GetHealthSet()->OnDamageReceived.AddLambda([&Counter](float, bool, float) { ++Counter; });
		Line.Add(Chaser);
	}
	const float ChaserHealth = GetHealth(Line[0]);
	AddInfo(FString::Printf(TEXT("WEAPONS pellets: BP_Chaser radius %.1f, health %.0f, pellet damage %.1f"),
		Line[0]->GetCapsuleComponent()->GetScaledCapsuleRadius(), ChaserHealth, PelletDamage));

	// One shot: 7 Blueprint pellets.
	URRAutoFireComponent* AutoFire = Shotgun->GetAutoFireComponent();
	AutoFire->StartFiring();
	AutoFire->AdvanceTime(0.001f);
	AutoFire->StopFiring();
	TestEqual(TEXT("one shot fired"), Shotgun->GetShotCount(), 1);
	int32 PelletCount = 0;
	int32 BlueprintPellets = 0;
	for (TActorIterator<ARRShotgunPellet> It(TestWorld.Get()); It; ++It)
	{
		++PelletCount;
		BlueprintPellets += It->IsA(PelletClass) ? 1 : 0;
	}
	TestEqual(TEXT("CSV PelletCount pellets spawned"), PelletCount, RRTestStats::WeaponRow(*this, RRTestStats::ShotgunRow).PelletCount);
	TestEqual(TEXT("every pellet is BP_ShotgunPellet"), BlueprintPellets, PelletCount);

	// Until the pellets are gone (lifetime Range / speed).
	int32 Frames = 0;
	for (bool bAny = true; bAny && Frames < 120; ++Frames)
	{
		TestWorld.Tick();
		bAny = false;
		for (TActorIterator<ARRShotgunPellet> It(TestWorld.Get()); It; ++It)
		{
			bAny |= IsValid(*It) && !It->IsActorBeingDestroyed();
		}
	}
	TestTrue(TEXT("pellets gone within 2 s"), Frames < 120);
	for (int32 Index = 0; Index < Line.Num(); ++Index)
	{
		const bool bPierced = Index < Pierce;
		TestEqual(FString::Printf(TEXT("robot %d: %s"), Index, bPierced ? TEXT("hit once by the center pellet") : TEXT("not hit (pierce spent)")), Hits[Index], bPierced ? 1 : 0);
		TestEqual(FString::Printf(TEXT("robot %d health"), Index), GetHealth(Line[Index]), bPierced ? ChaserHealth - PelletDamage : ChaserHealth, 1e-3f);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRIntegrationPlaceTurretKillsRobotTest, "RogueRigs.Integration.PlaceTurretInputKillsRobot",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRIntegrationPlaceTurretKillsRobotTest::RunTest(const FString& Parameters)
{
	using namespace RRWeaponsIntegrationTests;

	FTestWorld TestWorld(TEXT("RRIntegrationTurret"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	ARRMiner* Miner = Grid ? SpawnMiner(*this, TestWorld, Grid, OpenMinerCell) : nullptr;
	UClass* LauncherClass = LoadTestClass<ARRTurretLauncher>(*this, TurretLauncherClassPath);
	UClass* TurretClass = LoadTestClass<ARRTurret>(*this, TurretClassPath);
	UClass* BulletClass = LoadTestClass<ARRTurretBullet>(*this, TurretBulletClassPath);
	UClass* ChaserClass = LoadTestClass<ARRChaser>(*this, ChaserClassPath);
	if (!Miner || !LauncherClass || !TurretClass || !BulletClass || !ChaserClass)
	{
		return false;
	}
	ARRPlayerController* Controller = PossessMiner(*this, TestWorld, Miner);
	ARRTurretLauncher* Launcher = Cast<ARRTurretLauncher>(Miner->AddWeapon(LauncherClass));
	if (!Controller || !TestNotNull(TEXT("BP_TurretLauncher added"), Launcher))
	{
		return false;
	}
	StopMinerWeapons(Miner);
	AimAlong(Miner, FVector::ForwardVector);
	TestTrue(TEXT("the miner finds its launcher"), Miner->FindTurretLauncher() == Launcher);
	TestEqual(TEXT("full charges (CSV MagazineSize)"), Launcher->GetCharges(), FMath::RoundToInt32(RRTestStats::WeaponRow(*this, RRTestStats::TurretLauncherRow).MagazineSize));

	// The input: BP_RRPlayerController resolves its place-turret action, then one press runs its Started handler.
	TestNotNull(TEXT("BP_RRPlayerController resolves a place-turret action"), FPlayerControllerAccess::PlaceTurretActionOf(Controller));
	FPlayerControllerAccess::PressPlaceTurret(Controller);
	TArray<ARRTurret*> Turrets;
	for (TActorIterator<ARRTurret> It(TestWorld.Get()); It; ++It)
	{
		Turrets.Add(*It);
	}
	if (!TestEqual(TEXT("one turret placed by the press"), Turrets.Num(), 1))
	{
		return false;
	}
	ARRTurret* Turret = Turrets[0];
	TestTrue(TEXT("the turret is BP_Turret"), Turret->GetClass() == TurretClass);
	TestEqual(TEXT("one charge spent"), Launcher->GetCharges(), 1);
	TestTrue(TEXT("the turret fires BP_TurretBullet"), Turret->GetProjectileClass() && Turret->GetProjectileClass()->IsChildOf(BulletClass));
	TestEqual(TEXT("turret on the floor cell 200 uu ahead"), Grid->GetCellAtWorld(Turret->GetActorLocation()), FIntPoint(OpenMinerCell.X + 1, OpenMinerCell.Y));

	// A frozen BP_Chaser 4 cells east of the turret: locked, shot and killed by the turret alone (miner weapons stopped).
	ARRChaser* Chaser = SpawnFrozenChaser(*this, TestWorld, ChaserClass, Grid->GetCellCenter(TurretTargetCell.X, TurretTargetCell.Y));
	if (!Chaser)
	{
		return false;
	}
	const TWeakObjectPtr<ARRChaser> Weak(Chaser);
	int32 Hits = 0;
	Chaser->GetHealthSet()->OnDamageReceived.AddLambda([&Hits](float, bool, float) { ++Hits; });
	const double Start = TestWorld.Get()->GetTimeSeconds();
	bool bLocked = false;
	bool bDead = false;
	int32 Frame = 0;
	for (; Frame < 600 && !bDead; ++Frame)
	{
		TestWorld.Tick();
		bLocked |= Weak.IsValid() && Turret->GetTarget() == Weak.Get();
		bDead = !Weak.IsValid() || Weak->IsDead();
	}
	const double KillTime = TestWorld.Get()->GetTimeSeconds() - Start;
	AddInfo(FString::Printf(TEXT("WEAPONS turret: robot killed after %.3f s, %d hits, %d turret shots"), KillTime, Hits, Turret->GetShotCount()));
	TestTrue(TEXT("the turret locked the robot"), bLocked);
	TestTrue(TEXT("the robot is killed within the turret lifetime"), bDead && KillTime < Launcher->GetTurretLifetime());
	TestTrue(TEXT("the turret fired"), Turret->GetShotCount() > 0);
	TestTrue(TEXT("the robot took hits"), Hits > 0);
	TestEqual(TEXT("one turret placed"), Launcher->GetTurretsPlaced(), 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRIntegrationLevelUpDiscTest, "RogueRigs.Integration.LevelUpDiscEndsHidden",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRIntegrationLevelUpDiscTest::RunTest(const FString& Parameters)
{
	using namespace RRWeaponsIntegrationTests;

	// Declared before the world: released after it.
	FTestGameInstance GameInstance;
	FTestWorld TestWorld(TEXT("RRIntegrationDisc"));
	UWorld* World = TestWorld.Get();
	if (!StartGameMode(*this, TestWorld, GameInstance))
	{
		return false;
	}
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	ARRMiner* Miner = Grid ? SpawnMiner(*this, TestWorld, Grid, OpenMinerCell, DisableWeaponCards) : nullptr;
	if (!Miner || !PossessMiner(*this, TestWorld, Miner))
	{
		return false;
	}
	StopMinerWeapons(Miner);
	URRProgressionComponent* Progression = Miner->GetProgression();
	URRLevelUpComponent* LevelUp = Miner->GetLevelUp();
	Miner->GetUpgrades()->SetRandomSeed(IntegrationCardSeed);

	// The Blueprint disc: component LevelUpDisc and int variable DiscStep of BP_Miner, read by reflection.
	const FIntProperty* StepProperty = FindFProperty<FIntProperty>(Miner->GetClass(), TEXT("DiscStep"));
	const FObjectProperty* DiscProperty = FindFProperty<FObjectProperty>(Miner->GetClass(), TEXT("LevelUpDisc"));
	if (!TestNotNull(TEXT("BP_Miner has DiscStep"), StepProperty) || !TestNotNull(TEXT("BP_Miner has LevelUpDisc"), DiscProperty))
	{
		return false;
	}
	const USceneComponent* Disc = Cast<USceneComponent>(DiscProperty->GetObjectPropertyValue_InContainer(Miner));
	if (!TestNotNull(TEXT("LevelUpDisc is a scene component"), Disc))
	{
		return false;
	}
	auto IsShown = [Disc]() { return Disc->IsVisible() && !Disc->bHiddenInGame; };
	auto Step = [StepProperty, Miner]() { return StepProperty->GetPropertyValue_InContainer(Miner); };
	TestFalse(TEXT("disc hidden at start"), IsShown());

	// Chained level-ups: two levels in one call, then a third gained ThirdLevelFrame frames into the choice delay; each screen picked at once.
	Progression->AddXP((Progression->GetXPToNextLevelAt(1) + Progression->GetXPToNextLevelAt(2) + 2.f - Progression->GetXP()) / GetXPGain(Progression));
	TestEqual(TEXT("level 3 after two levels of XP"), Progression->GetLevel(), 3);
	FString Trace;
	int32 LastStep = Step();
	bool bLastShown = IsShown();
	bool bEverShown = bLastShown;
	int32 MaxStep = LastStep;
	int32 Picks = 0;
	int32 LastPickFrame = -1;
	auto Sample = [&](int32 Frame)
	{
		const int32 Now = Step();
		const bool bShown = IsShown();
		if (Now != LastStep || bShown != bLastShown)
		{
			Trace += FString::Printf(TEXT(" f%d:%d%s%s"), Frame, Now, bShown ? TEXT("V") : TEXT("h"), World->IsPaused() ? TEXT("P") : TEXT(""));
			LastStep = Now;
			bLastShown = bShown;
		}
		bEverShown |= bShown;
		MaxStep = FMath::Max(MaxStep, Now);
	};
	Sample(0);
	int32 Frame = 1;
	for (; Frame <= 600; ++Frame)
	{
		if (Frame == ThirdLevelFrame)
		{
			GrantOneLevel(Progression);
		}
		TestWorld.Tick();
		Sample(Frame);
		if (LevelUp->IsAwaitingChoice())
		{
			if (Picks == 0)
			{
				// The screens open while the restarted animation runs: the disc stays shown and its timer survives the pause.
				TestTrue(TEXT("disc shown when the first screen opens"), IsShown());
				TestTrue(TEXT("disc timer pending when the first screen opens"), UKismetSystemLibrary::K2_TimerExists(Miner, DiscTimerFunction));
			}
			TestTrue(FString::Printf(TEXT("pick %d accepted"), Picks + 1), LevelUp->ChooseUpgrade(0));
			++Picks;
			LastPickFrame = Frame;
			Sample(Frame);
		}
		else if (Frame > ThirdLevelFrame && LevelUp->GetPendingChoices() == 0 && !LevelUp->IsChoiceDelayActive())
		{
			break;
		}
	}
	TestEqual(TEXT("level 4"), Progression->GetLevel(), 4);
	TestEqual(TEXT("three picks"), Picks, 3);
	TestFalse(TEXT("world resumed after the last pick"), World->IsPaused());

	// After the last pick: one second of running world for the animation (documented 0.6 s) to finish.
	for (int32 After = 1; After <= 60; ++After)
	{
		TestWorld.Tick();
		Sample(Frame + After);
	}
	AddInfo(FString::Printf(TEXT("WEAPONS disc: last pick at frame %d; step/visibility changes (P = paused):%s"), LastPickFrame, *Trace));
	TestTrue(TEXT("the disc was shown"), bEverShown);
	TestFalse(TEXT("the disc ends hidden"), IsShown());
	TestFalse(TEXT("the disc animation finished (no DiscStep_Tick timer left)"), UKismetSystemLibrary::K2_TimerExists(Miner, DiscTimerFunction));
	TestEqual(TEXT("DiscStep ends on the last step (5, bp-miner)"), Step(), DiscLastStep);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
