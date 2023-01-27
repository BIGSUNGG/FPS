// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Kraver/Creature/Creature.h"
#include "Kraver/Weapon/Weapon.h"
#include "Soldier.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EViewType : uint8
{
	FIRST_PERSON   UMETA(DisplayName = "FIRST_PERSON"),
	THIRD_PERSON   UMETA(DisplayName = "THIRD_PERSON"),
};

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
	UFUNCTION(Server, reliable)
		void Server_EquipWeapon(AWeapon* Weapon);

	virtual void EquipWeapon(AWeapon* Weapon);

protected:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess = "true"))
		AWeapon* CurWeapon;
};
