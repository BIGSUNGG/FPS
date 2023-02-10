// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Weapon.generated.h"

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

	virtual int32 AddAdditiveWeaponMesh(USkeletalMeshComponent* Mesh);
	virtual int32 RemoveAdditiveWeaponMesh(USkeletalMeshComponent* Mesh);

	virtual bool Reload();
	virtual bool Equipped(ACreature* Character);
	virtual bool UnEquipped();
protected:
	UFUNCTION(Server, Reliable)
		virtual void Server_AddAdditiveWeaponMesh(USkeletalMeshComponent* Mesh);

	UFUNCTION(Server, Reliable)
		void Server_Equipped(ACreature* Character);
	UFUNCTION(Server, Reliable)
		void Server_UnEquipped();

	UFUNCTION()
		virtual void AttackStartEvent();
	UFUNCTION()
		virtual void AttackEndEvent();	

	virtual void Attack();
public:
	bool GetCanInteracted();
	bool GetIsAttacking() {return IsAttacking;}
	ACreature* GetOwnerCreature() { return OwnerCreature; }
	EWeaponType GetWeaponType() { return WeaponType; }
	EWeaponState GetWeaponState() { return WeaponState; }
	const FName& GetAttachSocketName() { return AttachSocketName; }
	USkeletalMeshComponent* GetWeaponMesh() {return WeaponMesh;}

protected:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Owner", meta = (AllowPrivateAccess = "true"))
		class ACreature* OwnerCreature = nullptr;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "WeaponState", meta = (AllowPrivateAccess = "true"))
		EWeaponState WeaponState = EWeaponState::NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponType", meta = (AllowPrivateAccess = "true"))
		EWeaponType WeaponType = EWeaponType::NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* WeaponMesh;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Additive", meta = (AllowPrivateAccess = "true"))
		TArray<USkeletalMeshComponent*> AdditiveWeaponMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
		TMap<FName, UStaticMeshComponent*> AttachmentMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
		FName AttachSocketName = "RightHandWeapon";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim", meta = (AllowPrivateAccess = "true"))
		UAnimationAsset* IdleAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		bool IsAttacking = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		bool bAutomaticAttack = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		bool bFirstAttackDelay = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack" , meta = (AllowPrivateAccess = "true"))
		float AttackDelay = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		float AttackDamage = 10.f;

	FTimerHandle AutomaticAttackHandle;

};
