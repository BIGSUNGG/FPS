// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Kraver/Creature/Creature.h"
#include "Kraver/Weapon/Weapon.h"
#include "Kraver/KraverComponent/CombatComponent.h"
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
	virtual void EquipWeapon(AWeapon* Weapon);

	UFUNCTION()
		virtual void OnEquipWeaponSuccess(AWeapon* Weapon);
	UFUNCTION()
		virtual void OnUnEquipWeaponSuccess(AWeapon* Weapon);
protected:
	UFUNCTION(Server, reliable)
		virtual void Server_OnEquipWeaponSuccess(AWeapon* Weapon);
	UFUNCTION(Server, reliable)
		virtual void Server_OnUnEquipWeaponSuccess(AWeapon* Weapon);

protected:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		UCombatComponent* CombatComponent;
};
