// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Anim/Creature/Soldier/KraverPlayer/KraverPlayerAnimInstance.h"
#include "KraverPlayerFppAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API UKraverPlayerFppAnimInstance : public UKraverPlayerAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Data|Fabrik", meta = (AllowPrivateAccess = "true"))
		FTransform LeftHandFppFabrikTransform; // 왼쪽손에 적용할 트랜스폼

	// Anim
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Weapon|Anim", Meta = (AllowPrivateAccess = true))
		UAnimSequence* AnimWeaponIdleFpp;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Weapon|Anim", Meta = (AllowPrivateAccess = true))
		UAnimSequence* AnimWeaponSubAttackFpp;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Weapon|Anim", Meta = (AllowPrivateAccess = true))
		UBlendSpace* AnimWeaponMovementFpp;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Weapon|Sway", Meta = (AllowPrivateAccess = true))
		FRotator WeaponSwayResultRot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|ProceduralAnim", Meta = (AllowPrivateAccess = true))
		FVector ProceduralAnimResultVec;
}; 