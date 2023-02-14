// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Components/ActorComponent.h"
#include "Kraver/Weapon/Weapon.h"
#include "Engine/DamageEvents.h"
#include "CombatComponent.generated.h"

class ACreature;
class AKraverPlayer;
class AKraverPlayerController;
class AKraverHUD;

// Attack Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAttackStartDele);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAttackEndDele);

// Equip Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEquipWeaponSuccessDele, AWeapon*, Weapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerEquipWeaponSuccessDele, AWeapon*, Weapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnEquipWeaponSuccessDele, AWeapon*, Weapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerUnEquipWeaponSuccessDele, AWeapon*, Weapon);

// Damaged Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FTakeDamageDele, float, DamageAmount, FDamageEvent const&, DamageEvent, AController*, EventInstigator, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FDeathDele, float, DamageAmount, FDamageEvent const&, DamageEvent, AController*, EventInstigator, AActor*, DamageCauser);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAVER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();
	float CalculateDamage(float DamageAmount);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void Death(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

public:
	AWeapon* GetCurWeapon() { return CurWeapon; }
	bool GetCanEquipWeapon() { return CurWeapon == nullptr; }

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetHUDCrosshairs(float DeltaTime);

	virtual void Reload();
	virtual void EquipWeapon(AWeapon* Weapon);
	virtual void UnEquipWeapon(AWeapon* Weapon);
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	virtual float GiveDamage(AActor* DamagedActor, float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

protected:
	UFUNCTION(Server, reliable)
		void Server_EquipWeapon(AWeapon* Weapon);
	UFUNCTION(Server, reliable)
		void Server_UnEquipWeapon(AWeapon* Weapon);

	UFUNCTION(Server, reliable)
		void Server_TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	UFUNCTION(Client, reliable)
		void Client_TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION(Server, reliable)
		void Server_GivePointDamage(AActor* DamagedActor, float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

public:
	// Getter Setter
	void SetIsAttacking(bool bAttack);

public:
	FAttackStartDele OnAttackStartDelegate;
	FAttackEndDele OnAttackEndDelegate;

	FEquipWeaponSuccessDele OnEquipWeaponSuccess;
	FEquipWeaponSuccessDele OnServerEquipWeaponSuccess;
	FUnEquipWeaponSuccessDele OnUnEquipWeaponSuccess;
	FServerUnEquipWeaponSuccessDele OnServerUnEquipWeaponSuccess;

	FTakeDamageDele OnTakeDamaged;
	FDeathDele OnDeath;

protected:
	ACreature* OwnerCreature;
	AKraverPlayer* OwnerPlayer;
	AKraverPlayerController* Controller;
	AKraverHUD* HUD;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess = "true"))
		AWeapon* CurWeapon = nullptr;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess = "true"))
		int32 CurHp = 100.f;
	UFUNCTION(Server, reliable)
		void Server_CurHp(int32 value);
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess = "true"))
		int32 MaxHp = 100.f;
	UFUNCTION(Server, reliable)
		void Server_MaxHp(int32 value);

};
