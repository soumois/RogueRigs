// Rogue Rigs - health status HUD panel (future parent of WBP_HealthStatus): drives the health bar and text from the miner health events, with a smooth drain of the shown fill.

#include "UI/RRHealthStatusWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Engine/World.h"
#include "Player/RRMiner.h"

void URRHealthStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// The Blueprint Destruct graph unbinds every listener of the miner: bind again when the widget comes back.
	if (ARRMiner* Miner = GetMiner())
	{
		Miner->OnHealthChanged.AddUniqueDynamic(this, &URRHealthStatusWidget::HandleHealthChanged);
		Miner->OnDamaged.AddUniqueDynamic(this, &URRHealthStatusWidget::HandleDamaged);
	}
	ApplyBarVisuals();
}

void URRHealthStatusWidget::NativeDestruct()
{
	StopBarUpdate();
	Super::NativeDestruct();
}

void URRHealthStatusWidget::NativeBindToMiner(ARRMiner* Miner, ARRMiner* PreviousMiner)
{
	if (PreviousMiner)
	{
		PreviousMiner->OnHealthChanged.RemoveDynamic(this, &URRHealthStatusWidget::HandleHealthChanged);
		PreviousMiner->OnDamaged.RemoveDynamic(this, &URRHealthStatusWidget::HandleDamaged);
	}
	if (Miner)
	{
		Miner->OnHealthChanged.AddUniqueDynamic(this, &URRHealthStatusWidget::HandleHealthChanged);
		Miner->OnDamaged.AddUniqueDynamic(this, &URRHealthStatusWidget::HandleDamaged);
		ShowHealth(Miner->GetCurrentHealth(), Miner->GetMaxHealth(), true);
	}
	else
	{
		ShowHealth(0.f, 0.f, true);
	}
}

void URRHealthStatusWidget::HandleHealthChanged(float Current, float Max)
{
	ShowHealth(Current, Max, false);
}

void URRHealthStatusWidget::HandleDamaged(float Amount, float HealthFraction)
{
	// Hit effects of the later subtasks start here; the drain itself follows HandleHealthChanged.
	StartBarUpdate();
}

void URRHealthStatusWidget::ShowHealth(float Current, float Max, bool bSnap)
{
	if (HealthText)
	{
		HealthText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), FMath::RoundToInt(Current), FMath::RoundToInt(Max))));
	}
	TargetFraction = Max > 0.f ? FMath::Clamp(Current / Max, 0.f, 1.f) : 0.f;
	if (bSnap)
	{
		ShownFraction = TargetFraction;
		ApplyBarVisuals();
	}
	else if (!FMath::IsNearlyEqual(ShownFraction, TargetFraction))
	{
		StartBarUpdate();
	}
}

void URRHealthStatusWidget::StartBarUpdate()
{
	if (bBarUpdateRunning)
	{
		return;
	}
	UWorld* World = GetWorld();
	if (!World)
	{
		// No world to tick in: show the end state at once.
		ShownFraction = TargetFraction;
		ApplyBarVisuals();
		return;
	}
	bBarUpdateRunning = true;
	BarUpdateTimer = World->GetTimerManager().SetTimerForNextTick(this, &URRHealthStatusWidget::UpdateBarEffects);
}

void URRHealthStatusWidget::StopBarUpdate()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(BarUpdateTimer);
	}
	BarUpdateTimer.Invalidate();
	bBarUpdateRunning = false;
}

void URRHealthStatusWidget::UpdateBarEffects()
{
	bBarUpdateRunning = false;
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	AdvanceBarEffects(World->GetDeltaSeconds());
	ApplyBarVisuals();
	if (!AreBarEffectsSettled())
	{
		StartBarUpdate();
	}
}

void URRHealthStatusWidget::AdvanceBarEffects(float DeltaSeconds)
{
	ShownFraction += (TargetFraction - ShownFraction) * FMath::Min(1.f, DrainRate * DeltaSeconds);
	if (FMath::Abs(TargetFraction - ShownFraction) < DrainSnapThreshold)
	{
		ShownFraction = TargetFraction;
	}
}

bool URRHealthStatusWidget::AreBarEffectsSettled() const
{
	return ShownFraction == TargetFraction;
}

void URRHealthStatusWidget::ApplyBarVisuals()
{
	if (HealthBar)
	{
		HealthBar->SetPercent(ShownFraction);
		HealthBar->SetFillColorAndOpacity(GetBaseFillColor());
	}
}
