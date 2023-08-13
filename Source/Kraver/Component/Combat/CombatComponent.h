// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "Components/ActorComponent.h"
#include "Kraver/Actor/Weapon/Weapon.h"
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
	FKraverDamageResult CalculateDamage(float DamageAmount, FDamageEvent const& DamageEvent); // 받을 데미지 계산

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetHUDCrosshairs(float DeltaTime);

	// Equip UnEquip
	UFUNCTION(BlueprintCallable)
		virtual void EquipWeapon(AWeapon* Weapon); // Weapon을 장착하는 함수
	UFUNCTION(BlueprintCallable)
		virtual void UnEquipWeapon(AWeapon* Weapon); // Weapon을 장착해제하는 함수

	// Holster Unholster
	virtual bool UnholsterWeapon(int32 WeaponIndex); // WeaponSlot에 있는 무기를 드는 함수
	virtual void UnholsterWeapon(AWeapon* Weapon); // Weapon을 드는 함수
	virtual bool HolsterWeapon(AWeapon* Weapon); // CurWeapon을 집어넣는 함수

	// Damage Event
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser); // 데미지를 받는 함수 (서버에서 클라이언트로 TakeDamage이벤트 호출)
	virtual float GiveDamage(AActor* DamagedActor, float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser); // 데미지를 주는 함수 (클라이언트에서 서버로 GiveDamage이벤트 호출)

	virtual void CancelTakeDamage();

protected:
	// Equip Weapon
	UFUNCTION(Server, Reliable)
		virtual void Server_EquipWeapon(AWeapon* Weapon); // Weapon을 장착하는 함수
	UFUNCTION(Server, Reliable)
		virtual void Server_UnEquipWeapon(AWeapon* Weapon); // Weapon을 장착하는 함수

	UFUNCTION(Client, Reliable)
		virtual void Client_EquipWeaponSuccess(AWeapon* Weapon);		
	UFUNCTION(Client, Reliable)
		virtual void Client_UnEquipWeaponSuccess(AWeapon* Weapon);

	UFUNCTION(Server, Reliable)
		virtual void Server_UnholsterWeapon(AWeapon* Weapon);
	UFUNCTION(Server, Reliable)
		virtual void Server_HolsterWeapon(AWeapon* Weapon); // CurWeapon을 집어넣는 함수


	// Take Damage
	UFUNCTION(Server, Reliable)
		void Server_TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	UFUNCTION(Server, Reliable)
		void Server_TakePointDamage(float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	UFUNCTION(Server, Reliable)
		void Server_TakeRadialDamage(float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION(Client, Reliable)
		void Client_TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);
	UFUNCTION(Client, Reliable)
		void Client_TakePointDamage(float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);
	UFUNCTION(Client, Reliable)
		void Client_TakeRadialDamage(float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);

	// Give Damage
	UFUNCTION(Server, Reliable)
		void Server_GiveDamage(AActor* DamagedActor, float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	UFUNCTION(Server, Reliable)
		void Server_GivePointDamage(AActor* DamagedActor, float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	UFUNCTION(Server, Reliable)
		void Server_GiveRadialDamage(AActor* DamagedActor, float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION(Client, Reliable)
		void Client_GiveDamageSuccess(AActor* DamagedActor, float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);
	UFUNCTION(Client, Reliable)
		void Client_GivePointDamageSuccess(AActor* DamagedActor, float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);
	UFUNCTION(Client, Reliable)
		void Client_GiveRadialDamageSuccess(AActor* DamagedActor, float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);

	// Death
	UFUNCTION(Server, Reliable)
		void Server_Death(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult); // Hp가 0이하가 되었을경우 호출
	UFUNCTION(Client, Reliable)
		void Client_Death(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult); // Hp가 0이하가 되었을경우 호출

	// OnRep
	UFUNCTION()
		void OnRepCurWeaponEvent(AWeapon* PrevWeapon);
	UFUNCTION()
		void OnRepWeaponSlotEvent(TArray<AWeapon*> PrevWeaponSlot);

	// Func
	bool AddWeapon(AWeapon* Weapon);
	bool RemoveWeapon(AWeapon* Weapon);

public:
	// Getter Setter
	void SetIsAttacking(bool bAttack);
	void SetIsSubAttacking(bool bAttack);

	AWeapon* GetCurWeapon() { return CurWeapon; }
	int32 GetCurHp() { return CurHp; }
	int32 GetMaxHp() { return MaxHp; }
	int8 GetCurWeaponSlotIndex();
	bool GetCanEquipWeapon();
	bool GetIsDead();
	const TArray<AWeapon*>& GetWeaponSlot() { return WeaponSlot; }

	int CountWeapon();

public:
	// Attack
	FAttackStartDele OnAttackStartDelegate; // 공격을 시작할때 호출
	FAttackEndDele OnAttackEndDelegate; // 공격을 멈출때 호출
	FAttackStartDele OnSubAttackStartDelegate; // 공격을 시작할때 호출
	FAttackEndDele OnSubAttackEndDelegate; // 공격을 멈출때 호출

	// Equip
	FEquipWeaponSuccessDele OnServerEquipWeaponSuccess; // 무기를 장착했을때 호출
	FEquipWeaponSuccessDele OnClientEquipWeaponSuccess; // 무기를 장착했을때 호출
	FUnEquipWeaponSuccessDele OnServerUnEquipWeaponSuccess; // 무기를 해제했을때 호출
	FUnEquipWeaponSuccessDele OnClientUnEquipWeaponSuccess; // 무기를 해제했을때 호출

	// Holster Holster
	FHolsterWeaponDele OnClientUnholsterWeapon;
	FHolsterWeaponDele OnServerUnholsterWeapon;
	FHolsterWeaponDele OnClientHolsterWeapon;
	FHolsterWeaponDele OnServerHolsterWeapon;

	// Take Damage
	FTakeDamageDele OnServerBeforeTakeDamage; // 데미지를 받았을때 호출

	FTakeDamageDele OnClientAfterTakeAnyDamageSuccess; // 데미지를 받았을때 호출
	FTakeDamageDele OnClientAfterTakeDamageSuccess; // 데미지를 받았을때 호출
	FTakePointDamageDele OnClientAfterTakePointDamageSuccess; // 데미지를 받았을때 호출
	FTakeRadialDamageDele OnClientAfterTakeRadialDamageSuccess; // 데미지를 받았을때 호출

	FGiveDamageDele OnClientGiveAnyDamageSuccess; // 데미지를 주었을때 호출
	FGiveDamageDele OnClientGiveDamageSuccess; // 데미지를 주었을때 호출
	FGivePointDamageDele OnClientGivePointDamageSuccess; // 데미지를 주었을때 호출
	FGiveRadialDamageDele OnClientGiveRadialDamageSuccess; // 데미지를 주었을때 호출

	// Death
	FDeathDele OnClientDeath; // 죽었을때 호출
	FDeathDele OnServerDeath; // 죽었을때 호출

	// OnRep
	FRepWeapon OnRepCurWeapon;
	FRepWeaponSlot OnRepWeaponSlot;

protected:
	ACreature* OwnerCreature;
	AKraverPlayer* OwnerPlayer;
	AKraverPlayerController* Controller;
	AKraverHUD* HUD;

	UPROPERTY(ReplicatedUsing = OnRepCurWeaponEvent, EditAnywhere, BlueprintReadWrite, Category = "Data|Weapon", meta = (AllowPrivateAccess = "true"))
		AWeapon* CurWeapon = nullptr; // 현재 무기
	void SetCurWeapon(AWeapon* Weapon);
	UFUNCTION(Server, Reliable)
		void Server_SetCurWeapon(AWeapon* Weapon);
	UPROPERTY(ReplicatedUsing = OnRepWeaponSlotEvent, EditAnywhere, BlueprintReadWrite, Category = "Data|Weapon", meta = (AllowPrivateAccess = "true"))
		TArray<AWeapon*> WeaponSlot; // Equip한 무기들을 가지고 있는 배열
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Weapon", meta = (AllowPrivateAccess = "true"))
		int32 MaxWeaponSlotSize = 3; // WeaponSlot 사이즈

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Data|Combat", meta = (AllowPrivateAccess = "true"))
		int32 CurHp = 100.f; // 현재 Hp
	UFUNCTION(Server, Reliable)
		void Server_SetCurHp(int32 value);
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_SetCurHp(int32 value);

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Data|Combat", meta = (AllowPrivateAccess = "true"))
		int32 MaxHp = 100.f; // 최대 Hp
	UFUNCTION(Server, Reliable)
		void Server_SetMaxHp(int32 value);

	bool bCanceledTakeDamage = false; // Server에서만 사용

};
