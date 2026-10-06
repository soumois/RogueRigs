// Rogue Rigs - polls the URRCheatSettings default object every frame (paused too) and applies the cheats it requests to its world, including the light halo look test.

#include "Debug/RRCheatSubsystem.h"

#include "Debug/RRCheats.h"
#include "Debug/RRCheatSettings.h"
#include "Camera/CameraComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Player/RRMiner.h"
#include "RogueRigs.h"

namespace RRCheatSubsystemPrivate
{
	/** Log label of the property route. */
	const TCHAR* const Source = TEXT("RRCheatSettings");
}

bool URRCheatSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
#if UE_BUILD_SHIPPING
	return false;
#else
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
#endif
}

TStatId URRCheatSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(URRCheatSubsystem, STATGROUP_Tickables);
}

void URRCheatSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	using RRCheatSubsystemPrivate::Source;
	URRCheatSettings* Settings = GetMutableDefault<URRCheatSettings>();
	UWorld* World = GetWorld();

	if (Settings->GrantXP > 0.f)
	{
		const float Amount = Settings->GrantXP;
		Settings->GrantXP = 0.f;
		RRCheats::GrantXP(World, Amount, Source);
	}

	if (Settings->bInvulnerable != bAppliedInvulnerable)
	{
		const int32 Count = RRCheats::SetInvulnerable(World, Settings->bInvulnerable, Source);
		// No miner yet: keep trying until one is tagged.
		if (!Settings->bInvulnerable || Count > 0)
		{
			bAppliedInvulnerable = Settings->bInvulnerable;
		}
	}

	const FAppliedHalo WantedHalo = ReadHaloSettings();
	if (HaloDiffers(WantedHalo, AppliedHalo) || (AppliedHalo.bOn && !HaloLight.IsValid()))
	{
		// No miner yet (or the halo's miner is gone): retried next frame.
		if (ApplyLightHalo(WantedHalo))
		{
			AppliedHalo = WantedHalo;
		}
		else
		{
			AppliedHalo = FAppliedHalo();
		}
	}

	if (Settings->ChooseUpgrade >= 0)
	{
		const int32 Index = Settings->ChooseUpgrade;
		Settings->ChooseUpgrade = -1;
		RRCheats::ChooseUpgrade(World, Index, Source);
	}

	if (Settings->bReroll)
	{
		Settings->bReroll = false;
		RRCheats::Reroll(World, Source);
	}

	if (Settings->bSkipToConsole)
	{
		Settings->bSkipToConsole = false;
		RRCheats::SkipToConsole(World, Source);
	}

	if (Settings->bCompleteConsoleHack)
	{
		Settings->bCompleteConsoleHack = false;
		RRCheats::CompleteConsoleHack(World, Source);
	}

	if (Settings->bSkipToExtraction)
	{
		Settings->bSkipToExtraction = false;
		RRCheats::SkipToExtraction(World, Source);
	}
}

void URRCheatSubsystem::Deinitialize()
{
	RemoveLightHalo();
	Super::Deinitialize();
}

URRCheatSubsystem::FAppliedHalo URRCheatSubsystem::ReadHaloSettings()
{
	const URRCheatSettings* Settings = GetDefault<URRCheatSettings>();
	FAppliedHalo Halo;
	Halo.bOn = Settings->bLightHalo;
	Halo.Intensity = Settings->LightHaloIntensity;
	Halo.Radius = Settings->LightHaloRadius;
	Halo.Height = Settings->LightHaloHeight;
	Halo.Color = Settings->LightHaloColor;
	Halo.bShadows = Settings->bLightHaloShadows;
	Halo.AmbientScale = Settings->LightHaloAmbientScale;
	Halo.ExposureBias = Settings->LightHaloExposureBias;
	return Halo;
}

bool URRCheatSubsystem::HaloDiffers(const FAppliedHalo& A, const FAppliedHalo& B)
{
	if (A.bOn != B.bOn)
	{
		return true;
	}
	// Off: the values do not matter.
	return A.bOn && (A.Intensity != B.Intensity || A.Radius != B.Radius || A.Height != B.Height || A.Color != B.Color
		|| A.bShadows != B.bShadows || A.AmbientScale != B.AmbientScale || A.ExposureBias != B.ExposureBias);
}

void URRCheatSubsystem::RemoveLightHalo()
{
	if (UPointLightComponent* Light = HaloLight.Get())
	{
		Light->DestroyComponent();
	}
	HaloLight.Reset();
	for (const TPair<TWeakObjectPtr<ULightComponentBase>, float>& Entry : ScaledLights)
	{
		if (ULightComponent* Light = Cast<ULightComponent>(Entry.Key.Get()))
		{
			Light->SetIntensity(Entry.Value);
		}
		else if (USkyLightComponent* Sky = Cast<USkyLightComponent>(Entry.Key.Get()))
		{
			Sky->SetIntensity(Entry.Value);
		}
	}
	ScaledLights.Reset();
	if (UCameraComponent* Camera = HaloCamera.Get())
	{
		Camera->PostProcessSettings.bOverride_AutoExposureBias = bCameraHadBiasOverride;
		Camera->PostProcessSettings.AutoExposureBias = CameraOriginalBias;
	}
	HaloCamera.Reset();
}

bool URRCheatSubsystem::ApplyLightHalo(const FAppliedHalo& Wanted)
{
	using RRCheatSubsystemPrivate::Source;
	const bool bWasOn = HaloLight.IsValid() || !ScaledLights.IsEmpty();
	RemoveLightHalo();
	if (!Wanted.bOn)
	{
		if (bWasOn)
		{
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Cheat.LightHalo 0 (%s): halo removed, lights and exposure restored."), Source);
		}
		return true;
	}
	UWorld* World = GetWorld();
	ARRMiner* Miner = RRCheats::FindPlayerMiner(World);
	if (!Miner)
	{
		return false;
	}

	UPointLightComponent* Light = NewObject<UPointLightComponent>(Miner, TEXT("RRCheatLightHalo"), RF_Transient);
	Light->SetMobility(EComponentMobility::Movable);
	Light->SetupAttachment(Miner->GetRootComponent());
	Light->SetRelativeLocation(FVector(0.f, 0.f, Wanted.Height));
	Light->SetIntensityUnits(ELightUnits::Candelas);
	Light->SetIntensity(Wanted.Intensity);
	Light->SetAttenuationRadius(Wanted.Radius);
	Light->SetLightColor(Wanted.Color);
	Light->SetCastShadows(Wanted.bShadows);
	Light->RegisterComponent();
	HaloLight = Light;

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		TInlineComponentArray<ULightComponentBase*> Lights(*It);
		for (ULightComponentBase* Base : Lights)
		{
			if (Base->Mobility == EComponentMobility::Static && (Base->IsA<UDirectionalLightComponent>() || Base->IsA<USkyLightComponent>()))
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Cheat.LightHalo (%s): %s is static, not dimmed."), Source, *Base->GetReadableName());
				continue;
			}
			if (UDirectionalLightComponent* Sun = Cast<UDirectionalLightComponent>(Base))
			{
				ScaledLights.Emplace(Sun, Sun->Intensity);
				Sun->SetIntensity(Sun->Intensity * Wanted.AmbientScale);
			}
			else if (USkyLightComponent* Sky = Cast<USkyLightComponent>(Base))
			{
				ScaledLights.Emplace(Sky, Sky->Intensity);
				Sky->SetIntensity(Sky->Intensity * Wanted.AmbientScale);
			}
		}
	}

	if (UCameraComponent* Camera = Miner->FindComponentByClass<UCameraComponent>())
	{
		HaloCamera = Camera;
		bCameraHadBiasOverride = Camera->PostProcessSettings.bOverride_AutoExposureBias != 0;
		CameraOriginalBias = Camera->PostProcessSettings.AutoExposureBias;
		Camera->PostProcessSettings.bOverride_AutoExposureBias = true;
		Camera->PostProcessSettings.AutoExposureBias = CameraOriginalBias + Wanted.ExposureBias;
	}
	UE_LOG(LogRogueRigs, Log, TEXT("rr.Cheat.LightHalo 1 (%s): halo on %s (%.0f cd, radius %.0f, height %.0f, shadows %d), %d sun/sky lights x%.2f, exposure bias %+.2f%s."),
		Source, *Miner->GetName(), Wanted.Intensity, Wanted.Radius, Wanted.Height, Wanted.bShadows ? 1 : 0, ScaledLights.Num(), Wanted.AmbientScale,
		Wanted.ExposureBias, HaloCamera.IsValid() ? TEXT("") : TEXT(" (no miner camera)"));
	return true;
}
