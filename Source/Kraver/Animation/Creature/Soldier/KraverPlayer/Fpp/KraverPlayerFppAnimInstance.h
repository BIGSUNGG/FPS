// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include KraverPlayerAnimInstance_h
#include "KraverPlayerFppAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API UKraverPlayerFppAnimInstance : public UKraverPlayerAnimInstance
{
	GENERATED_BODY()
	
public:
	UKraverPlayerFppAnimInstance();

	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void WeaponSway(float DeltaSeconds);

protected:
	void StartProceduralAnim(float Strength);
	void ProceduralAnimEvent();

	UFUNCTION()
	virtual void OnLandEvent(const FHitResult& Result);
	UFUNCTION()
	virtual void OnJumpEvent();
	UFUNCTION()
	virtual void OnStartCrouchEvent(float HalfHeightAdjust, float ScaledHalfHeightAdjust);
	UFUNCTION()
	virtual void OnEndCrouchEvent(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

protected:
	// Animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Weapon|Animation", Meta = (AllowPrivateAccess = true))
	UAnimSequence* AnimWeaponIdleFpp;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Weapon|Animation", Meta = (AllowPrivateAccess = true))
	UAnimSequence* AnimWeaponSprintFpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Weapon|Animation", Meta = (AllowPrivateAccess = true))
	UAnimSequence* AnimWeaponSubAttackFpp;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Weapon|Animation", Meta = (AllowPrivateAccess = true))
	UBlendSpace* AnimWeaponMovementFpp;	

	// Bone Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Weapon|Sway", Meta = (AllowPrivateAccess = true))
	FRotator WeaponSwayResultRot;
	FRotator CurWeaponSwayRot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|ProceduralAnim", Meta = (AllowPrivateAccess = true))
	FVector WeaponSwayResultVec;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|ProceduralAnim", Meta = (AllowPrivateAccess = true))
	FVector ProceduralAnimResultVec;

	// ProceduralAnim
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Curve", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* ProceduralAnimCurve;
	TArray<TTuple<FTimeline, float>> ProceduralAnimStates;

	// Weapon Sway
	float SwayPitchValue = -0.5f;
	float SwayYawValue = 0.25f;
	float MaxSwayYawDegree = 0.2f;
	float MinSwayYawDegree = -0.2f;
	float MaxSwayPitchDegree = 0.5f;
	float MinSwayPitchDegree = -0.5f;
}; 