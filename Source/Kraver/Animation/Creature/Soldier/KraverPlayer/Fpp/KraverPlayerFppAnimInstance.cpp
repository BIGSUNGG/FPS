// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverPlayerFppAnimInstance.h"
#include KraverPlayer_h

void UKraverPlayerFppAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	KraverPlayer->OnLand.AddDynamic(this, &ThisClass::OnLandEvent);
	KraverPlayer->OnJump.AddDynamic(this, &ThisClass::OnJumpEvent);
	KraverPlayer->OnCrouchStart.AddDynamic(this, &ThisClass::OnStartCrouchEvent);
	KraverPlayer->OnCrouchEnd.AddDynamic(this, &ThisClass::OnEndCrouchEvent);
}

void UKraverPlayerFppAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(KraverPlayer == nullptr) 
		return;

	if (!KraverPlayer->IsLocallyControlled())
		return;

	for (auto& State : ProceduralAnimStates)
	{
		State.Key.TickTimeline(DeltaSeconds);
	}

	USkeletalMeshComponent* Fp_WeaponMesh = dynamic_cast<USkeletalMeshComponent*>(KraverPlayer->CombatComponent->GetCurWeapon() ? KraverPlayer->CombatComponent->GetCurWeapon()->GetFppWeaponPrimitiveInfo()["Root"] : nullptr);

	if (IsEquippingWeapon)
	{
		AnimWeaponIdleFpp = CurWeapon->GetAnimIdleFpp();
		AnimWeaponSprintFpp = CurWeapon->GetAnimSprintFpp();
		AnimWeaponSubAttackFpp = CurWeapon->GetSubAttackFpp();
		AnimWeaponMovementFpp = CurWeapon->GetAnimMovementFpp();
	}
	else
	{
	}

	ProceduralAnimEvent();
	WeaponSway(DeltaSeconds);

}

void UKraverPlayerFppAnimInstance::WeaponSway(float DeltaSeconds)
{
#if TEST_ADS
	return;
#endif

	float TurnValue = KraverPlayer->GetInputAxisValue("Turn") * SwayValue;
	float LookUpValue = KraverPlayer->GetInputAxisValue("LookUp") * SwayValue;
	FRotator WeaponSwayFinalRot;
	FRotator WeaponSwayInitRot;

	WeaponSwayFinalRot.Roll = LookUpValue * SwayValue;
	WeaponSwayFinalRot.Yaw = TurnValue * SwayValue;
	WeaponSwayFinalRot.Pitch = TurnValue * SwayValue;

	FRotator TargetRot;
	TargetRot.Roll = WeaponSwayInitRot.Roll - WeaponSwayFinalRot.Roll;
	TargetRot.Yaw = WeaponSwayInitRot.Yaw + WeaponSwayFinalRot.Yaw;
	TargetRot.Pitch = WeaponSwayInitRot.Pitch + WeaponSwayFinalRot.Pitch;

	CurWeaponSwayRot = FMath::RInterpTo(CurWeaponSwayRot, TargetRot, DeltaSeconds, 4.f);

	if (CurWeaponSwayRot.Roll > MaxSwayDegree)
		CurWeaponSwayRot.Roll = MaxSwayDegree;
	else if (CurWeaponSwayRot.Roll < MinSwayDegree)
		CurWeaponSwayRot.Roll = MinSwayDegree;

	if (CurWeaponSwayRot.Yaw > MaxSwayDegree)
		CurWeaponSwayRot.Yaw = MaxSwayDegree;
	else if (CurWeaponSwayRot.Yaw < MinSwayDegree)
		CurWeaponSwayRot.Yaw = MinSwayDegree;

	if (CurWeaponSwayRot.Roll > MaxSwayDegree)
		CurWeaponSwayRot.Roll = MaxSwayDegree;
	else if (CurWeaponSwayRot.Roll < MinSwayDegree)
		CurWeaponSwayRot.Roll = MinSwayDegree;

	if (CurWeaponSwayRot.Pitch > MaxSwayDegree)
		CurWeaponSwayRot.Pitch = MaxSwayDegree;
	else if (CurWeaponSwayRot.Pitch < MinSwayDegree)
		CurWeaponSwayRot.Pitch = MinSwayDegree;

	if (KraverPlayer->CombatComponent->GetCurWeapon() && KraverPlayer->CombatComponent->GetCurWeapon()->IsSubAttacking())
		WeaponSwayResultRot = CurWeaponSwayRot * 0.1f;
	else
		WeaponSwayResultRot = CurWeaponSwayRot;

}


void UKraverPlayerFppAnimInstance::StartProceduralAnim(float Strength)
{
	for (int i = 0; i < ProceduralAnimStates.Num(); i++)
	{
		float TimelineValue = ProceduralAnimStates[i].Key.GetPlaybackPosition();

		if (TimelineValue >= ProceduralAnimStates[i].Key.GetTimelineLength())
			ProceduralAnimStates.RemoveAt(i);
	}

	int Index = ProceduralAnimStates.Add(TTuple<FTimeline, float>());

	ProceduralAnimStates[Index].Value = Strength;

	FOnTimelineFloat TimelineCallback;

	ProceduralAnimStates[Index].Key.AddInterpFloat(ProceduralAnimCurve, TimelineCallback);
	ProceduralAnimStates[Index].Key.PlayFromStart();

}

void UKraverPlayerFppAnimInstance::ProceduralAnimEvent()
{
	ProceduralAnimResultVec = FVector::ZeroVector;

	for(int i = 0; i < ProceduralAnimStates.Num(); i++)
	{
		float TimelineValue = ProceduralAnimStates[i].Key.GetPlaybackPosition();
		float CurveFloatValue = ProceduralAnimCurve->GetFloatValue(TimelineValue);

		ProceduralAnimResultVec.Z += UKismetMathLibrary::Lerp(0.f, -10.f, CurveFloatValue * ProceduralAnimStates[i].Value);

	}
	if (KraverPlayer->CombatComponent->GetCurWeapon() && KraverPlayer->CombatComponent->GetCurWeapon()->IsSubAttacking())
		ProceduralAnimResultVec.Z *= 0.1f;
}

void UKraverPlayerFppAnimInstance::OnLandEvent(const FHitResult& Result)
{
	StartProceduralAnim(0.35f);
}

void UKraverPlayerFppAnimInstance::OnJumpEvent()
{
	StartProceduralAnim(-0.4f);
}

void UKraverPlayerFppAnimInstance::OnStartCrouchEvent(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	StartProceduralAnim(-0.25f);
}

void UKraverPlayerFppAnimInstance::OnEndCrouchEvent(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	StartProceduralAnim(0.25f);
}
