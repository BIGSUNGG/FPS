// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "Components/ActorComponent.h"
#include "Kraver/Weapon/Weapon.h"
#include "CombatComponent.generated.h"

class ACreature;
class AKraverPlayer;
class AKraverPlayerController;
class AKraverHUD;

// 무기와 전투에 관련된 컴포넌트
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAVER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();
	FKraverDamageResult CalculateDamage(float DamageAmount, FKraverDamageEvent const& DamageEvent); // 받을 데미지 계산

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetHUDCrosshairs(float DeltaTime);

	// Equip UnEquip
	virtual void EquipWeapon(AWeapon* Weapon); // Weapon을 장착하는 함수
	virtual void UnEquipWeapon(AWeapon* Weapon); // Weapon을 장착해제하는 함수

	// Hold Holster
	virtual bool HoldWeapon(int32 WeaponIndex); // WeaponSlot에 있는 무기를 드는 함수
	virtual void HoldWeapon(AWeapon* Weapon); // Weapon을 드는 함수
	virtual bool HolsterWeapon(AWeapon* Weapon); // CurWeapon을 집어넣는 함수

	// Damage Event
	virtual float TakeDamage(float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser); // 데미지를 받는 함수 (서버에서 클라이언트로 TakeDamage이벤트 호출)
	virtual float GiveDamage(AActor* DamagedActor, float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser); // 데미지를 주는 함수 (클라이언트에서 서버로 GiveDamage이벤트 호출)
	UFUNCTION(Client, reliable)
		void Client_GiveDamageSuccess(AActor* DamagedActor, float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);
	virtual void CancelTakeDamage();

protected:
	// Equip Weapon
	UFUNCTION(Server, reliable)
		virtual void Server_EquipWeapon(AWeapon* Weapon); // Weapon을 장착하는 함수
	UFUNCTION(Server, reliable)
		virtual void Server_UnEquipWeapon(AWeapon* Weapon); // Weapon을 장착하는 함수

	UFUNCTION(Client, reliable)
		virtual void Client_EquipWeaponSuccess(AWeapon* Weapon);		
	UFUNCTION(Client, reliable)
		virtual void Client_UnEquipWeaponSuccess(AWeapon* Weapon);

	UFUNCTION(Server, reliable)
		virtual void Server_HoldWeapon(AWeapon* Weapon);
	UFUNCTION(Server, reliable)
		virtual void Server_HolsterWeapon(AWeapon* Weapon); // CurWeapon을 집어넣는 함수


	// Take Damage
	UFUNCTION(Server, reliable)
		void Server_TakeDamage(float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	UFUNCTION(Client, reliable)
		void Client_TakeDamageSuccess(float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);

	// Give Damage
	UFUNCTION(Server, reliable)
		void Server_GiveDamage(AActor* DamagedActor, float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	// Death
	UFUNCTION(Server, reliable)
		void Server_Death(float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult); // Hp가 0이하가 되었을경우 호출
	UFUNCTION(Client, reliable)
		void Client_Death(float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult); // Hp가 0이하가 되었을경우 호출

public:
	// Getter Setter
	void SetIsAttacking(bool bAttack);
	void SetIsSubAttacking(bool bAttack);

	void SetMaxWeaponSlot(int32 size);

	AWeapon* GetCurWeapon() { return CurWeapon; }
	int32 GetCurHp() { return CurHp; }
	int32 GetMaxHp() { return MaxHp; }
	bool GetCanEquipWeapon();
	bool GetIsDead();

public:
	FAttackStartDele OnAttackStartDelegate; // 공격을 시작할때 호출
	FAttackEndDele OnAttackEndDelegate; // 공격을 멈출때 호출
	FAttackStartDele OnSubAttackStartDelegate; // 공격을 시작할때 호출
	FAttackEndDele OnSubAttackEndDelegate; // 공격을 멈출때 호출

	FEquipWeaponSuccessDele OnServerEquipWeaponSuccess; // 무기를 장착했을때 호출
	FEquipWeaponSuccessDele OnClientEquipWeaponSuccess; // 무기를 장착했을때 호출
	FUnEquipWeaponSuccessDele OnServerUnEquipWeaponSuccess; // 무기를 해제했을때 호출
	FUnEquipWeaponSuccessDele OnClientUnEquipWeaponSuccess; // 무기를 해제했을때 호출

	FHoldWeaponDele OnClientHoldWeapon;
	FHoldWeaponDele OnServerHoldWeapon;
	FHolsterWeaponDele OnClientHolsterWeapon;
	FHolsterWeaponDele OnServerHolsterWeapon;

	FTakeDamageDele OnServerBeforeTakeDamage; // 데미지를 받았을때 호출

	FTakeDamageDele OnClientAfterTakeDamageSuccess; // 데미지를 받았을때 호출
	FGiveDamageDele OnClientGiveDamageSuccess; // 데미지를 주었을때 호출
	FDeathDele OnClientDeath; // 죽었을때 호출
	FDeathDele OnServerDeath; // 죽었을때 호출

protected:
	ACreature* OwnerCreature;
	AKraverPlayer* OwnerPlayer;
	AKraverPlayerController* Controller;
	AKraverHUD* HUD;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Data|Weapon", meta = (AllowPrivateAccess = "true"))
		AWeapon* CurWeapon = nullptr; // 현재 무기
	void SetCurWeapon(AWeapon* Weapon);
	UFUNCTION(Server, reliable)
		void Server_SetCurWeapon(AWeapon* Weapon);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Weapon", meta = (AllowPrivateAccess = "true"))
		TArray<AWeapon*> WeaponSlot; // Equip한 무기들을 가지고 있는 배열
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Data|Weapon", meta = (AllowPrivateAccess = "true"))
		int32 MaxWeaponSlotSize = 1; // WeaponSlot 사이즈

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Data|Combat", meta = (AllowPrivateAccess = "true"))
		int32 CurHp = 100.f; // 현재 Hp
	UFUNCTION(Server, reliable)
		void Server_SetCurHp(int32 value);
	UFUNCTION(NetMulticast, reliable)
		void Multicast_SetCurHp(int32 value);

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Data|Combat", meta = (AllowPrivateAccess = "true"))
		int32 MaxHp = 100.f; // 최대 Hp
	UFUNCTION(Server, reliable)
		void Server_SetMaxHp(int32 value);

	bool bCanceledTakeDamage = false; // Server에서만 사용
};
