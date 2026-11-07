// Rogue Rigs - base class of the in-run HUD widget: health, XP, level-up, upgrade choice, alert bar, objective and game-over events fed by C++.

#include "UI/RRHUDWidget.h"

#include "AbilitySystemComponent.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Widget.h"
#include "EngineUtils.h"
#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "TimerManager.h"
#include "UnrealClient.h"
#include "Enemies/RREnemySpawner.h"
#include "Enemies/RRWaveDirector.h"
#include "GameFramework/PlayerController.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Objectives/RRBonusChest.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Objectives/RRShuttle.h"
#include "Player/RRGameMode.h"
#include "Player/RRMiner.h"
#include "Progression/RRLevelUpComponent.h"
#include "Progression/RRProgressionComponent.h"
#include "RogueRigs.h"
#include "Run/RRRunTransitionSubsystem.h"
#include "Text/RRSubtitleSubsystem.h"
#include "Text/RRTextContentSubsystem.h"
#include "UI/RRHUDPanelWidget.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRTurretLauncher.h"
#include "Weapons/RRWeaponAttributeSet.h"

TArray<URRHUDPanelWidget*> URRHUDWidget::GetPanels() const
{
	TArray<URRHUDPanelWidget*> Panels;
	if (WidgetTree)
	{
		WidgetTree->ForEachWidget([&Panels](UWidget* Widget)
		{
			if (URRHUDPanelWidget* Panel = Cast<URRHUDPanelWidget>(Widget))
			{
				Panels.Add(Panel);
			}
		});
	}
	return Panels;
}

UWidget* URRHUDWidget::FindWidgetInTree(const UUserWidget* Root, FName Name)
{
	if (!Root || !Root->WidgetTree || Name.IsNone())
	{
		return nullptr;
	}
	if (UWidget* Found = Root->WidgetTree->FindWidget(Name))
	{
		return Found;
	}
	// Not in this tree: search the trees of the user widgets placed in it (HUD panels), depth first in tree order.
	UWidget* Found = nullptr;
	Root->WidgetTree->ForEachWidget([&Found, Name](UWidget* Widget)
	{
		const UUserWidget* Child = Found ? nullptr : Cast<UUserWidget>(Widget);
		if (Child)
		{
			Found = FindWidgetInTree(Child, Name);
		}
	});
	return Found;
}

UUserWidget* URRHUDWidget::FindUserWidgetWithProperty(const UUserWidget* Root, FName PropertyName)
{
	if (!Root || PropertyName.IsNone())
	{
		return nullptr;
	}
	if (Root->GetClass()->FindPropertyByName(PropertyName))
	{
		return const_cast<UUserWidget*>(Root);
	}
	UUserWidget* Found = nullptr;
	if (Root->WidgetTree)
	{
		Root->WidgetTree->ForEachWidget([&Found, PropertyName](UWidget* Widget)
		{
			const UUserWidget* Child = Found ? nullptr : Cast<UUserWidget>(Widget);
			if (Child)
			{
				Found = FindUserWidgetWithProperty(Child, PropertyName);
			}
		});
	}
	return Found;
}

const TArray<FRRAlertBarEvent>& URRHUDWidget::GetAlertBarEvents() const
{
	static const TArray<FRRAlertBarEvent> NoEvents;
	URRRunFlowSubsystem* RunFlow = BoundRunFlow.Get();
	return RunFlow ? RunFlow->GetAlertBarEvents() : NoEvents;
}

void URRHUDWidget::HandleAlertBarEventsChanged(const TArray<FRRAlertBarEvent>& Events)
{
	OnAlertEventsChanged(Events);
}

void URRHUDWidget::BindToMiner(ARRMiner* Miner)
{
	UnbindMiner();
	for (URRHUDPanelWidget* Panel : GetPanels())
	{
		Panel->SetHUD(this);
		Panel->SetMiner(Miner);
	}

	UAbilitySystemComponent* ASC = Miner ? Miner->GetAbilitySystemComponent() : nullptr;
	if (!ASC)
	{
		return;
	}
	BoundASC = ASC;
	HealthChangedHandle = ASC->GetGameplayAttributeValueChangeDelegate(URRHealthAttributeSet::GetHealthAttribute())
		.AddUObject(this, &URRHUDWidget::HandleHealthAttributeChanged);
	MaxHealthChangedHandle = ASC->GetGameplayAttributeValueChangeDelegate(URRHealthAttributeSet::GetMaxHealthAttribute())
		.AddUObject(this, &URRHUDWidget::HandleHealthAttributeChanged);
	BroadcastHealth();

	if (URRProgressionComponent* Progression = Miner->GetProgression())
	{
		BoundProgression = Progression;
		Progression->OnXPChanged.AddUniqueDynamic(this, &URRHUDWidget::HandleXPChanged);
		Progression->OnLevelUp.AddUniqueDynamic(this, &URRHUDWidget::HandleLevelUp);
		HandleXPChanged(Progression->GetXP(), Progression->GetXPToNextLevel(), Progression->GetLevel());
	}

	if (URRLevelUpComponent* LevelUp = Miner->GetLevelUp())
	{
		BoundLevelUp = LevelUp;
		LevelUp->OnUpgradeOfferShown.AddUniqueDynamic(this, &URRHUDWidget::HandleUpgradeOfferShown);
		LevelUp->OnUpgradeOfferClosed.AddUniqueDynamic(this, &URRHUDWidget::HandleUpgradeOfferClosed);
		if (LevelUp->IsAwaitingChoice())
		{
			HandleUpgradeOfferShown(LevelUp->GetOfferCards(), LevelUp->CanReroll());
		}
	}

	BoundMiner = Miner;
	Miner->OnWeaponAdded.AddUniqueDynamic(this, &URRHUDWidget::HandleWeaponAdded);
	BindToTurretLauncher(Miner->FindTurretLauncher());

	// Minimap and locator follow the miner's own transform updates (fired only when it really moves), never a HUD tick.
	if (USceneComponent* Root = Miner->GetRootComponent())
	{
		BoundMinerRoot = Root;
		MinerMovedHandle = Root->TransformUpdated.AddUObject(this, &URRHUDWidget::HandleMinerMoved);
		USceneComponent* LocatorComponent = Miner->FindComponentByClass<UCameraComponent>();
		LocatorComponent = LocatorComponent ? LocatorComponent : Root;
		BoundLocatorComponent = LocatorComponent;
		LocatorMovedHandle = LocatorComponent->TransformUpdated.AddUObject(this, &URRHUDWidget::HandleLocatorViewMoved);
	}
	WarmUpVignette();
	// A minimap created here already sent its texture and view (with this miner); an existing one shows the new miner once.
	const bool bHadMinimap = Minimap != nullptr;
	TryCreateMinimap();
	if (bHadMinimap && Minimap)
	{
		Minimap->Update(Miner);
		BroadcastMinimapView();
	}
	BroadcastLocatorView();
}

void URRHUDWidget::UnbindMinerMovement()
{
	if (USceneComponent* Root = BoundMinerRoot.Get())
	{
		Root->TransformUpdated.Remove(MinerMovedHandle);
	}
	if (USceneComponent* LocatorComponent = BoundLocatorComponent.Get())
	{
		LocatorComponent->TransformUpdated.Remove(LocatorMovedHandle);
	}
	BoundMinerRoot.Reset();
	BoundLocatorComponent.Reset();
	MinerMovedHandle.Reset();
	LocatorMovedHandle.Reset();
	bMinimapViewSent = false;
	bLocatorViewSent = false;
}

void URRHUDWidget::BindToTurretLauncher(ARRTurretLauncher* Launcher)
{
	if (ARRTurretLauncher* Previous = BoundTurretLauncher.Get())
	{
		Previous->OnTurretChargesChanged.RemoveDynamic(this, &URRHUDWidget::HandleTurretChargesChanged);
	}
	BoundTurretLauncher.Reset();
	if (!Launcher)
	{
		return;
	}
	BoundTurretLauncher = Launcher;
	Launcher->OnTurretChargesChanged.AddUniqueDynamic(this, &URRHUDWidget::HandleTurretChargesChanged);
	HandleTurretChargesChanged(Launcher->GetCharges(), Launcher->GetMaxCharges(), Launcher->IsRecharging() ? Launcher->GetRechargeDuration() : 0.f);
}

void URRHUDWidget::HandleWeaponAdded(ARRWeapon* Weapon)
{
	if (!BoundTurretLauncher.IsValid())
	{
		if (ARRTurretLauncher* Launcher = Cast<ARRTurretLauncher>(Weapon))
		{
			BindToTurretLauncher(Launcher);
		}
	}
}

void URRHUDWidget::HandleTurretChargesChanged(int32 Charges, int32 MaxCharges, float RechargeDuration)
{
	OnTurretChargesChanged(Charges, MaxCharges, RechargeDuration);
}

float URRHUDWidget::GetTurretRechargeProgress() const
{
	const ARRTurretLauncher* Launcher = BoundTurretLauncher.Get();
	return Launcher ? Launcher->GetRechargeProgress() : 0.f;
}

int32 URRHUDWidget::GetWeaponSlotCount() const
{
	const ARRMiner* Miner = BoundMiner.Get();
	return Miner ? FMath::Max(Miner->GetMaxWeapons(), 0) : 0;
}

FRRWeaponSlotHUD URRHUDWidget::GetWeaponSlot(int32 SlotIndex) const
{
	const ARRMiner* Miner = BoundMiner.Get();
	return Miner ? Miner->GetWeaponSlotHUD(SlotIndex) : FRRWeaponSlotHUD();
}

TArray<FRRWeaponSlotHUD> URRHUDWidget::GetWeaponSlots() const
{
	TArray<FRRWeaponSlotHUD> Slots;
	const int32 SlotCount = GetWeaponSlotCount();
	Slots.Reserve(SlotCount);
	if (const ARRMiner* Miner = BoundMiner.Get())
	{
		for (const TObjectPtr<ARRWeapon>& Weapon : Miner->GetWeapons())
		{
			if (Slots.Num() >= SlotCount)
			{
				break;
			}
			if (IsValid(Weapon))
			{
				Slots.Add(MakeWeaponSlot(Weapon));
			}
		}
	}
	Slots.SetNum(SlotCount);
	return Slots;
}

FRRWeaponSlotHUD URRHUDWidget::MakeWeaponSlot(const ARRWeapon* Weapon)
{
	return FRRWeaponSlotHUD::Make(Weapon);
}

bool URRHUDWidget::ChooseUpgrade(int32 Index)
{
	URRLevelUpComponent* LevelUp = BoundLevelUp.Get();
	return LevelUp && LevelUp->ChooseUpgrade(Index);
}

bool URRHUDWidget::RequestReroll()
{
	URRLevelUpComponent* LevelUp = BoundLevelUp.Get();
	return LevelUp && LevelUp->RequestReroll();
}

bool URRHUDWidget::CanRerollUpgrade() const
{
	const URRLevelUpComponent* LevelUp = BoundLevelUp.Get();
	return LevelUp && LevelUp->CanReroll();
}

int32 URRHUDWidget::GetUpgradeRerollCost() const
{
	const URRLevelUpComponent* LevelUp = BoundLevelUp.Get();
	return LevelUp ? LevelUp->GetRerollCost() : 0;
}

void URRHUDWidget::BindToGameMode(ARRGameMode* GameMode)
{
	if (ARRGameMode* Previous = BoundGameMode.Get())
	{
		Previous->OnGameOver.RemoveDynamic(this, &URRHUDWidget::HandleGameOver);
	}
	BoundGameMode = GameMode;
	for (URRHUDPanelWidget* Panel : GetPanels())
	{
		Panel->SetHUD(this);
		Panel->SetGameMode(GameMode);
	}
	if (GameMode)
	{
		GameMode->OnGameOver.AddUniqueDynamic(this, &URRHUDWidget::HandleGameOver);
	}
}

void URRHUDWidget::BindToRunFlow(URRRunFlowSubsystem* RunFlow)
{
	if (URRRunFlowSubsystem* Previous = BoundRunFlow.Get())
	{
		Previous->OnAlertChanged.RemoveDynamic(this, &URRHUDWidget::HandleAlertChanged);
		Previous->OnObjectiveChanged.RemoveDynamic(this, &URRHUDWidget::HandleObjectiveChanged);
		Previous->OnObjectiveProgressChanged.RemoveDynamic(this, &URRHUDWidget::HandleObjectiveProgressChanged);
		Previous->OnExtractionReady.RemoveDynamic(this, &URRHUDWidget::HandleExtractionReady);
		Previous->OnShuttleCountdown.RemoveDynamic(this, &URRHUDWidget::HandleShuttleCountdown);
		Previous->OnRunEnded.RemoveDynamic(this, &URRHUDWidget::HandleRunEnded);
		Previous->OnMinerRegistered.RemoveDynamic(this, &URRHUDWidget::HandleMinerRegistered);
		Previous->OnSpawnerRegistered.RemoveDynamic(this, &URRHUDWidget::HandleSpawnerRegistered);
		Previous->OnAlertBarEventsChanged.RemoveDynamic(this, &URRHUDWidget::HandleAlertBarEventsChanged);
		Previous->OnLocatorTargetChanged.RemoveDynamic(this, &URRHUDWidget::HandleLocatorTargetChanged);
		Previous->OnConsoleDropped.RemoveDynamic(this, &URRHUDWidget::HandleConsoleDropped);
		Previous->OnBonusChestDropped.RemoveDynamic(this, &URRHUDWidget::HandleBonusChestDropped);
		Previous->OnShuttleLanded.RemoveDynamic(this, &URRHUDWidget::HandleShuttleLanded);
	}
	if (URRRunLostSubsystem* PreviousRunLost = BoundRunLost.Get())
	{
		PreviousRunLost->OnRunLost.RemoveDynamic(this, &URRHUDWidget::HandleRunLost);
	}
	BoundRunLost.Reset();
	BoundRunFlow = RunFlow;
	for (URRHUDPanelWidget* Panel : GetPanels())
	{
		Panel->SetHUD(this);
		Panel->SetRunFlow(RunFlow);
	}
	if (!RunFlow)
	{
		return;
	}
	// Late binding: a miner or spawner that begins play after this HUD registers with the run flow.
	RunFlow->OnMinerRegistered.AddUniqueDynamic(this, &URRHUDWidget::HandleMinerRegistered);
	RunFlow->OnSpawnerRegistered.AddUniqueDynamic(this, &URRHUDWidget::HandleSpawnerRegistered);
	RunFlow->OnAlertChanged.AddUniqueDynamic(this, &URRHUDWidget::HandleAlertChanged);
	RunFlow->OnObjectiveChanged.AddUniqueDynamic(this, &URRHUDWidget::HandleObjectiveChanged);
	RunFlow->OnObjectiveProgressChanged.AddUniqueDynamic(this, &URRHUDWidget::HandleObjectiveProgressChanged);
	RunFlow->OnExtractionReady.AddUniqueDynamic(this, &URRHUDWidget::HandleExtractionReady);
	RunFlow->OnShuttleCountdown.AddUniqueDynamic(this, &URRHUDWidget::HandleShuttleCountdown);
	RunFlow->OnRunEnded.AddUniqueDynamic(this, &URRHUDWidget::HandleRunEnded);
	RunFlow->OnAlertBarEventsChanged.AddUniqueDynamic(this, &URRHUDWidget::HandleAlertBarEventsChanged);
	RunFlow->OnLocatorTargetChanged.AddUniqueDynamic(this, &URRHUDWidget::HandleLocatorTargetChanged);
	RunFlow->OnConsoleDropped.AddUniqueDynamic(this, &URRHUDWidget::HandleConsoleDropped);
	RunFlow->OnBonusChestDropped.AddUniqueDynamic(this, &URRHUDWidget::HandleBonusChestDropped);
	RunFlow->OnShuttleLanded.AddUniqueDynamic(this, &URRHUDWidget::HandleShuttleLanded);
	// Death (task-484): the HUD hides behind the run-lost screen, after the death sequence, not at the lethal hit.
	URRRunLostSubsystem* RunLost = URRRunLostSubsystem::Get(RunFlow);
	BoundRunLost = RunLost;
	if (RunLost)
	{
		RunLost->OnRunLost.AddUniqueDynamic(this, &URRHUDWidget::HandleRunLost);
	}

	// Push the current state once.
	OnAlertChanged(RunFlow->GetAlert(), RunFlow->HasPendingObjective());
	if (ARRConsole* Console = RunFlow->GetConsole())
	{
		HandleObjectiveChanged(Console, Console->GetConsoleState(), Console->GetMarkedRockCount(), Console->GetInitialMarkedRockCount());
		OnObjectiveProgressChanged(Console->GetHackProgress(), Console->GetConsoleState() == ERRConsoleState::Hacking);
	}
	else
	{
		HandleObjectiveChanged(nullptr, ERRConsoleState::Locked, 0, 0);
	}
	if (RunFlow->IsExtractionReady())
	{
		OnExtractionReady();
	}
	if (const ARRShuttle* Shuttle = RunFlow->GetShuttle())
	{
		OnShuttleCountdown(Shuttle->GetRemainingSeconds(), Shuttle->GetActorLocation());
	}
	if (RunFlow->IsRunEnded())
	{
		OnRunEnded(RunFlow->WasRunSuccessful());
		// A level 1-3 success ends the flow without an end screen (intermission, task-206-03); the run won end screen waits for the
		// end of the last level's stats screen (task-317-01).
		const URRRunTransitionSubsystem* Transition = URRRunTransitionSubsystem::Get(this);
		if (!RunFlow->WasRunSuccessful() || !Transition || (Transition->IsRunWon() && !Transition->IsInIntermission()))
		{
			ShowEndScreen(RunFlow->WasRunSuccessful(), false);
		}
	}
	if (RunLost && RunLost->IsRunLostShown() && RunLost->GetRecap().bMinerDied)
	{
		ShowEndScreen(false, true);
	}
	OnAlertEventsChanged(RunFlow->GetAlertBarEvents());
}

void URRHUDWidget::BindToSpawner(ARREnemySpawner* Spawner)
{
	if (ARREnemySpawner* Previous = BoundSpawner.Get())
	{
		Previous->OnHordeIncoming.RemoveDynamic(this, &URRHUDWidget::HandleHordeIncoming);
	}
	BoundSpawner = Spawner;
	for (URRHUDPanelWidget* Panel : GetPanels())
	{
		Panel->SetHUD(this);
		Panel->SetSpawner(Spawner);
	}
	if (!Spawner)
	{
		return;
	}
	Spawner->OnHordeIncoming.AddUniqueDynamic(this, &URRHUDWidget::HandleHordeIncoming);

	// Bound during a banner: show it for the time left.
	const int32 Active = Spawner->GetActiveHorde();
	if (Active != INDEX_NONE && Spawner->GetHordeState(Active) == ERRHordeState::Announced)
	{
		// Same banner: keep its line (and subtitle), show one only when none was picked yet.
		const URRTextContentSubsystem* TextContent = URRTextContentSubsystem::Get(this);
		if (TextContent && TextContent->GetCurrentAlertLine(FRRTextContentLibrary::AlertHordeIncoming).IsEmpty())
		{
			ShowHordeAlertLine();
		}
		OnHordeIncoming(Spawner->GetSecondsUntilHordeFront());
	}
	// The run flow owns the alert-bar icons (its registered spawner): recompute them now; a change comes back through
	// HandleAlertBarEventsChanged.
	if (URRRunFlowSubsystem* RunFlow = BoundRunFlow.Get())
	{
		RunFlow->RefreshAlertBarEvents();
	}
}

float URRHUDWidget::EstimateBarPosition(float Alert, float SecondsUntil, float AlertFillSeconds)
{
	return URRRunFlowSubsystem::EstimateBarPosition(Alert, SecondsUntil, AlertFillSeconds);
}

bool URRHUDWidget::GetObjectiveTarget(ERRLocatorTarget& OutTarget, FVector& OutWorldLocation) const
{
	OutTarget = ERRLocatorTarget::None;
	OutWorldLocation = FVector::ZeroVector;
	const URRRunFlowSubsystem* RunFlow = BoundRunFlow.Get();
	return RunFlow && RunFlow->GetLocatorTarget(OutTarget, OutWorldLocation);
}

bool URRHUDWidget::GetObjectiveLocator(FRRObjectiveLocator& OutLocator, float EdgeMargin) const
{
	OutLocator = FRRObjectiveLocator();
	if (!GetObjectiveTarget(OutLocator.Target, OutLocator.WorldLocation))
	{
		return false;
	}
	FMinimalViewInfo View;
	if (!GetLocatorView(View))
	{
		return false;
	}
	const FVector2D Viewport = UWidgetLayoutLibrary::GetViewportSize(const_cast<URRHUDWidget*>(this));
	const float Scale = UWidgetLayoutLibrary::GetViewportScale(const_cast<URRHUDWidget*>(this));
	if (Viewport.X <= 0.f || Viewport.Y <= 0.f || Scale <= 0.f)
	{
		return false;
	}
	OutLocator.ViewportSize = Viewport / Scale;
	ProjectToViewport(View, OutLocator.ViewportSize, OutLocator.WorldLocation, OutLocator.ScreenPosition);
	OutLocator.bOnScreen = ComputeEdgeLocator(OutLocator.ScreenPosition, OutLocator.ViewportSize, EdgeMargin, OutLocator.EdgePosition, OutLocator.ArrowAngle);
	return true;
}

bool URRHUDWidget::ProjectToViewport(const FMinimalViewInfo& View, const FVector2D& ViewportSize, const FVector& WorldLocation, FVector2D& OutScreenPosition)
{
	const FVector Local = View.Rotation.UnrotateVector(WorldLocation - View.Location);
	const double TanHalf = FMath::Tan(FMath::DegreesToRadians(FMath::Clamp(View.FOV, 1.f, 170.f) * 0.5));
	const double Aspect = ViewportSize.Y > 0.0 ? ViewportSize.X / ViewportSize.Y : 1.0;
	const FVector2D Center = ViewportSize * 0.5;
	if (Local.X <= KINDA_SMALL_NUMBER)
	{
		// Behind the camera: far out along the camera-space direction (right = +Y, up = +Z, screen Y grows downward).
		const FVector2D Direction = FVector2D(Local.Y, -Local.Z * Aspect).GetSafeNormal();
		OutScreenPosition = Center + (Direction.IsNearlyZero() ? FVector2D(0.0, 1.0) : Direction) * (ViewportSize.X + ViewportSize.Y) * 10.0;
		return false;
	}
	const double NdcX = Local.Y / (Local.X * TanHalf);
	const double NdcY = Local.Z * Aspect / (Local.X * TanHalf);
	OutScreenPosition = FVector2D((NdcX + 1.0) * 0.5 * ViewportSize.X, (1.0 - NdcY) * 0.5 * ViewportSize.Y);
	return true;
}

bool URRHUDWidget::ComputeEdgeLocator(const FVector2D& ScreenPosition, const FVector2D& ViewportSize, float EdgeMargin, FVector2D& OutEdgePosition, float& OutArrowAngle)
{
	const FVector2D Center = ViewportSize * 0.5;
	const FVector2D Delta = ScreenPosition - Center;
	OutArrowAngle = Delta.IsNearlyZero() ? 0.f : static_cast<float>(FMath::RadiansToDegrees(FMath::Atan2(Delta.Y, Delta.X)));
	const bool bOnScreen = ScreenPosition.X >= 0.0 && ScreenPosition.Y >= 0.0 && ScreenPosition.X <= ViewportSize.X && ScreenPosition.Y <= ViewportSize.Y;
	if (bOnScreen)
	{
		OutEdgePosition = ScreenPosition;
		return true;
	}
	const FVector2D Half(FMath::Max(Center.X - EdgeMargin, 0.0), FMath::Max(Center.Y - EdgeMargin, 0.0));
	const double ScaleX = FMath::Abs(Delta.X) > KINDA_SMALL_NUMBER ? Half.X / FMath::Abs(Delta.X) : TNumericLimits<double>::Max();
	const double ScaleY = FMath::Abs(Delta.Y) > KINDA_SMALL_NUMBER ? Half.Y / FMath::Abs(Delta.Y) : TNumericLimits<double>::Max();
	OutEdgePosition = Center + Delta * FMath::Min(ScaleX, ScaleY);
	return false;
}

bool URRHUDWidget::GetLocatorView(FMinimalViewInfo& OutView) const
{
	// The miner camera component is current inside its own TransformUpdated; the camera manager cache is only refreshed after
	// every tick group (one frame late there, and it would miss the last frame of a move).
	if (UCameraComponent* Camera = Cast<UCameraComponent>(BoundLocatorComponent.Get()))
	{
		Camera->GetCameraView(0.f, OutView);
		return true;
	}
	const APlayerController* Controller = GetOwningPlayer();
	if (!Controller || !Controller->PlayerCameraManager)
	{
		return false;
	}
	OutView = Controller->PlayerCameraManager->GetCameraCacheView();
	return true;
}

void URRHUDWidget::TryCreateMinimap()
{
	if (!MinimapConfig.bEnabled || Minimap)
	{
		// Created once per HUD (a destroyed grid is not replaced).
		return;
	}
	ARRArenaGrid* Grid = nullptr;
	if (UWorld* World = GetWorld())
	{
		for (TActorIterator<ARRArenaGrid> It(World); It; ++It)
		{
			Grid = *It;
			break;
		}
	}
	if (!Grid || Grid->GetGridSize().X <= 0 || Grid->GetGridSize().Y <= 0)
	{
		// The grid builds in its BeginPlay: retry on the next ticks at startup only (miner moves retry too).
		UWorld* World = GetWorld();
		if (World && !bMinimapRetryPending && MinimapRetries < MinimapRetryFrames)
		{
			bMinimapRetryPending = true;
			World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &URRHUDWidget::RetryCreateMinimap));
		}
		return;
	}
	URRMinimap* NewMinimap = NewObject<URRMinimap>(this);
	if (!NewMinimap->Initialize(Grid, MinimapConfig))
	{
		return;
	}
	Minimap = NewMinimap;
	MinimapDirtiedHandle = Minimap->OnPixelsDirtied.AddUObject(this, &URRHUDWidget::HandleMinimapPixelsDirtied);
	++MinimapTextureReadyCount;
	OnMinimapTextureReady.Broadcast(Minimap->GetTexture());
	Minimap->Update(BoundMiner.Get());
	BroadcastMinimapView();
}

void URRHUDWidget::RetryCreateMinimap()
{
	bMinimapRetryPending = false;
	++MinimapRetries;
	TryCreateMinimap();
}

void URRHUDWidget::HandleMinerMoved(USceneComponent* UpdatedComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	const ARRMiner* Miner = BoundMiner.Get();
	if (!Miner || !MinimapConfig.bEnabled)
	{
		return;
	}
	if (!Minimap)
	{
		TryCreateMinimap();
		return;
	}
	// Seen pass only when the miner entered another cell; pixels sent at once (one dirty rectangle).
	const bool bContentChanged = Minimap->Update(Miner);
	const FVector2D MinerPosition = Minimap->WorldToTexture(Miner->GetActorLocation());
	const bool bViewMoved = !bMinimapViewSent
		|| FVector2D::DistSquared(MinerPosition, LastMinimapMinerPosition) > FMath::Square(static_cast<double>(MinimapConfig.ViewChangeThresholdCells));
	if (bContentChanged || bViewMoved)
	{
		BroadcastMinimapView();
	}
}

void URRHUDWidget::HandleLocatorViewMoved(USceneComponent* UpdatedComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	ERRLocatorTarget Target = ERRLocatorTarget::None;
	FVector TargetLocation;
	const ARRMiner* Miner = BoundMiner.Get();
	FMinimalViewInfo View;
	if (!Miner || !GetObjectiveTarget(Target, TargetLocation) || !GetLocatorView(View))
	{
		// No target: the arrow is hidden (the target change already sent None), nothing moves on screen.
		return;
	}
	if (bLocatorViewSent
		&& View.Location.Equals(LastLocatorViewLocation, LocatorMoveThreshold)
		&& View.Rotation.Equals(LastLocatorViewRotation, LocatorTurnThresholdDegrees)
		&& Miner->GetActorLocation().Equals(LastLocatorMinerLocation, LocatorMoveThreshold))
	{
		return;
	}
	BroadcastLocatorView();
}

void URRHUDWidget::HandleMinimapPixelsDirtied()
{
	UWorld* World = GetWorld();
	if (!World || bMinimapFlushPending)
	{
		return;
	}
	bMinimapFlushPending = true;
	World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &URRHUDWidget::FlushMinimapPixels));
}

void URRHUDWidget::FlushMinimapPixels()
{
	bMinimapFlushPending = false;
	if (Minimap && Minimap->FlushDirtyPixels())
	{
		BroadcastMinimapView();
	}
}

void URRHUDWidget::HandleViewportResized(FViewport* Viewport, uint32 Unused)
{
	if (bLocatorViewSent)
	{
		BroadcastLocatorView();
	}
}

void URRHUDWidget::HandleLocatorTargetChanged(ERRLocatorTarget Target, FVector WorldLocation)
{
	BroadcastLocatorView();
}

void URRHUDWidget::HandleConsoleDropped(ARRConsole* Console)
{
	BroadcastMinimapView();
}

void URRHUDWidget::HandleBonusChestDropped(ARRBonusChest* BonusChest)
{
	BroadcastMinimapView();
}

void URRHUDWidget::HandleShuttleLanded(ARRShuttle* Shuttle)
{
	BroadcastMinimapView();
}

void URRHUDWidget::BroadcastMinimapView()
{
	FRRMinimapState State;
	if (!GetMinimapState(State))
	{
		return;
	}
	bMinimapViewSent = BoundMiner.IsValid();
	LastMinimapMinerPosition = State.MinerPosition;
	++MinimapViewChangedCount;
	OnMinimapViewChanged.Broadcast(State);
}

void URRHUDWidget::BroadcastLocatorView()
{
	FRRObjectiveLocator Locator;
	const bool bHasLocator = GetObjectiveLocator(Locator, LocatorEdgeMargin);
	FMinimalViewInfo View;
	const ARRMiner* Miner = BoundMiner.Get();
	if (Miner && GetLocatorView(View))
	{
		bLocatorViewSent = true;
		LastLocatorViewLocation = View.Location;
		LastLocatorViewRotation = View.Rotation;
		LastLocatorMinerLocation = Miner->GetActorLocation();
	}
	++LocatorViewChangedCount;
	OnLocatorViewChanged.Broadcast(bHasLocator, Locator);
}

UTexture2D* URRHUDWidget::GetMinimapTexture() const
{
	return Minimap ? Minimap->GetTexture() : nullptr;
}

bool URRHUDWidget::GetMinimapState(FRRMinimapState& OutState) const
{
	if (!Minimap)
	{
		OutState = FRRMinimapState();
		return false;
	}
	const URRRunFlowSubsystem* RunFlow = BoundRunFlow.Get();
	if (!RunFlow)
	{
		const UWorld* World = GetWorld();
		RunFlow = World ? World->GetSubsystem<URRRunFlowSubsystem>() : nullptr;
	}
	return Minimap->GetState(BoundMiner.Get(), RunFlow, OutState);
}

FVector2D URRHUDWidget::GetMinimapOffset(FVector2D Position, FVector2D Center, float Radius, bool bClampToCircle, bool& bOutOfRange)
{
	FVector2D Offset = Radius > 0.f ? (Position - Center) / Radius : FVector2D::ZeroVector;
	const double LengthSquared = Offset.SizeSquared();
	bOutOfRange = LengthSquared > 1.0;
	if (bOutOfRange && bClampToCircle)
	{
		Offset /= FMath::Sqrt(LengthSquared);
	}
	return Offset;
}

void URRHUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	for (URRHUDPanelWidget* Panel : GetPanels())
	{
		Panel->SetHUD(this);
	}
	WarmUpVignette();
	TryCreateMinimap();
}

void URRHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	for (URRHUDPanelWidget* Panel : GetPanels())
	{
		Panel->SetHUD(this);
	}
	WarmUpVignette();
	TryCreateMinimap();
	if (!ViewportResizedHandle.IsValid())
	{
		ViewportResizedHandle = FViewport::ViewportResizedEvent.AddUObject(this, &URRHUDWidget::HandleViewportResized);
	}
}

void URRHUDWidget::WarmUpVignette()
{
	if (bVignetteWarmedUp || VignetteWarmOpacity <= 0.f || DamageVignetteWidgetName.IsNone())
	{
		return;
	}
	UWidget* Vignette = FindHUDWidget(DamageVignetteWidgetName);
	if (!Vignette)
	{
		return;
	}
	bVignetteWarmedUp = true;
	DamageVignette = Vignette;
	++VignetteWarmUpCount;
	if (Vignette->GetRenderOpacity() < VignetteWarmOpacity)
	{
		Vignette->SetRenderOpacity(VignetteWarmOpacity);
	}
}

FText URRHUDWidget::GetAlertLine(FName EventId) const
{
	const URRTextContentSubsystem* TextContent = URRTextContentSubsystem::Get(this);
	return TextContent ? TextContent->GetCurrentAlertLine(EventId) : FText::GetEmpty();
}

void URRHUDWidget::ShowHordeAlertLine()
{
	if (URRSubtitleSubsystem* Subtitles = URRSubtitleSubsystem::Get(this))
	{
		Subtitles->ShowAlertLine(FRRTextContentLibrary::AlertHordeIncoming);
	}
	else if (URRTextContentSubsystem* TextContent = URRTextContentSubsystem::Get(this))
	{
		FText Unused;
		TextContent->GetAlertLine(FRRTextContentLibrary::AlertHordeIncoming, Unused);
	}
}

void URRHUDWidget::HandleHordeIncoming(int32 HordeIndex, float SecondsUntil)
{
	ShowHordeAlertLine();
	OnHordeIncoming(SecondsUntil);
}

void URRHUDWidget::HandleMinerRegistered(ARRMiner* Miner)
{
	if (!BoundMiner.IsValid() && IsValid(Miner))
	{
		BindToMiner(Miner);
	}
}

void URRHUDWidget::HandleSpawnerRegistered(ARREnemySpawner* Spawner)
{
	if (!BoundSpawner.IsValid() && IsValid(Spawner))
	{
		BindToSpawner(Spawner);
	}
}

void URRHUDWidget::RestartLevel()
{
	URRRunFlowSubsystem* RunFlow = BoundRunFlow.Get();
	if (!RunFlow)
	{
		const UWorld* World = GetWorld();
		RunFlow = World ? World->GetSubsystem<URRRunFlowSubsystem>() : nullptr;
	}
	if (RunFlow)
	{
		RunFlow->RestartLevel();
	}
}

void URRHUDWidget::HandleShuttleCountdown(float RemainingSeconds, FVector ShuttleLocation)
{
	OnShuttleCountdown(RemainingSeconds, ShuttleLocation);
}

void URRHUDWidget::HandleRunEnded(bool bSuccess)
{
	OnRunEnded(bSuccess);
	// OnRunEnded fires only for an end screen: success = last level, run won (levels 1-3 open the intermission instead).
	ShowEndScreen(bSuccess, false);
}

void URRHUDWidget::ShowEndScreen(bool bRunWon, bool bGameOver)
{
	if (bEndScreenShown)
	{
		return;
	}
	bEndScreenShown = true;
	bEndScreenRunWon = bRunWon;
	bEndScreenGameOver = bGameOver;
	UE_LOG(LogRogueRigs, Log, TEXT("URRHUDWidget: end screen shown (%s), HUD elements hide except the end screen."),
		bGameOver ? TEXT("game over") : (bRunWon ? TEXT("run won") : TEXT("shuttle missed")));
	OnEndScreenShown(bRunWon, bGameOver);
	OnEndScreenShownDelegate.Broadcast(bRunWon, bGameOver);
}

void URRHUDWidget::HandleAlertChanged(float Alert, bool bPaused)
{
	OnAlertChanged(Alert, bPaused);
}

void URRHUDWidget::HandleObjectiveChanged(ARRConsole* Console, ERRConsoleState State, int32 MarkedRocksLeft, int32 MarkedRocksTotal)
{
	OnObjectiveChanged(IsValid(Console), State, MarkedRocksLeft, MarkedRocksTotal);
}

void URRHUDWidget::HandleObjectiveProgressChanged(float Progress, bool bHacking)
{
	OnObjectiveProgressChanged(Progress, bHacking);
}

void URRHUDWidget::HandleExtractionReady()
{
	OnExtractionReady();
}

void URRHUDWidget::NativeDestruct()
{
	UnbindMiner();
	BindToGameMode(nullptr);
	BindToRunFlow(nullptr);
	BindToSpawner(nullptr);
	if (ViewportResizedHandle.IsValid())
	{
		FViewport::ViewportResizedEvent.Remove(ViewportResizedHandle);
		ViewportResizedHandle.Reset();
	}
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearAllTimersForObject(this);
	}
	bMinimapRetryPending = false;
	bMinimapFlushPending = false;
	if (Minimap)
	{
		Minimap->OnPixelsDirtied.Remove(MinimapDirtiedHandle);
		Minimap->Shutdown();
	}
	Super::NativeDestruct();
}

void URRHUDWidget::HandleHealthAttributeChanged(const FOnAttributeChangeData& ChangeData)
{
	BroadcastHealth();

	// Damage: Health went down. A MaxHealth decrease clamps Health to the new maximum (OldValue above it): not a hit.
	const UAbilitySystemComponent* ASC = BoundASC.Get();
	if (!ASC || ChangeData.Attribute != URRHealthAttributeSet::GetHealthAttribute() || ChangeData.NewValue >= ChangeData.OldValue)
	{
		return;
	}
	const float Max = ASC->GetNumericAttribute(URRHealthAttributeSet::GetMaxHealthAttribute());
	if (Max > 0.f && ChangeData.OldValue <= Max)
	{
		OnPlayerDamaged(ChangeData.OldValue - ChangeData.NewValue, FMath::Clamp(ChangeData.NewValue / Max, 0.f, 1.f));
	}
}

void URRHUDWidget::HandleGameOver()
{
	OnGameOver();
}

void URRHUDWidget::HandleRunLost(const FRRRunLostRecap& Recap)
{
	// The missed shuttle already showed its end screen from OnRunEnded (ShowEndScreen runs once).
	if (Recap.bMinerDied)
	{
		ShowEndScreen(false, true);
	}
}

void URRHUDWidget::HandleXPChanged(float Current, float Needed, int32 Level)
{
	// The Blueprint divides by Needed: skip until the curve gives a need.
	if (Needed > 0.f)
	{
		OnXPChanged(Current, Needed, Level);
	}
}

void URRHUDWidget::HandleLevelUp(int32 NewLevel)
{
	OnLevelUp(NewLevel);
}

void URRHUDWidget::HandleUpgradeOfferShown(const TArray<FRRUpgradeCard>& Cards, bool bCanReroll)
{
	OnUpgradeOfferShown(Cards, bCanReroll);
}

void URRHUDWidget::HandleUpgradeOfferClosed()
{
	OnUpgradeOfferClosed();
}

void URRHUDWidget::BroadcastHealth()
{
	const UAbilitySystemComponent* ASC = BoundASC.Get();
	if (!ASC)
	{
		return;
	}
	// The Blueprint divides by Max: before the miner applied its base stats MaxHealth is 0, skip until it is set
	// (the MaxHealth change then calls this again with the real values).
	const float Max = ASC->GetNumericAttribute(URRHealthAttributeSet::GetMaxHealthAttribute());
	if (Max > 0.f)
	{
		OnHealthChanged(ASC->GetNumericAttribute(URRHealthAttributeSet::GetHealthAttribute()), Max);
	}
}

void URRHUDWidget::UnbindMiner()
{
	if (UAbilitySystemComponent* ASC = BoundASC.Get())
	{
		ASC->GetGameplayAttributeValueChangeDelegate(URRHealthAttributeSet::GetHealthAttribute()).Remove(HealthChangedHandle);
		ASC->GetGameplayAttributeValueChangeDelegate(URRHealthAttributeSet::GetMaxHealthAttribute()).Remove(MaxHealthChangedHandle);
	}
	if (URRProgressionComponent* Progression = BoundProgression.Get())
	{
		Progression->OnXPChanged.RemoveDynamic(this, &URRHUDWidget::HandleXPChanged);
		Progression->OnLevelUp.RemoveDynamic(this, &URRHUDWidget::HandleLevelUp);
	}
	if (URRLevelUpComponent* LevelUp = BoundLevelUp.Get())
	{
		LevelUp->OnUpgradeOfferShown.RemoveDynamic(this, &URRHUDWidget::HandleUpgradeOfferShown);
		LevelUp->OnUpgradeOfferClosed.RemoveDynamic(this, &URRHUDWidget::HandleUpgradeOfferClosed);
	}
	if (ARRMiner* Miner = BoundMiner.Get())
	{
		Miner->OnWeaponAdded.RemoveDynamic(this, &URRHUDWidget::HandleWeaponAdded);
	}
	UnbindMinerMovement();
	BindToTurretLauncher(nullptr);
	BoundMiner.Reset();
	BoundLevelUp.Reset();
	BoundProgression.Reset();
	BoundASC.Reset();
	HealthChangedHandle.Reset();
	MaxHealthChangedHandle.Reset();
}
