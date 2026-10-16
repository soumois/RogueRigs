// Rogue Rigs - health status HUD panel (future parent of WBP_HealthStatus): drives the health bar and text from the miner health events, with a smooth drain of the shown fill, a damage chunk behind it, a hit flash of the fill and a hit shake of the bar root.

#include "UI/RRHealthStatusWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"
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
	// OnHealthChanged runs first, so the chunk still holds the fraction shown before the hit. Each hit restarts its hold.
	// Hit effects of the later subtasks start here; the drain itself follows HandleHealthChanged.
	ChunkFraction = FMath::Max(ChunkFraction, ShownFraction);
	ChunkDelayRemaining = ChunkDelay;
	FlashAlpha = FlashStrength;
	ShakeTimeRemaining = ShakeDuration;
	RollShakeOffset();
	// Draw the full flash and the first shake offset this frame; the fade starts on the next one.
	ApplyBarVisuals();
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
		ChunkFraction = ShownFraction;
		ChunkDelayRemaining = 0.f;
		FlashAlpha = 0.f;
		ShakeTimeRemaining = 0.f;
		ShakeOffset = FVector2D::ZeroVector;
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
		ChunkFraction = ShownFraction;
		ChunkDelayRemaining = 0.f;
		FlashAlpha = 0.f;
		ShakeTimeRemaining = 0.f;
		ShakeOffset = FVector2D::ZeroVector;
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

	// Damage chunk: holds during its delay, then shrinks to the shown fraction; a heal past it pulls it up.
	if (ChunkDelayRemaining > 0.f)
	{
		ChunkDelayRemaining = FMath::Max(0.f, ChunkDelayRemaining - DeltaSeconds);
	}
	else
	{
		ChunkFraction += (ShownFraction - ChunkFraction) * FMath::Min(1.f, ChunkRate * DeltaSeconds);
		if (FMath::Abs(ShownFraction - ChunkFraction) < DrainSnapThreshold)
		{
			ChunkFraction = ShownFraction;
		}
	}
	ChunkFraction = FMath::Max(ChunkFraction, ShownFraction);

	// Hit flash: linear fade back to the base fill colour.
	FlashAlpha = FMath::Max(0.f, FlashAlpha - FlashDecay * DeltaSeconds);

	// Hit shake: new random offset each frame, amplitude decaying linearly to 0; zero (identity) at the end.
	ShakeTimeRemaining = FMath::Max(0.f, ShakeTimeRemaining - DeltaSeconds);
	RollShakeOffset();
}

void URRHealthStatusWidget::RollShakeOffset()
{
	if (ShakeTimeRemaining <= 0.f || ShakeDuration <= 0.f)
	{
		ShakeTimeRemaining = 0.f;
		ShakeOffset = FVector2D::ZeroVector;
		return;
	}
	const float Amplitude = ShakeAmplitude * (ShakeTimeRemaining / ShakeDuration);
	ShakeOffset = FVector2D(FMath::FRandRange(-Amplitude, Amplitude), FMath::FRandRange(-Amplitude, Amplitude));
}

bool URRHealthStatusWidget::AreBarEffectsSettled() const
{
	return ShownFraction == TargetFraction && ChunkFraction == ShownFraction && ChunkDelayRemaining <= 0.f && FlashAlpha <= 0.f
		&& ShakeTimeRemaining <= 0.f;
}

FLinearColor URRHealthStatusWidget::GetCurrentFillColor() const
{
	return FMath::Lerp(GetBaseFillColor(), FlashColor, FlashAlpha);
}

FWidgetTransform URRHealthStatusWidget::GetBarRootTransform() const
{
	// Single place that composes the bar root transform: later effects (punch scale) set their component here.
	FWidgetTransform Transform;
	Transform.Translation = ShakeOffset;
	return Transform;
}

void URRHealthStatusWidget::ApplyBarVisuals()
{
	if (HealthBar)
	{
		HealthBar->SetPercent(ShownFraction);
		HealthBar->SetFillColorAndOpacity(GetCurrentFillColor());
	}
	if (DamageChunkBar)
	{
		DamageChunkBar->SetPercent(ChunkFraction);
		DamageChunkBar->SetFillColorAndOpacity(ChunkColor);
	}
	if (HealthBarRoot)
	{
		HealthBarRoot->SetRenderTransform(GetBarRootTransform());
	}
}
