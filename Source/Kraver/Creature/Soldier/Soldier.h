// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Kraver/Creature/Creature.h"
#include "Soldier.generated.h"

/**
 * 
 */

UCLASS()
class KRAVER_API ASoldier : public ACreature
{
	GENERATED_BODY()

public:
	ASoldier();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void EquipWeapon(AWeapon* Weapon); // 매개변수 Weapon을 장착하는 함수

protected:

};
