// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Weapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAttackDele);

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	NONE   UMETA(DisplayName = "NONE"),
	EQUIPPED   UMETA(DisplayName = "EQUIPPED"),
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	NONE   UMETA(DisplayName = "NONE"),
	GUN   UMETA(DisplayName = "GUN"),
};

UCLASS()
class KRAVER_API AWeapon : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AWeapon();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual int32 MakeAdditiveWeaponMesh(); // 추가적인 WeaponMesh를 추가 (Return 값은 추가된 WeaponMesh의 인덱스값)
	virtual int32 RemoveAdditiveWeaponMesh(USkeletalMeshComponent* Mesh); // 추가적인 WeaponMesh를 제거 (Return 값은 제거된 WeaponMesh의 인덱스값)
	virtual int32 FindAdditiveWeaponMesh(USkeletalMeshComponent* Mesh); // 추가적인 WeaponMesh를 찾음 (Return 값은 찾은 WeaponMesh의 인덱스값 못찾았을 경우 -1)

	virtual bool RefillAmmo(); // CurAmmo를 보충함
	virtual bool Equipped(ACreature* Character); // Character에게 장착됨
	virtual bool UnEquipped(); // 장착해제됨
	virtual bool Hold();
	virtual bool Holster();
protected:
	UFUNCTION(Server, Reliable)
		void Server_Equipped(ACreature* Character);
	UFUNCTION(Server, Reliable)
		void Server_UnEquipped();

	UFUNCTION()
		virtual void AttackStartEvent(); // 캐릭터의 공격이 시작하였을때 호출되는 함수
	UFUNCTION()
		virtual void AttackEndEvent(); // 캐릭터의 공격이 끝났을때 호출되는 함수

	virtual void Attack(); // 공격할때 호출되는 함수
public:
	// Getter Setter
	bool GetCanInteracted();
	bool GetIsAttacking() {return IsAttacking;}
	virtual bool GetCanReload() { return false; }
	float GetAttackImpulse() { return AttackImpulse; }
	ACreature* GetOwnerCreature() { return OwnerCreature; }
	EWeaponType GetWeaponType() { return WeaponType; }
	EWeaponState GetWeaponState() { return WeaponState; }
	USkeletalMeshComponent* GetWeaponMesh() {return WeaponMesh;}
	TArray<USkeletalMeshComponent*> GetAdditiveWeaponMesh() { return AdditiveWeaponMesh; }

	UAnimMontage* GetReloadMontageTpp() { return ReloadMontageTpp; }
	UAnimMontage* GetReloadMontageFpp() { return ReloadMontageFpp; }
	UAnimMontage* GetAttackMontageTpp() { return AttackMontageTpp; }
	UAnimMontage* GetAttackMontageFpp() { return AttackMontageFpp; }
public:
	FAttackDele OnAttack;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		class UTexture2D* CrosshairsCenter;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* CrosshairsLeft;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* CrosshairsRight;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* CrosshairsTop;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* CrosshairsBottom;

protected:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Owner", meta = (AllowPrivateAccess = "true"))
		class ACreature* OwnerCreature = nullptr;
	UFUNCTION(Server, Reliable)
		void Server_SetOwnerCreature(ACreature* pointer);

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "WeaponState", meta = (AllowPrivateAccess = "true"))
		EWeaponState WeaponState = EWeaponState::NONE; // 무기 상태
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponType", meta = (AllowPrivateAccess = "true"))
		EWeaponType WeaponType = EWeaponType::NONE; // 무기 종류

	// Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", Meta = (AllowPrivateAccess = true))
		UAnimMontage* ReloadMontageTpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", Meta = (AllowPrivateAccess = true))
		UAnimMontage* ReloadMontageFpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", Meta = (AllowPrivateAccess = true))
		UAnimMontage* AttackMontageTpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", Meta = (AllowPrivateAccess = true))
		UAnimMontage* AttackMontageFpp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* WeaponMesh; // 기본적으로 보이는 메쉬
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Additive", meta = (AllowPrivateAccess = "true"))
		TArray<USkeletalMeshComponent*> AdditiveWeaponMesh; // 추가적인 WeaponMesh를 가지는 배열

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		bool IsAttacking = false; // 공격중인지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		bool bAutomaticAttack = false; // 연사공격이 가능한지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		bool bFirstAttackDelay = false; // 첫공격에 딜레이가 있는지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		bool bFirstInputAttack = false; // 공격 선입력이 입력되어있는지 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack" , meta = (AllowPrivateAccess = "true"))
		float AttackDelay = 0.2f; // 공격 딜레이
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack" , meta = (AllowPrivateAccess = "true"))
		float CurAttackDelay = 0.f; // 현재 공격 딜레이
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		float AttackDamage = 10.f; // 공격을 하였을때 주는 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		float AttackImpulse = 150.f; // 공격을 하였을때 주는 충격량

	FTimerHandle AutomaticAttackHandle; // 연사공격을 담당하는 TimerHandle
};
