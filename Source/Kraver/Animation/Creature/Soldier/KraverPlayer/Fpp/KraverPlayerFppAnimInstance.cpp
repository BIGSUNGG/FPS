// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverPlayerFppAnimInstance.h"
#include KraverPlayer_h
#include Gun_h

UKraverPlayerFppAnimInstance::UKraverPlayerFppAnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MONTAGE_FppHolster(TEXT("Engine.AnimMontage'/Game/InfimaGames/AnimatedLowPolyWeapons/Art/Characters/Animations/ARs/AM_FP_PCH_AR_01_Holster.AM_FP_PCH_AR_01_Holster'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MONTAGE_FppUnholster(TEXT("Engine.AnimMontage'/Game/InfimaGames/AnimatedLowPolyWeapons/Art/Characters/Animations/ARs/AM_FP_PCH_AR_01_Unholster.AM_FP_PCH_AR_01_Unholster'"));

	if (MONTAGE_FppHolster.Succeeded())
		HolsterMontage = MONTAGE_FppHolster.Object;
	else
		KR_LOG(Error, TEXT("Failed to find HolsterMontageFpp asset"));

	if (MONTAGE_FppUnholster.Succeeded())
		UnholsterMontage = MONTAGE_FppUnholster.Object;
	else
		KR_LOG(Error, TEXT("Failed to find UnholsterMontageFpp asset"));

}

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

	float TurnValue = KraverPlayer->GetInputAxisValue("Turn") * SwayYawValue;
	float LookUpValue = KraverPlayer->GetInputAxisValue("LookUp") * SwayYawValue;
	FRotator WeaponSwayFinalRot;
	FRotator WeaponSwayInitRot;

	WeaponSwayFinalRot.Roll = LookUpValue * SwayPitchValue;
	WeaponSwayFinalRot.Yaw = TurnValue * SwayYawValue;
	WeaponSwayFinalRot.Pitch = TurnValue * SwayYawValue;

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

	if (CurWeaponSwayRot.Pitch > MaxSwayDegree)
		CurWeaponSwayRot.Pitch = MaxSwayDegree;
	else if (CurWeaponSwayRot.Pitch < MinSwayDegree)
		CurWeaponSwayRot.Pitch = MinSwayDegree;

	if (KraverPlayer->CombatComponent->GetCurWeapon() && KraverPlayer->CombatComponent->GetCurWeapon()->IsSubAttacking())
		WeaponSwayResultRot = CurWeaponSwayRot * 0.25f;
	else
		WeaponSwayResultRot = CurWeaponSwayRot;

	if (AGun* Gun = Cast<AGun>(KraverPlayer->CombatComponent->GetCurWeapon()))
		WeaponSwayResultRot += Gun->GetRecoilSwayRot();
	
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
