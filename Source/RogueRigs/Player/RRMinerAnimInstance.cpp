// Rogue Rigs - anim instance of the miner: ground speed, move direction relative to the aim, blend speed and play rate for ABP_Miner.

#include "Player/RRMinerAnimInstance.h"

#include "RogueRigs.h"
#include "Components/MeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/World.h"
#include "Player/RRMiner.h"
#include "Weapons/RRWeapon.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "HAL/IConsoleManager.h"

namespace RRMinerAnimDebug
{
	/** rr.Debug.MinerAnim: 1 logs MoveX, MoveY and GroundSpeed of every miner anim instance at about 2 Hz. */
	static TAutoConsoleVariable<int32> CVarMinerAnim(
		TEXT("rr.Debug.MinerAnim"), 0,
		TEXT("1 logs the miner anim instance MoveX, MoveY and GroundSpeed to LogRogueRigs at about 2 Hz; 0 off."),
		ECVF_Cheat);

	/** Seconds between two debug log lines (about 2 Hz). Debug-only, not a gameplay tunable. */
	static constexpr double LogIntervalSeconds = 0.5;

	/** rr.Debug.MinerAnimForce "x y": forces MoveX=x, MoveY=y (length clamped to 1) and GroundSpeed = length * MaxWalkSpeed; empty = off. */
	static TAutoConsoleVariable<FString> CVarMinerAnimForce(
		TEXT("rr.Debug.MinerAnimForce"), TEXT(""),
		TEXT("\"x y\" (two floats) forces the miner anim instance MoveX=x, MoveY=y (length clamped to 1) and GroundSpeed = length * MaxWalkSpeed; empty = off."),
		ECVF_Cheat);

	/** Parses the force cvar; true with the clamped move vector when it holds two floats. */
	static bool TryGetForcedMove(FVector2D& OutMove)
	{
		const FString Value = CVarMinerAnimForce.GetValueOnGameThread();
		TArray<FString> Tokens;
		Value.ParseIntoArrayWS(Tokens);
		float X = 0.f;
		float Y = 0.f;
		if (Tokens.Num() != 2 || !LexTryParseString(X, *Tokens[0]) || !LexTryParseString(Y, *Tokens[1]))
		{
			return false;
		}
		OutMove = FVector2D(X, Y);
		if (OutMove.SizeSquared() > 1.0)
		{
			OutMove.Normalize();
		}
		return true;
	}

	/** rr.Debug.MinerArmFix "<elbowUp> <handForward>": overrides the right arm offsets with (0, 0, elbowUp) and (0, handForward, 0) cm; empty = off. */
	static TAutoConsoleVariable<FString> CVarMinerArmFix(
		TEXT("rr.Debug.MinerArmFix"), TEXT(""),
		TEXT("\"<elbowUp> <handForward>\" (two floats, cm) overrides the miner right arm offsets: RightElbowOffset = (0, 0, elbowUp), RightHandOffset = (0, handForward, 0) (component axes, +Y forward, +Z up); empty = off (the ABP_Miner class default offsets apply)."),
		ECVF_Cheat);

	/** rr.Debug.MinerAnimFreeze: 1 stops the miner animation time (mesh GlobalAnimRateScale 0) while the graph keeps evaluating; 0 restores. */
	static TAutoConsoleVariable<int32> CVarMinerAnimFreeze(
		TEXT("rr.Debug.MinerAnimFreeze"), 0,
		TEXT("1 freezes the miner animation time (mesh GlobalAnimRateScale 0; the anim graph keeps evaluating, so a new rr.Debug.MinerArmFix value shows on the same frame); 0 restores the previous rate scale."),
		ECVF_Cheat);

	/** Parses the arm fix cvar; true with the elbow up and hand forward amounts (cm) when it holds two floats. */
	static bool TryGetArmFix(float& OutElbowUp, float& OutHandForward)
	{
		const FString Value = CVarMinerArmFix.GetValueOnGameThread();
		TArray<FString> Tokens;
		Value.ParseIntoArrayWS(Tokens);
		return Tokens.Num() == 2 && LexTryParseString(OutElbowUp, *Tokens[0]) && LexTryParseString(OutHandForward, *Tokens[1]);
	}
}

void URRMinerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	const APawn* PawnOwner = TryGetPawnOwner();
	const FVector Velocity = PawnOwner ? PawnOwner->GetVelocity() : FVector::ZeroVector;
	const FVector GroundVelocity(Velocity.X, Velocity.Y, 0.0);

	GroundSpeed = static_cast<float>(GroundVelocity.Size());

	if (PawnOwner && !FMath::IsNearlyZero(GroundSpeed))
	{
		// Velocity in the actor yaw frame: X forward (the aim), Y right. Same math as UKismetAnimationLibrary::CalculateDirection.
		const FRotator YawRotation(0.0, PawnOwner->GetActorRotation().Yaw, 0.0);
		const FVector LocalVelocity = YawRotation.UnrotateVector(GroundVelocity);
		Direction = static_cast<float>(FMath::RadiansToDegrees(FMath::Atan2(LocalVelocity.Y, LocalVelocity.X)));
	}
	else
	{
		Direction = 0.f;
	}

	RunSpeedForDirection = ComputeRunSpeedForDirection(Direction);
	// task-386-16: the simple blend space plays the clips at their authored rate, the Q4 clamp rule is off for now.
	LocomotionPlayRate = 1.f;
	BlendSpeed = GroundSpeed / LocomotionPlayRate;

	// Move vector (task-386-16): ground velocity in the actor yaw frame (the aim) over MaxWalkSpeed, clamped to length 1.
	const ACharacter* CharacterOwner = Cast<ACharacter>(PawnOwner);
	const UCharacterMovementComponent* MovementComponent = CharacterOwner ? CharacterOwner->GetCharacterMovement() : nullptr;
	const float MaxWalkSpeed = MovementComponent ? MovementComponent->MaxWalkSpeed : 0.f;
	if (PawnOwner && MaxWalkSpeed > UE_KINDA_SMALL_NUMBER)
	{
		const FRotator YawRotation(0.0, PawnOwner->GetActorRotation().Yaw, 0.0);
		const FVector Forward = YawRotation.Vector();
		const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		FVector2D Move(
			FVector::DotProduct(GroundVelocity, Forward) / MaxWalkSpeed,
			FVector::DotProduct(GroundVelocity, Right) / MaxWalkSpeed);
		if (Move.SizeSquared() > 1.0)
		{
			Move.Normalize();
		}
		MoveX = static_cast<float>(Move.X);
		MoveY = static_cast<float>(Move.Y);
	}
	else
	{
		MoveX = 0.f;
		MoveY = 0.f;
	}

	// task-386-21: debug override of the move vector (rr.Debug.MinerAnimForce), so each blend space sample can be captured.
	FVector2D ForcedMove;
	if (RRMinerAnimDebug::TryGetForcedMove(ForcedMove))
	{
		MoveX = static_cast<float>(ForcedMove.X);
		MoveY = static_cast<float>(ForcedMove.Y);
		GroundSpeed = static_cast<float>(ForcedMove.Size()) * MaxWalkSpeed;
	}

	RefreshRightArmOffsets();
	ApplyDebugAnimFreeze();

	LogDebugValues();
}

void URRMinerAnimInstance::RefreshRightArmOffsets()
{
	// task-407-01: the debug override replaces the tuned values while set, so offset series can be captured on one frame.
	float ElbowUp = 0.f;
	float HandForward = 0.f;
	if (RRMinerAnimDebug::TryGetArmFix(ElbowUp, HandForward))
	{
		AppliedRightElbowOffset = FVector(0.0, 0.0, ElbowUp);
		AppliedRightHandOffset = FVector(0.0, HandForward, 0.0);
	}
	else
	{
		AppliedRightElbowOffset = RightElbowOffset;
		AppliedRightHandOffset = RightHandOffset;
	}
}

void URRMinerAnimInstance::ApplyDebugAnimFreeze()
{
	USkeletalMeshComponent* MinerMesh = GetSkelMeshComponent();
	if (!MinerMesh)
	{
		return;
	}

	// The scale applies from the next tick (this update already received its delta time); update and evaluation keep
	// running with a zero delta, so pose changes from offsets still show.
	const bool bWantFrozen = RRMinerAnimDebug::CVarMinerAnimFreeze.GetValueOnGameThread() != 0;
	if (bWantFrozen && !bDebugAnimFrozen)
	{
		SavedGlobalAnimRateScale = MinerMesh->GlobalAnimRateScale;
		MinerMesh->GlobalAnimRateScale = 0.f;
		bDebugAnimFrozen = true;
	}
	else if (!bWantFrozen && bDebugAnimFrozen)
	{
		MinerMesh->GlobalAnimRateScale = SavedGlobalAnimRateScale;
		bDebugAnimFrozen = false;
	}
}

const FName URRMinerAnimInstance::LeftHandGripSocketName(TEXT("LeftHandGrip"));

void URRMinerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	ARRMiner* Miner = Cast<ARRMiner>(TryGetPawnOwner());
	if (!Miner)
	{
		return;
	}
	BoundMiner = Miner;
	Miner->OnWeaponAdded.AddUniqueDynamic(this, &URRMinerAnimInstance::HandleWeaponAdded);

	// Weapons acquired before this anim instance started (the delegate would not fire for them).
	for (const ARRWeapon* Weapon : Miner->GetWeapons())
	{
		RefreshLeftHandIK(Weapon);
	}
}

void URRMinerAnimInstance::NativeUninitializeAnimation()
{
	if (ARRMiner* Miner = BoundMiner.Get())
	{
		Miner->OnWeaponAdded.RemoveDynamic(this, &URRMinerAnimInstance::HandleWeaponAdded);
	}
	BoundMiner.Reset();
	Super::NativeUninitializeAnimation();
}

void URRMinerAnimInstance::HandleWeaponAdded(ARRWeapon* Weapon)
{
	RefreshLeftHandIK(Weapon);
}

void URRMinerAnimInstance::RefreshLeftHandIK(const ARRWeapon* Weapon)
{
	// Only a hand-held weapon (attached to its hand socket on the miner mesh) drives the left hand.
	const USkeletalMeshComponent* MinerMesh = GetSkelMeshComponent();
	const FName HandSocketName = Weapon ? Weapon->GetHandSocketName() : NAME_None;
	if (!Weapon || !MinerMesh || HandSocketName.IsNone()
		|| Weapon->GetAttachParentActor() != MinerMesh->GetOwner()
		|| Weapon->GetAttachParentSocketName() != HandSocketName)
	{
		return;
	}

	// Result for this hand-held weapon: off until a mesh with LeftHandGrip is found.
	LeftHandIKAlpha = 0.f;
	LeftHandIKTarget = FVector::ZeroVector;

	const USkeletalMeshSocket* HandSocket = MinerMesh->GetSocketByName(HandSocketName);
	if (!HandSocket)
	{
		return;
	}

	TArray<UMeshComponent*> MeshComponents;
	Weapon->GetComponents<UMeshComponent>(MeshComponents);
	for (const UMeshComponent* WeaponMesh : MeshComponents)
	{
		if (!WeaponMesh->DoesSocketExist(LeftHandGripSocketName))
		{
			continue;
		}
		// The weapon is rigid on WeaponSocket_R, so the grip in the weapon actor space moved by the socket local transform
		// (bone -> socket) gives the grip in the bone space of the right hand, whatever the current pose.
		const FTransform GripWorld = WeaponMesh->GetSocketTransform(LeftHandGripSocketName, RTS_World);
		const FVector GripInActor = GripWorld.GetRelativeTransform(Weapon->GetActorTransform()).GetLocation();
		LeftHandIKTarget = HandSocket->GetSocketLocalTransform().TransformPosition(GripInActor);
		LeftHandIKAlpha = 1.f;
		return;
	}
}

void URRMinerAnimInstance::LogDebugValues()
{
	if (RRMinerAnimDebug::CVarMinerAnim.GetValueOnGameThread() == 0)
	{
		return;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const double Now = World->GetTimeSeconds();
	if (LastDebugLogTime >= 0.0 && Now - LastDebugLogTime < RRMinerAnimDebug::LogIntervalSeconds)
	{
		return;
	}
	LastDebugLogTime = Now;

	UE_LOG(LogRogueRigs, Log, TEXT("MinerAnim %s: MoveX=%.3f MoveY=%.3f GroundSpeed=%.1f"),
		*GetNameSafe(TryGetPawnOwner()), MoveX, MoveY, GroundSpeed);
}

float URRMinerAnimInstance::ComputeRunSpeedForDirection(float InDirection) const
{
	const float AbsDirection = FMath::Abs(InDirection);
	const float SideSpeed = InDirection >= 0.f ? RunSpeedRight : RunSpeedLeft;

	float Speed;
	if (AbsDirection <= 90.f)
	{
		// Forward to side.
		Speed = FMath::Lerp(RunSpeedForward, SideSpeed, AbsDirection / 90.f);
	}
	else
	{
		// Side to backward.
		Speed = FMath::Lerp(SideSpeed, RunSpeedBackward, FMath::Min(AbsDirection - 90.f, 90.f) / 90.f);
	}

	// Guard the division in NativeUpdateAnimation against a zero speed set by hand.
	return FMath::Max(Speed, 1.f);
}
