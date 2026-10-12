// Rogue Rigs - automation tests of the gameplay HUD events (RogueRigs.HUD.Events*): miner health and damage, weapon ammo and slots, run flow alert-bar icons, horde relay, locator target, XP gained, HUD panel binding.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Enemies/RREnemySpawner.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Player/RRMiner.h"
#include "Progression/RRProgressionComponent.h"
#include "Tests/RRTestHarness.h"
#include "Tests/RRTestHUDWidget.h"
#include "UObject/StrongObjectPtr.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"
#include "Weapons/RRWeaponAttributeSet.h"

namespace RRHUDEventTests
{
	/** Miner cell on arena_m1_fine (open floor), as in the pressure tests. */
	const FIntPoint EventMinerCell = RRTestHarness::FineCell(FIntPoint(40, 40));

	/** Fine grid and a miner whose weapons are stopped; null on failure. */
	ARRMiner* SpawnStoppedMiner(FAutomationTestBase& Test, const RRTestHarness::FTestWorld& TestWorld)
	{
		const ARRArenaGrid* Grid = RRTestHarness::SpawnFineGrid(Test, TestWorld);
		ARRMiner* Miner = Grid ? RRTestHarness::SpawnMiner(Test, TestWorld, Grid, EventMinerCell) : nullptr;
		if (Miner)
		{
			for (ARRWeapon* Weapon : Miner->GetWeapons())
			{
				if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
				{
					Weapon->GetAutoFireComponent()->StopFiring();
				}
			}
		}
		return Miner;
	}

	/** A recorder kept alive for the test. */
	TStrongObjectPtr<URRTestHUDEventRecorder> MakeRecorder()
	{
		return TStrongObjectPtr<URRTestHUDEventRecorder>(NewObject<URRTestHUDEventRecorder>());
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRHUDEventsHealthTest, "RogueRigs.HUD.EventsHealth",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRHUDEventsHealthTest::RunTest(const FString& Parameters)
{
	using namespace RRHUDEventTests;

	RRTestHarness::FTestWorld TestWorld(TEXT("RRHUDEventsHealth"));
	ARRMiner* Miner = SpawnStoppedMiner(*this, TestWorld);
	UAbilitySystemComponent* ASC = Miner ? Miner->GetAbilitySystemComponent() : nullptr;
	if (!TestNotNull(TEXT("miner ASC"), ASC))
	{
		return false;
	}
	TestTrue(TEXT("base stats applied (MaxHealth > 0)"), Miner->GetMaxHealth() > 0.f);
	ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetMaxHealthAttribute(), 100.f);
	ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), 100.f);

	TStrongObjectPtr<URRTestHUDEventRecorder> Recorder = MakeRecorder();
	Miner->OnHealthChanged.AddDynamic(Recorder.Get(), &URRTestHUDEventRecorder::HandleHealthChanged);
	Miner->OnDamaged.AddDynamic(Recorder.Get(), &URRTestHUDEventRecorder::HandleDamaged);

	// No change: same values written again send nothing.
	ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), 100.f);
	ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetMaxHealthAttribute(), 100.f);
	TestEqual(TEXT("no event without a change"), Recorder->HealthCalls.Num() + Recorder->DamageCalls.Num(), 0);

	// A hit: one health change and one damage event with the payload.
	ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), 70.f);
	if (TestEqual(TEXT("one health event per hit"), Recorder->HealthCalls.Num(), 1) && TestEqual(TEXT("one damage event per hit"), Recorder->DamageCalls.Num(), 1))
	{
		TestNearlyEqual(TEXT("health current 70"), Recorder->HealthCalls[0].X, 70.0, double(KINDA_SMALL_NUMBER));
		TestNearlyEqual(TEXT("health max 100"), Recorder->HealthCalls[0].Y, 100.0, double(KINDA_SMALL_NUMBER));
		TestNearlyEqual(TEXT("damage amount 30"), Recorder->DamageCalls[0].X, 30.0, double(KINDA_SMALL_NUMBER));
		TestNearlyEqual(TEXT("damage fraction 0.7"), Recorder->DamageCalls[0].Y, 0.7, double(KINDA_SMALL_NUMBER));
	}

	// A heal: health event, no damage. A MaxHealth decrease that clamps Health: no damage.
	ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), 90.f);
	TestEqual(TEXT("heal sends a health event"), Recorder->HealthCalls.Num(), 2);
	ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetMaxHealthAttribute(), 60.f);
	TestEqual(TEXT("heal and clamp are not damage"), Recorder->DamageCalls.Num(), 1);
	if (TestTrue(TEXT("clamp sends a health event"), Recorder->HealthCalls.Num() > 2))
	{
		TestNearlyEqual(TEXT("last health event = clamped values"), Recorder->HealthCalls.Last().X, 60.0, double(KINDA_SMALL_NUMBER));
		TestNearlyEqual(TEXT("last health event max 60"), Recorder->HealthCalls.Last().Y, 60.0, double(KINDA_SMALL_NUMBER));
	}

	// MaxHealth 0 never sends a health event (the Blueprint divides by Max).
	const int32 HealthBefore = Recorder->HealthCalls.Num();
	ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetMaxHealthAttribute(), 0.f);
	bool bNoZeroMax = true;
	for (int32 Index = HealthBefore; Index < Recorder->HealthCalls.Num(); ++Index)
	{
		bNoZeroMax &= Recorder->HealthCalls[Index].Y > 0.0;
	}
	TestTrue(TEXT("never a health event with Max 0"), bNoZeroMax);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRHUDEventsWeaponsTest, "RogueRigs.HUD.EventsWeapons",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRHUDEventsWeaponsTest::RunTest(const FString& Parameters)
{
	using namespace RRHUDEventTests;

	RRTestHarness::FTestWorld TestWorld(TEXT("RRHUDEventsWeapons"));
	ARRMiner* Miner = SpawnStoppedMiner(*this, TestWorld);
	ARRWeapon* Rifle = Miner && Miner->GetWeapons().Num() > 0 ? Miner->GetWeapons()[0].Get() : nullptr;
	UAbilitySystemComponent* ASC = Rifle ? Rifle->GetAbilitySystemComponent() : nullptr;
	URRAutoFireComponent* AutoFire = Rifle ? Rifle->GetAutoFireComponent() : nullptr;
	if (!TestNotNull(TEXT("rifle ASC"), ASC) || !TestNotNull(TEXT("rifle auto-fire"), AutoFire))
	{
		return false;
	}
	const int32 Rounds = Rifle->GetRounds();
	const int32 Magazine = Rifle->GetMagazineRounds();
	TestTrue(TEXT("full magazine of at least 2 rounds"), Magazine >= 2 && Rounds == Magazine);
	TestTrue(TEXT("slot 0 owned by the rifle"), Miner->GetWeaponSlotHUD(0).bOwned && Miner->GetWeaponSlotHUD(0).Weapon == Rifle);

	TStrongObjectPtr<URRTestHUDEventRecorder> Recorder = MakeRecorder();
	Rifle->OnAmmoChanged.AddDynamic(Recorder.Get(), &URRTestHUDEventRecorder::HandleAmmoChanged);
	Miner->OnWeaponSlotChanged.AddDynamic(Recorder.Get(), &URRTestHUDEventRecorder::HandleWeaponSlotChanged);

	// One round used: one ammo event and one slot event with the payload.
	ASC->SetNumericAttributeBase(URRWeaponAttributeSet::GetAmmoAttribute(), Rounds - 1.f);
	if (TestEqual(TEXT("one ammo event per round"), Recorder->AmmoCalls.Num(), 1))
	{
		TestEqual(TEXT("ammo event weapon"), Recorder->AmmoWeapons[0].Get(), Rifle);
		TestEqual(TEXT("ammo event rounds"), Recorder->AmmoCalls[0], FIntPoint(Rounds - 1, Magazine));
	}
	if (TestEqual(TEXT("one slot event per round"), Recorder->Slots.Num(), 1))
	{
		TestEqual(TEXT("slot event index 0"), Recorder->SlotIndices[0], 0);
		TestEqual(TEXT("slot event rounds"), Recorder->Slots[0].Rounds, Rounds - 1);
		TestEqual(TEXT("slot event magazine"), Recorder->Slots[0].MagazineSize, Magazine);
	}

	// No HUD change (same value, or a fraction that rounds down the same): no event.
	ASC->SetNumericAttributeBase(URRWeaponAttributeSet::GetAmmoAttribute(), Rounds - 1.f);
	ASC->SetNumericAttributeBase(URRWeaponAttributeSet::GetAmmoAttribute(), Rounds - 0.5f);
	TestEqual(TEXT("no ammo event without a rounded change"), Recorder->AmmoCalls.Num(), 1);
	TestEqual(TEXT("no slot event without a change"), Recorder->Slots.Num(), 1);

	// Magazine upgrade: one event of each with the new size.
	ASC->SetNumericAttributeBase(URRWeaponAttributeSet::GetMagazineSizeAttribute(), Magazine + 2.f);
	if (TestEqual(TEXT("one ammo event per magazine change"), Recorder->AmmoCalls.Num(), 2))
	{
		TestEqual(TEXT("new magazine size"), Recorder->AmmoCalls[1].Y, Magazine + 2);
	}
	TestEqual(TEXT("one slot event per magazine change"), Recorder->Slots.Num(), 2);

	// Reload: one slot event when it starts (bReloading), one when it ends (refilled, not reloading).
	ASC->SetNumericAttributeBase(URRWeaponAttributeSet::GetAmmoAttribute(), 0.f);
	const int32 SlotsBeforeReload = Recorder->Slots.Num();
	AutoFire->StartFiring();
	for (int32 Step = 0; Step < 600 && !AutoFire->IsReloading(); ++Step)
	{
		AutoFire->AdvanceTime(0.01f);
	}
	if (TestTrue(TEXT("reload started"), AutoFire->IsReloading()))
	{
		TestTrue(TEXT("slot event on reload start"), Recorder->Slots.Num() > SlotsBeforeReload && Recorder->Slots.Last().bReloading);
		const int32 SlotsAtReload = Recorder->Slots.Num();
		AutoFire->AdvanceTime(0.01f);
		TestEqual(TEXT("reload progress alone sends nothing"), Recorder->Slots.Num(), SlotsAtReload);
		for (int32 Step = 0; Step < 2000 && AutoFire->IsReloading(); ++Step)
		{
			AutoFire->AdvanceTime(0.01f);
		}
		AutoFire->StopFiring();
		TestFalse(TEXT("reload finished"), AutoFire->IsReloading());
		const FRRWeaponSlotHUD* EndSlot = Recorder->Slots.FindByPredicate([](const FRRWeaponSlotHUD& Slot) { return !Slot.bReloading && Slot.Rounds > 0; });
		TestNotNull(TEXT("slot event after the reload (refilled, not reloading)"), EndSlot);
	}
	AutoFire->StopFiring();

	// A new weapon fills the next slot: one event for slot 1.
	UClass* ShotgunClass = LoadClass<ARRWeapon>(nullptr, RRTestHarness::ShotgunClassPath);
	if (ShotgunClass && !Miner->OwnsWeaponClass(ShotgunClass))
	{
		const int32 SlotsBefore = Recorder->Slots.Num();
		ARRWeapon* Shotgun = Miner->AddWeapon(ShotgunClass);
		if (TestNotNull(TEXT("shotgun added"), Shotgun) && TestEqual(TEXT("one slot event on acquisition"), Recorder->Slots.Num(), SlotsBefore + 1))
		{
			TestEqual(TEXT("acquired slot index"), Recorder->SlotIndices.Last(), Miner->GetWeapons().IndexOfByKey(Shotgun));
			TestTrue(TEXT("acquired slot owned"), Recorder->Slots.Last().bOwned && Recorder->Slots.Last().Weapon == Shotgun);
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRHUDEventsRunFlowTest, "RogueRigs.HUD.EventsRunFlow",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRHUDEventsRunFlowTest::RunTest(const FString& Parameters)
{
	using namespace RRHUDEventTests;

	RRTestHarness::FTestWorld TestWorld(TEXT("RRHUDEventsRunFlow"));
	ARRMiner* Miner = SpawnStoppedMiner(*this, TestWorld);
	URRRunFlowSubsystem* RunFlow = TestWorld.Get()->GetSubsystem<URRRunFlowSubsystem>();
	if (!Miner || !TestNotNull(TEXT("run flow"), RunFlow))
	{
		return false;
	}
	TestEqual(TEXT("miner registered with the run flow"), RunFlow->GetMiner(), Miner);
	RunFlow->SetConfig(FRRRunFlowConfig());

	TStrongObjectPtr<URRTestHUDEventRecorder> Recorder = MakeRecorder();
	RunFlow->OnAlertBarEventsChanged.AddDynamic(Recorder.Get(), &URRTestHUDEventRecorder::HandleAlertBarEventsChanged);
	RunFlow->OnHordeIncoming.AddDynamic(Recorder.Get(), &URRTestHUDEventRecorder::HandleHordeIncoming);
	RunFlow->OnLocatorTargetChanged.AddDynamic(Recorder.Get(), &URRTestHUDEventRecorder::HandleLocatorTargetChanged);

	// Alert-bar icons: sent once, then nothing while nothing changes.
	RunFlow->RefreshAlertBarEvents();
	RunFlow->RefreshAlertBarEvents();
	if (TestEqual(TEXT("alert-bar icons sent once"), Recorder->AlertBarCalls.Num(), 1))
	{
		TestTrue(TEXT("console icon first, shuttle last"), Recorder->AlertBarCalls[0].Num() >= 2
			&& Recorder->AlertBarCalls[0][0].Type == ERRAlertBarEventType::Console && Recorder->AlertBarCalls[0].Last().Type == ERRAlertBarEventType::Extraction);
		TestEqual(TEXT("GetAlertBarEvents = sent list"), RunFlow->GetAlertBarEvents().Num(), Recorder->AlertBarCalls[0].Num());
	}

	// Spawner: registers at BeginPlay; its horde announcement is relayed once with the payload.
	ARREnemySpawner* Spawner = TestWorld.Spawn<ARREnemySpawner>(ARREnemySpawner::StaticClass(), FVector::ZeroVector);
	if (TestNotNull(TEXT("spawner"), Spawner))
	{
		TestEqual(TEXT("spawner registered"), RunFlow->GetSpawner(), Spawner);
		Spawner->OnHordeIncoming.Broadcast(2, 5.f);
		if (TestEqual(TEXT("horde relayed once"), Recorder->HordeCalls.Num(), 1))
		{
			TestEqual(TEXT("relayed horde index"), Recorder->HordeCalls[0].Key, 2);
			TestNearlyEqual(TEXT("relayed seconds"), Recorder->HordeCalls[0].Value, 5.f, KINDA_SMALL_NUMBER);
		}
	}

	// Locator: none before the drop; console on the drop; none once hacked (no shuttle yet).
	ERRLocatorTarget Target = ERRLocatorTarget::Console;
	FVector Location;
	TestFalse(TEXT("no locator target before the drop"), RunFlow->GetLocatorTarget(Target, Location));
	TestEqual(TEXT("no locator event before the drop"), Recorder->LocatorTargets.Num(), 0);
	RunFlow->SetRandomSeed(7);
	ARRConsole* Console = RunFlow->SkipToConsole();
	if (TestNotNull(TEXT("console dropped"), Console) && TestEqual(TEXT("one locator event on the drop"), Recorder->LocatorTargets.Num(), 1))
	{
		TestEqual(TEXT("locator on the console"), Recorder->LocatorTargets[0], ERRLocatorTarget::Console);
		TestTrue(TEXT("locator at the console"), Recorder->LocatorLocations[0].Equals(Console->GetActorLocation()));
		const int32 AlertBarBefore = Recorder->AlertBarCalls.Num();
		RunFlow->CompleteConsoleHack();
		TestEqual(TEXT("one locator event on the hack"), Recorder->LocatorTargets.Num(), 2);
		TestEqual(TEXT("locator none after the hack"), Recorder->LocatorTargets.Last(), ERRLocatorTarget::None);
		TestTrue(TEXT("console icon done sent"), Recorder->AlertBarCalls.Num() > AlertBarBefore && Recorder->AlertBarCalls.Last()[0].bDone);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRHUDEventsXPTest, "RogueRigs.HUD.EventsXP",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRHUDEventsXPTest::RunTest(const FString& Parameters)
{
	using namespace RRHUDEventTests;

	RRTestHarness::FTestWorld TestWorld(TEXT("RRHUDEventsXP"));
	ARRMiner* Miner = SpawnStoppedMiner(*this, TestWorld);
	URRProgressionComponent* Progression = Miner ? Miner->GetProgression() : nullptr;
	if (!TestNotNull(TEXT("progression"), Progression))
	{
		return false;
	}
	TStrongObjectPtr<URRTestHUDEventRecorder> Recorder = MakeRecorder();
	Progression->OnXPGained.AddDynamic(Recorder.Get(), &URRTestHUDEventRecorder::HandleXPGained);
	const float XPBefore = Progression->GetXP();
	Progression->AddXP(0.f);
	TestEqual(TEXT("no XP event for 0"), Recorder->XPGains.Num(), 0);
	Progression->AddXP(0.25f);
	if (TestEqual(TEXT("one XP event per gain"), Recorder->XPGains.Num(), 1))
	{
		TestNearlyEqual(TEXT("XP gained = XP added"), Recorder->XPGains[0], Progression->GetXP() - XPBefore, 1e-4f);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRHUDEventsPanelBindingTest, "RogueRigs.HUD.EventsPanelBinding",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRHUDEventsPanelBindingTest::RunTest(const FString& Parameters)
{
	using namespace RRHUDEventTests;

	RRTestHarness::FTestWorld TestWorld(TEXT("RRHUDEventsPanelBinding"));
	const ARRArenaGrid* Grid = RRTestHarness::SpawnFineGrid(*this, TestWorld);
	URRRunFlowSubsystem* RunFlow = TestWorld.Get()->GetSubsystem<URRRunFlowSubsystem>();
	URRTestHUDWidget* HUD = CreateWidget<URRTestHUDWidget>(TestWorld.Get(), URRTestHUDWidget::StaticClass());
	if (!Grid || !TestNotNull(TEXT("run flow"), RunFlow) || !TestNotNull(TEXT("HUD"), HUD) || !TestNotNull(TEXT("HUD widget tree"), HUD->WidgetTree.Get()))
	{
		return false;
	}
	TStrongObjectPtr<URRTestHUDWidget> HUDRoot(HUD);

	// Tree: canvas root with two panels, one nested in a vertical box, plus a plain widget.
	UCanvasPanel* Canvas = HUD->WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("Root"));
	UVerticalBox* Box = HUD->WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("Box"));
	HUD->WidgetTree->RootWidget = Canvas;
	URRTestHUDPanelWidget* PanelA = CreateWidget<URRTestHUDPanelWidget>(HUD, URRTestHUDPanelWidget::StaticClass(), TEXT("PanelA"));
	URRTestHUDPanelWidget* PanelB = CreateWidget<URRTestHUDPanelWidget>(HUD, URRTestHUDPanelWidget::StaticClass(), TEXT("PanelB"));
	if (!TestNotNull(TEXT("panel A"), PanelA) || !TestNotNull(TEXT("panel B"), PanelB))
	{
		return false;
	}
	Canvas->AddChild(PanelA);
	Canvas->AddChild(Box);
	Box->AddChild(PanelB);
	TestEqual(TEXT("HUD finds both panels"), HUD->GetPanels().Num(), 2);

	// Run flow first, no miner yet (the controller's BeginPlay before possession).
	HUD->BindToMiner(nullptr);
	HUD->BindToRunFlow(RunFlow);
	HUD->BindToRunFlow(RunFlow);
	HUD->BindToSpawner(nullptr);
	for (const URRTestHUDPanelWidget* Panel : { PanelA, PanelB })
	{
		TestEqual(TEXT("run flow bound once"), Panel->GetRunFlowBindCount(), 1);
		TestEqual(TEXT("panel run flow"), Panel->GetRunFlow(), RunFlow);
		TestEqual(TEXT("miner hook once (null)"), Panel->GetMinerBindCount(), 1);
		TestEqual(TEXT("spawner hook once (null)"), Panel->GetSpawnerBindCount(), 1);
	}

	// Late miner and spawner: they register with the run flow at BeginPlay and the HUD binds them, then hands them to the panels.
	ARRMiner* Miner = RRTestHarness::SpawnMiner(*this, TestWorld, Grid, EventMinerCell);
	ARREnemySpawner* Spawner = TestWorld.Spawn<ARREnemySpawner>(ARREnemySpawner::StaticClass(), FVector::ZeroVector);
	if (!Miner || !TestNotNull(TEXT("spawner"), Spawner))
	{
		return false;
	}
	HUD->BindToMiner(Miner);
	for (const URRTestHUDPanelWidget* Panel : { PanelA, PanelB })
	{
		TestEqual(TEXT("late miner reaches the panel"), Panel->GetMiner(), Miner);
		TestEqual(TEXT("miner hook: null, then the miner once"), Panel->GetMinerBindCount(), 2);
		TestEqual(TEXT("late spawner reaches the panel"), Panel->GetSpawner(), Spawner);
		TestEqual(TEXT("spawner hook: null, then the spawner once"), Panel->GetSpawnerBindCount(), 2);
		TestEqual(TEXT("run flow still bound once"), Panel->GetRunFlowBindCount(), 1);
	}
	TestEqual(TEXT("the HUD slots read the late miner"), HUD->GetWeaponSlotCount(), Miner->GetMaxWeapons());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRHUDFindWidgetInPanelsTest, "RogueRigs.HUD.FindWidgetInPanels",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRHUDFindWidgetInPanelsTest::RunTest(const FString& Parameters)
{
	RRTestHarness::FTestWorld TestWorld(TEXT("RRHUDFindWidgetInPanels"));
	URRTestHUDWidget* HUD = CreateWidget<URRTestHUDWidget>(TestWorld.Get(), URRTestHUDWidget::StaticClass());
	if (!TestNotNull(TEXT("HUD"), HUD) || !TestNotNull(TEXT("HUD widget tree"), HUD->WidgetTree.Get()))
	{
		return false;
	}
	TStrongObjectPtr<URRTestHUDWidget> HUDRoot(HUD);

	// HUD: canvas > box > panel A; panel A: box > text PanelText + panel B; panel B: text DeepText (a panel inside a panel).
	UCanvasPanel* Canvas = HUD->WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("Root"));
	UVerticalBox* Box = HUD->WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("Box"));
	HUD->WidgetTree->RootWidget = Canvas;
	Canvas->AddChild(Box);
	URRTestHUDPanelWidget* PanelA = CreateWidget<URRTestHUDPanelWidget>(HUD, URRTestHUDPanelWidget::StaticClass(), TEXT("PanelA"));
	URRTestHUDPanelWidget* PanelB = PanelA ? CreateWidget<URRTestHUDPanelWidget>(PanelA, URRTestHUDPanelWidget::StaticClass(), TEXT("PanelB")) : nullptr;
	if (!TestNotNull(TEXT("panel A tree"), PanelA ? PanelA->WidgetTree.Get() : nullptr) || !TestNotNull(TEXT("panel B tree"), PanelB ? PanelB->WidgetTree.Get() : nullptr))
	{
		return false;
	}
	Box->AddChild(PanelA);
	UVerticalBox* PanelBox = PanelA->WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("PanelBox"));
	PanelA->WidgetTree->RootWidget = PanelBox;
	UTextBlock* PanelText = PanelA->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("PanelText"));
	PanelBox->AddChild(PanelText);
	PanelBox->AddChild(PanelB);
	UTextBlock* DeepText = PanelB->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("DeepText"));
	PanelB->WidgetTree->RootWidget = DeepText;

	// The engine lookup only sees the root tree: the reason for the helper.
	TestNull(TEXT("GetWidgetFromName misses a panel widget"), HUD->GetWidgetFromName(TEXT("PanelText")));
	TestTrue(TEXT("root tree widget found"), HUD->FindHUDWidget(TEXT("Box")) == Box);
	TestTrue(TEXT("panel widget found"), HUD->FindHUDWidget(TEXT("PanelText")) == PanelText);
	TestTrue(TEXT("widget of a nested panel found"), HUD->FindHUDWidget(TEXT("DeepText")) == DeepText);
	TestTrue(TEXT("static helper, same result"), URRHUDWidget::FindWidgetInTree(HUD, TEXT("DeepText")) == DeepText);
	TestNull(TEXT("unknown name"), HUD->FindHUDWidget(TEXT("NoSuchWidget")));
	TestNull(TEXT("no name"), HUD->FindHUDWidget(NAME_None));
	TestNull(TEXT("a panel does not search its parent"), URRHUDWidget::FindWidgetInTree(PanelA, TEXT("Box")));
	TestNull(TEXT("null root"), URRHUDWidget::FindWidgetInTree(nullptr, TEXT("Box")));
	TestNull(TEXT("unknown variable"), URRHUDWidget::FindUserWidgetWithProperty(HUD, TEXT("NoSuchVariable")));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
