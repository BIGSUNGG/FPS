// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Animation/Creature/Soldier/KraverPlayer/KraverPlayerAnimInstance.h"
#include "KraverPlayerFppAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API UKraverPlayerFppAnimInstance : public UKraverPlayerAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

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
	UPROPERTY(BlueprintReadOnly, Category = "Data|Fabrik", meta = (AllowPrivateAccess = "true"))
		FTransform LeftHandFppFabrikTransform; // 왼쪽손에 적용할 트랜스폼

	// Animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Weapon|Animation", Meta = (AllowPrivateAccess = true))
		UAnimSequence* AnimWeaponIdleFpp;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Weapon|Animation", Meta = (AllowPrivateAccess = true))
		UAnimSequence* AnimWeaponSubAttackFpp;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Weapon|Animation", Meta = (AllowPrivateAccess = true))
		UBlendSpace* AnimWeaponMovementFpp;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Weapon|Sway", Meta = (AllowPrivateAccess = true))
		FRotator WeaponSwayResultRot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|ProceduralAnim", Meta = (AllowPrivateAccess = true))
		FVector ProceduralAnimResultVec;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Curve", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* ProceduralAnimCurve;
	TArray<TTuple<FTimeline, float>> ProceduralAnimStates;
}; 