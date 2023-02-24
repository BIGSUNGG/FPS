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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FTakePointDamageDele, float, DamageAmount, FPointDamageEvent const&, DamageEvent, AController*, EventInstigator, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FTakeRadialDamageDele, float, DamageAmount, FRadialDamageEvent const&, DamageEvent, AController*, EventInstigator, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FDeathDele, float, DamageAmount, FDamageEvent const&, DamageEvent, AController*, EventInstigator, AActor*, DamageCauser);

// 무기와 전투에 관련된 컴포넌트
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAVER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();
	float CalculateDamage(float DamageAmount, FDamageEvent const& DamageEvent); // 해당 컴포넌트가 받을 데미지를 계산
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void Death(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser); // Hp가 0이하가 되었을경우 호출

public:
	// Getter Setter
	AWeapon* GetCurWeapon() { return CurWeapon; }
	float GetCurHp() { return CurHp; }
	bool GetCanEquipWeapon() { return CurWeapon == nullptr; }
	bool IsDead();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetHUDCrosshairs(float DeltaTime);

	virtual void RefillAmmo(); // 재장전하는 함수
	virtual void EquipWeapon(AWeapon* Weapon); // Weapon을 장착하는 함수
	virtual void UnEquipWeapon(AWeapon* Weapon); // Weapon을 장착해제하는 함수
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser); // 데미지를 받는 함수 (서버에서 클라이언트로 TakeDamage이벤트 호출)
	virtual float GiveDamage(AActor* DamagedActor, float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser); // 데미지를 주는 함수 (클라이언트에서 서버로 GiveDamage이벤트 호출)

protected:
	UFUNCTION(Server, reliable)
		void Server_EquipWeapon(AWeapon* Weapon);
	UFUNCTION(Server, reliable)
		void Server_UnEquipWeapon(AWeapon* Weapon);

	// Take Damage
	UFUNCTION(Server, reliable)
		void Server_TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	UFUNCTION(Server, reliable)
		void Server_TakePointDamage(float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	UFUNCTION(Server, reliable)
		void Server_TakeRadialDamage(float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION(Client, reliable)
		void Client_TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	UFUNCTION(Client, reliable)
		void Client_TakeRadialDamage(float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	UFUNCTION(Client, reliable)
		void Client_TakePointDamage(float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	// Give Damage
	UFUNCTION(Server, reliable)
		void Server_GiveDamage(AActor* DamagedActor, float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	UFUNCTION(Server, reliable)
		void Server_GivePointDamage(AActor* DamagedActor, float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	UFUNCTION(Server, reliable)
		void Server_GiveRadialDamage(AActor* DamagedActor, float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
public:
	// Getter Setter
	void SetIsAttacking(bool bAttack);

public:
	FAttackStartDele OnAttackStartDelegate; // 공격을 시작할때 호출
	FAttackEndDele OnAttackEndDelegate; // 공격을 멈출때 호출

	FEquipWeaponSuccessDele OnEquipWeaponSuccess; // 무기를 장착했을때 호출
	FServerEquipWeaponSuccessDele OnServerEquipWeaponSuccess;
	FUnEquipWeaponSuccessDele OnUnEquipWeaponSuccess; // 무기를 해제했을때 호출
	FServerUnEquipWeaponSuccessDele OnServerUnEquipWeaponSuccess;

	FTakeDamageDele OnAfterTakeDamage; // 데미지를 받았을때 호출
	FTakePointDamageDele OnAfterTakePointDamage;
	FTakeRadialDamageDele OnAfterTakeRadialDamge;
	FDeathDele OnDeath; // 죽었을때 호출

protected:
	ACreature* OwnerCreature;
	AKraverPlayer* OwnerPlayer;
	AKraverPlayerController* Controller;
	AKraverHUD* HUD;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess = "true"))
		AWeapon* CurWeapon = nullptr; // 현재 무기
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess = "true"))
		int32 CurHp = 100.f; // 현재 Hp
	UFUNCTION(Server, reliable)
		void Server_CurHp(int32 value);
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess = "true"))
		int32 MaxHp = 100.f; // 최대 Hp
	UFUNCTION(Server, reliable)
		void Server_MaxHp(int32 value);

};
