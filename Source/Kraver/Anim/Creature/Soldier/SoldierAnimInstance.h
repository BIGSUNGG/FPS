// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Anim/Creature/CreatureAnimInstance.h"
#include "SoldierAnimInstance.generated.h"

/**
 * 
 */

class ASoldier;

UCLASS()
class KRAVER_API USoldierAnimInstance : public UCreatureAnimInstance
{
	GENERATED_BODY()
	
public:
	USoldierAnimInstance();
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	virtual void FabrikLeftHand(USkeletalMeshComponent* HandMesh, USkeletalMeshComponent* WeaponMesh, FTransform& Transform);

protected:
	ASoldier* Soldier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon, Meta = (AllowPrivateAccess = true))
		bool IsEquippingWeapon; // 캐릭터가 무기를 장착하고 있는지 여부

	UPROPERTY(BlueprintReadOnly, Category = Fabrik, meta = (AllowPrivateAccess = "true"))
		FTransform LeftHandFabrikTransform; // 왼쪽손에 적용할 트랜스폼
};
