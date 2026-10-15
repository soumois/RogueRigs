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
		UE_LOG(LogRogueRigs, Log, TEXT("URROreBankSubsystem: %d ore secured, session total %d."), Amount, SecuredOre);
	}
	return SecuredOre;
}
