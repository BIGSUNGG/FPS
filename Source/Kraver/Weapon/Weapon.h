// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kraver/Creature/Creature.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	NONE   UMETA(DisplayName = "NONE"),
	EQUIPPED   UMETA(DisplayName = "EQUIPPED"),
};

UCLASS()
class KRAVER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void Equipped(ACreature* Character);
	UFUNCTION(Server, Reliable)
		void Server_Equipped(ACreature* Character);
public:
	const FName& GetAttachSocketName() { return AttachSocketName; }
	bool GetCanInteract();
	USkeletalMeshComponent* GetWeaponMesh() {return WeaponMesh;}
protected:
	UFUNCTION()
		virtual void AttackStartEvent();
	UFUNCTION()
		virtual void AttackEndEvent();	

	virtual void Attack();

protected:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "WeaponState", meta = (AllowPrivateAccess = "true"))
		EWeaponState WeaponState = EWeaponState::NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* WeaponMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
		TMap<FName, UStaticMeshComponent*> AttachmentMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
		FName AttachSocketName = "SOCKET_Weapon_L";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim", meta = (AllowPrivateAccess = "true"))
		UAnimationAsset* IdleAnim;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Owner", meta = (AllowPrivateAccess = "true"))
		ACreature* OwnerCharacter = nullptr;

	bool IsAttacking = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		bool IsAutomaticAttack = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack" , meta = (AllowPrivateAccess = "true"))
		float AttackDelay = 0.2f;

	FTimerHandle AutomaticAttackHandle;
	
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	//	UProjectileMovementComponent* ProjectileMovement;

};
