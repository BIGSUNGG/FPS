// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "Components/ActorComponent.h"
#include Weapon_h
#include "CombatComponent.generated.h"

class ACreature;
class AKraverPlayer;
class AKraverPlayerController;
class AKraverHud;

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
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

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
	UFUNCTION(Client, Reliable)
	virtual void UnholsterWeapon(AWeapon* Weapon); // Weapon을 드는 함수
	UFUNCTION(Client, Reliable)
	virtual void HolsterWeapon(); // Weapon을 집어넣는 함수

	// Damage Event
	virtual float OnServer_TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser); // 데미지를 받는 함수 (서버에서 클라이언트로 TakeDamage이벤트 호출)
	virtual float OnServer_GiveDamage(AActor* DamagedActor, float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser); // 데미지를 주는 함수 (클라이언트에서 서버로 GiveDamage이벤트 호출)

	virtual void OnServer_CancelTakeDamage(); // 데미지 취소

protected:
	// Equip Weapon
	UFUNCTION(Server, Reliable)
	virtual void Server_EquipWeapon(AWeapon* Weapon); // Weapon을 장착하는 함수
	UFUNCTION(Server, Reliable)
	virtual void Server_UnEquipWeapon(AWeapon* Weapon); // Weapon을 장착하는 함수

	virtual void OnLocal_EquipWeaponSuccess(AWeapon* Weapon);		
	virtual void OnLocal_UnEquipWeaponSuccess(AWeapon* Weapon);

	UFUNCTION(Server, Reliable)
	virtual void Server_UnholsterWeapon(AWeapon* Weapon);
	UFUNCTION(Server, Reliable)
	virtual void Server_HolsterWeapon(AWeapon* Weapon); // Weapon을 집어넣는 함수


	// Take Damage
	UFUNCTION(Client, Reliable)
	void Client_TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);
	UFUNCTION(Client, Reliable)
	void Client_TakePointDamage(float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);
	UFUNCTION(Client, Reliable)
	void Client_TakeRadialDamage(float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_TakePointDamage(float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_TakeRadialDamage(float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);

	// Give Damage
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
	void Client_Death(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult); 
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Death(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult); 

	// Hp
	UFUNCTION(Client, Reliable)
	void Client_SetCurHp(int32 Value);

	// OnRep
	UFUNCTION()
	void OnRep_CurWeaponEvent(AWeapon* PrevWeapon);
	UFUNCTION()
	void OnRep_WeaponSlotEvent(TArray<AWeapon*> PrevWeaponSlot);

	// Func
	bool AddWeapon(AWeapon* Weapon); // 무기 슬롯에 무기 추가
	bool RemoveWeapon(AWeapon* Weapon); // 무기 슬롯에 무기 제거

public:
	// Getter Setter
	void SetIsAttacking(bool bAttack);
	void SetIsSubAttacking(bool bAttack);

	bool IsDead();

	int8 FindCurWeaponSlotIndex(); // 현재 장착중인 무기의 인덱스 값 구하기
	AWeapon* GetCurWeapon() { return CurWeapon; }
	int32 GetCurHp() { return CurHp; }
	int32 GetMaxHp() { return MaxHp; }
	bool GetCanEquipWeapon(); // 무기를 더 장착할 수 있는지
	const TArray<AWeapon*>& GetWeaponSlot() { return WeaponSlot; }
	const FTeamInfo& GetTeamInfo() { return TeamInfo; }

	int CountWeapon(); // 무기 슬롯에 있는 무기 개수 구하기

	void SetTeam(ETeam InTeam) { TeamInfo.CurTeam = InTeam; } 
	void SetUnholsterWhenEquip(bool InValue) { bUnholsterNextEquip = InValue; }

public:
	// Attack
	FAttackStartDele OnAttackStartDelegate; // 공격을 시작할때 호출
	FAttackEndDele OnAttackEndDelegate; // 공격을 멈출때 호출
	FAttackStartDele OnSubAttackStartDelegate; // 공격을 시작할때 호출
	FAttackEndDele OnSubAttackEndDelegate; // 공격을 멈출때 호출

	// Equip
	FEquipWeaponSuccessDele OnServerEquipWeaponSuccess; // 무기를 장착했을때 호출
	FEquipWeaponSuccessDele OnEquipWeaponSuccess; // 무기를 장착했을때 호출
	FUnEquipWeaponSuccessDele OnServerUnEquipWeaponSuccess; // 무기를 해제했을때 호출
	FUnEquipWeaponSuccessDele OnUnEquipWeaponSuccess; // 무기를 해제했을때 호출

	// Holster Holster
	FHolsterWeaponDele OnServerUnholsterWeapon;
	FHolsterWeaponDele OnUnholsterWeapon;
	FHolsterWeaponDele OnServerHolsterWeapon;
	FHolsterWeaponDele OnHolsterWeapon;

	// Take Damage
	FTakeDamageDele OnServerBeforeTakeDamage; // 데미지를 받기전에 호출

	FTakeDamageDele OnClientAfterTakeAnyDamageSuccess; // 데미지를 받았을때 호출
	FTakeDamageDele OnClientAfterTakeDamageSuccess; 
	FTakePointDamageDele OnClientAfterTakePointDamageSuccess; 
	FTakeRadialDamageDele OnClientAfterTakeRadialDamageSuccess; 

	FTakeDamageDele OnServerAfterTakeAnyDamageSuccess; 
	FTakeDamageDele OnServerAfterTakeDamageSuccess;
	FTakePointDamageDele OnServerAfterTakePointDamageSuccess;
	FTakeRadialDamageDele OnServerAfterTakeRadialDamageSuccess;

	FTakeDamageDele OnMulticastAfterTakeAnyDamageSuccess;
	FTakeDamageDele OnMulticastAfterTakeDamageSuccess;
	FTakePointDamageDele OnMulticastAfterTakePointDamageSuccess;
	FTakeRadialDamageDele OnMulticastAfterTakeRadialDamageSuccess;

	// Give Damage
	FGiveDamageDele OnClientGiveAnyDamageSuccess; // 데미지를 주었을때 호출
	FGiveDamageDele OnClientGiveDamageSuccess;
	FGivePointDamageDele OnClientGivePointDamageSuccess; 
	FGiveRadialDamageDele OnClientGiveRadialDamageSuccess;

	// Death
	FDeathDele OnClientDeath; // 죽었을때 호출
	FDeathDele OnServerDeath;
	FDeathDele OnMulticastDeath;

protected:
	ACreature* OwnerCreature;
	AKraverPlayer* OwnerPlayer;
	AKraverPlayerController* Controller;
	AKraverHud* HUD;

	UPROPERTY(ReplicatedUsing = OnRep_CurWeaponEvent, EditAnywhere, BlueprintReadWrite, Category = "Data|Weapon", meta = (AllowPrivateAccess = "true"))
	AWeapon* CurWeapon = nullptr; // 현재 무기
	UPROPERTY(ReplicatedUsing = OnRep_WeaponSlotEvent, EditAnywhere, BlueprintReadWrite, Category = "Data|Weapon", meta = (AllowPrivateAccess = "true"))
	TArray<AWeapon*> WeaponSlot; // Equip한 무기들을 가지고 있는 배열
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Weapon", meta = (AllowPrivateAccess = "true"))
	int32 MaxWeaponSlotSize = 3; // WeaponSlot 사이즈

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Data|Combat", meta = (AllowPrivateAccess = "true"))
	int32 CurHp = 100.f; // 현재 Hp
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Data|Combat", meta = (AllowPrivateAccess = "true"))
	int32 MaxHp = 100.f; // 최대 Hp

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Data|Team", meta = (AllowPrivateAccess = "true"))
	FTeamInfo TeamInfo; // 팀 정보

	bool bCancelNextDamage = false; // Server에서만 사용
	bool bUnholsterNextEquip = true;  // 무기를 장착했을때 바로 무기를 들것인지

};
