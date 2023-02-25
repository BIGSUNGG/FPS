// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Anim/CreatureAnimInstance.h"
#include "Kraver/Creature/Soldier/Soldier.h"
#include "SoldierAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API USoldierAnimInstance : public UCreatureAnimInstance
{
	GENERATED_BODY()
	
public:
	USoldierAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	ASoldier* Soldier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon, Meta = (AllowPrivateAccess = true))
		bool IsEquippingWeapon; // 캐릭터가 무기를 장착하고 있는지 여부

	UPROPERTY(BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
		FTransform LeftHandTransform; // 왼쪽손에 적용할 트랜스폼
};
