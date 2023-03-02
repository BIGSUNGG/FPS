// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Anim/Creature/Soldier/SoldierAnimInstance.h"
#include "KraverPlayerAnimInstance.generated.h"

/**
 * 
 */

class AKraverPlayer;

UCLASS()
class KRAVER_API UKraverPlayerAnimInstance : public USoldierAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	AKraverPlayer* KraverPlayer;

	UPROPERTY(BlueprintReadOnly, Category = Fabrik, meta = (AllowPrivateAccess = "true"))
		FTransform LeftHandFppFabrikTransform; // 왼쪽손에 적용할 트랜스폼
};
