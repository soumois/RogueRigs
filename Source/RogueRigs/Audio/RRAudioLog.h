// Rogue Rigs - log category of the audio code (sound events, distance sounds, music): "log LogRRAudio Verbose" shows all of them.
// One extern category, defined in RRAudioSubsystem.cpp: a DEFINE_LOG_CATEGORY_STATIC per file collides in a unity build (task-522, task-524).

#pragma once

#include "CoreMinimal.h"
#include "Logging/LogMacros.h"

ROGUERIGS_API DECLARE_LOG_CATEGORY_EXTERN(LogRRAudio, Log, All);
