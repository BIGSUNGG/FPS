// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Weapon.generated.h"

UCLASS()
class KRAVER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void Equipped(ACharacter* Character, bool AttachToMesh = true);

public:
	USkeletalMeshComponent* GetWeaponMesh() { return WeaponMesh; }
	const FName& GetAttachSocketName() { return AttachSocketName; }
	bool GetCanInteract() { return (OwnerCharacter == nullptr); }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowPrivateAccess = "true"))
		FName AttachSocketName = "SOCKET_Weapon_L";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Anim, meta = (AllowPrivateAccess = "true"))
		UAnimationAsset* IdleAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Owner, meta = (AllowPrivateAccess = "true"))
		ACharacter* OwnerCharacter;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	//	UProjectileMovementComponent* ProjectileMovement;

};
