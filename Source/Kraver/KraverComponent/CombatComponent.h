// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Components/ActorComponent.h"
#include "Kraver/Weapon/Weapon.h"
#include "Kraver/Creature/Creature.h"
#include "CombatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEquipWeaponSuccess, AWeapon*, Weapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerEquipWeaponSuccess, AWeapon*, Weapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnEquipWeaponSuccess, AWeapon*, Weapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerUnEquipWeaponSuccess, AWeapon*, Weapon);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAVER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	AWeapon* GetCurWeapon() { return CurWeapon; }
	bool GetCanEquipWeapon() { return CurWeapon == nullptr; }
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void EquipWeapon(AWeapon* Weapon);
	virtual void UnEquipWeapon(AWeapon* Weapon);

protected:
	UFUNCTION(Server, reliable)
		void Server_EquipWeapon(AWeapon* Weapon);
	UFUNCTION(Server, reliable)
		void Server_UnEquipWeapon(AWeapon* Weapon);

public:
	FEquipWeaponSuccess OnEquipWeaponSuccess;
	FEquipWeaponSuccess OnServerEquipWeaponSuccess;
	FUnEquipWeaponSuccess OnUnEquipWeaponSuccess;
	FServerUnEquipWeaponSuccess OnServerUnEquipWeaponSuccess;

protected:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess = "true"))
		AWeapon* CurWeapon;
	
	ACreature* OwnerCreature;
	
};
