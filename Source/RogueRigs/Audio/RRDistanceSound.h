// Rogue Rigs - distance sounds (task-522): every sound heard from a point of the world and faded by the player's distance goes
// through FRRDistanceSound (one-shot or loop); FRRExplosionSound measures from the edge of the blast instead of its centre.

#pragma once

#include "CoreMinimal.h"
#include "UObject/WeakObjectPtrTemplates.h"

class UAudioComponent;
class UObject;
struct FRRSoundEventRow;

/**
 * A sound event played from a point of the world, faded by the horizontal distance from the player pawn: the row's Volume up to its
 * FadeInner, linear down to silence at its FadeOuter (row values in Config/DefaultGame.ini, [/Script/RogueRigs.RRAudioSettings]).
 * - One-shot: PlayAt (Impact.Wall, EnemyDeath, ...). Not played at all at or beyond FadeOuter.
 * - Loop (a bLoop row): UpdateLoop every tick while the source exists. The play starts (faded in over LoopFadeIn) once the player is
 *   inside FadeOuter, follows the source, its level follows the distance on every call, and it fades out over LoopFadeOut when the
 *   caller releases it or the player leaves FadeOuter. A paused game stops it at once; the next call restarts it.
 * A row without FadeOuter plays at its Volume (placed when bSpatial). Child classes change how the distance is measured (GetDistance).
 * Plain C++ object: a one-shot is built on the stack; a loop is a member of its source actor (it holds its play).
 */
class ROGUERIGS_API FRRDistanceSound
{
public:
	/** Event of the table; Variant refines it (Event.Variant row first, else Event). */
	explicit FRRDistanceSound(FName InEvent, FName InVariant = NAME_None);
	virtual ~FRRDistanceSound() = default;

	/** One play at Location. Returns the component played, or null (no row, no sound, beyond FadeOuter). */
	UAudioComponent* PlayAt(const UObject* WorldContextObject, const FVector& Location) const;

	/** Loop rows: call every tick with bOn = the source is sounding and Location = where it is now. */
	void UpdateLoop(const UObject* WorldContextObject, bool bOn, const FVector& Location);

	/** Releases the loop (fade out over the row's LoopFadeOut); same as UpdateLoop with bOn false. */
	void StopLoop(const UObject* WorldContextObject);

	/** True while the loop play is active. */
	bool IsLoopPlaying() const;

	/**
	 * Loop rows: true = the play goes on while the game is paused (played as a UI sound, never stopped by the pause); the holder
	 * releases it itself. Changing it while the loop plays restarts the play (task-522: the extractor takeoff behind the end screen).
	 */
	void SetPlaysWhilePaused(bool bInPlaysWhilePaused) { bPlaysWhilePaused = bInPlaysWhilePaused; }
	bool PlaysWhilePaused() const { return bPlaysWhilePaused; }

	/** Distance factor of the loop at its last update (0 to 1), -1 before the first one or without a player. */
	float GetLoopFade() const { return LoopFade; }

	/** Fade distance of a play at Location: horizontal distance player to Location. */
	virtual float GetDistance(const FVector& PlayerLocation, const FVector& Location) const;

	/** Radius of the circle the distance is measured from (logs); 0 for a point. */
	virtual float GetEdgeRadius() const { return 0.f; }

	/** Event name played (refined by the variant). */
	FName GetEvent() const { return Event; }

	/**
	 * Player distance (GetDistance) and volume factor of a play of Row at Location now. False when the row has no fade (FadeOuter 0)
	 * and no near reduction (NearOuter 0), or there is no player pawn: factor 1, distance -1.
	 */
	bool Measure(const UObject* WorldContextObject, const FRRSoundEventRow& Row, const FVector& Location, float& OutDistance, float& OutFade) const;

	/** Volume factor of Row at Distance uu: GetFarFade times GetNearFactor. */
	static float GetFade(const FRRSoundEventRow& Row, float Distance);

	/** Distance fade: 1 up to FadeInner, linear to 0 at FadeOuter, 0 beyond; 1 when FadeOuter is 0. */
	static float GetFarFade(const FRRSoundEventRow& Row, float Distance);

	/** Near reduction (task-523): NearVolume up to NearInner, linear to 1 at NearOuter, 1 beyond; 1 when NearOuter is 0. */
	static float GetNearFactor(const FRRSoundEventRow& Row, float Distance);

protected:
	FName Event;

private:
	void ReleaseLoop(const UObject* WorldContextObject, const TCHAR* Reason);

	TWeakObjectPtr<UAudioComponent> LoopComponent;
	bool bPlaysWhilePaused = false;
	float LoopFade = -1.f;
	/** Tenth of the factor last logged (Verbose line each time the loop level crosses a 0.1 step). */
	int32 LoggedStep = -1;
};

/**
 * Explosion sound (task-516): the distance is measured from the edge of the blast, distance to the centre minus Radius, 0 inside, so a
 * player caught in the blast or at its edge hears it at full level.
 */
class ROGUERIGS_API FRRExplosionSound : public FRRDistanceSound
{
public:
	FRRExplosionSound(FName InEvent, float InRadius, FName InVariant = NAME_None);

	virtual float GetDistance(const FVector& PlayerLocation, const FVector& Location) const override;
	virtual float GetEdgeRadius() const override { return Radius; }

private:
	float Radius = 0.f;
};
