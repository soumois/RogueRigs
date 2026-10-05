// Rogue Rigs - player settings saved in GameUserSettings.ini (damage number toggle).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "RRGameUserSettings.generated.h"

/**
 * The project's game user settings class (Config/DefaultEngine.ini: [/Script/Engine.Engine] GameUserSettingsClassName).
 * Holds the player options that are not engine ones; saved with SaveSettings into the user's GameUserSettings.ini.
 */
UCLASS(Config = GameUserSettings, ConfigDoNotCheckDefaults)
class ROGUERIGS_API URRGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	/** The engine's game user settings as this class; null when the engine uses another class or has none. */
	UFUNCTION(BlueprintPure, Category = "Settings")
	static URRGameUserSettings* GetRRGameUserSettings();

	/** Shows or hides floating damage numbers (takes effect at once; call SaveSettings to keep it). */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetShowDamageNumbers(bool bShow) { bShowDamageNumbers = bShow; }

	/** True when floating damage numbers are shown. */
	UFUNCTION(BlueprintPure, Category = "Settings")
	bool GetShowDamageNumbers() const { return bShowDamageNumbers; }

	/** Engine defaults plus the project options (damage numbers on). */
	virtual void SetToDefaults() override;

protected:
	/** Player toggle of the floating damage numbers (damage-numbers). */
	UPROPERTY(Config)
	bool bShowDamageNumbers = true;
};
