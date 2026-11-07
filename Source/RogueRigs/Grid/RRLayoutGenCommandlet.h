// Rogue Rigs - command-line entry point of the layout generator, run outside a play session:
//   UnrealEditor-Cmd.exe RogueRigs.uproject -run=RRLayoutGen -seed=<n> [-params=<file>] [-name=<snake_case>] [-out=<dir>] [-brief=<file>]

#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "RRLayoutGenCommandlet.generated.h"

/** Runs FRRLayoutGenerator for a seed and params file and reports what it produced (sys-layout-generator). */
UCLASS()
class URRLayoutGenCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	URRLayoutGenCommandlet();

	/** Parses the arguments, generates, writes and returns 0 on success, 1 on failure, 2 on bad arguments. */
	virtual int32 Main(const FString& Params) override;
};
