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
	// Anim notify
	UFUNCTION()
		void AnimNotify_RefillAmmo();

protected:
	UPROPERTY(BlueprintReadOnly, Category = Fabrik, meta = (AllowPrivateAccess = "true"))
		FTransform LeftHandFppFabrikTransform; // 왼쪽손에 적용할 트랜스폼

	UPROPERTY(BlueprintReadWrite, Category = Fabrik, meta = (AllowPrivateAccess = "true"))
		FRotator RightHandFppRotation;
};