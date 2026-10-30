// Rogue Rigs - secured ore of the session: a game-instance subsystem, so the total survives a level restart (OpenLevel).

#include "Objectives/RROreBankSubsystem.h"

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "RogueRigs.h"

URROreBankSubsystem* URROreBankSubsystem::Get(const UObject* WorldContext)
{
	const UWorld* World = WorldContext && GEngine ? GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull) : nullptr;
	const UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	return GameInstance ? GameInstance->GetSubsystem<URROreBankSubsystem>() : nullptr;
}

int32 URROreBankSubsystem::AddSecuredOre(int32 Amount)
{
	if (Amount > 0)
	{
		SecuredOre += Amount;
		SecuredSinceReset += Amount;
		UE_LOG(LogRogueRigs, Log, TEXT("URROreBankSubsystem: %d ore secured, session total %d, secured since the run start %d."), Amount, SecuredOre, SecuredSinceReset);
	}
	return SecuredOre;
}

int32 URROreBankSubsystem::RemoveSecuredOre(int32 Amount)
{
	const int32 Removed = FMath::Clamp(Amount, 0, SecuredOre);
	if (Removed > 0)
	{
		SecuredOre -= Removed;
		UE_LOG(LogRogueRigs, Log, TEXT("URROreBankSubsystem: %d secured ore spent, session total %d."), Removed, SecuredOre);
	}
	return SecuredOre;
}

int32 URROreBankSubsystem::ResetSecuredOre()
{
	const int32 Cleared = SecuredOre;
	const int32 ClearedSinceReset = SecuredSinceReset;
	SecuredOre = 0;
	SecuredSinceReset = 0;
	UE_LOG(LogRogueRigs, Log, TEXT("URROreBankSubsystem: run ended, %d secured ore cleared (%d secured during the run), session total 0."), Cleared, ClearedSinceReset);
	return Cleared;
}
