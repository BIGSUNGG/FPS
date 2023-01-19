// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Kraver/Anim/CreatureAnimInstance.h"
#include "Kraver/Creature/Soldier.h"
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
	ASoldier* Player;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon, Meta = (AllowPrivateAccess = true))
		bool IsEquippingWeapon;
};
