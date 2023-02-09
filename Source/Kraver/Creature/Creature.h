// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Character.h"
#include "Kraver/KraverComponent/CombatComponent.h"
#include "Kraver/KraverComponent/ServerComponent.h"
#include "Creature.generated.h"

UCLASS()
class KRAVER_API ACreature : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACreature();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void OwningOtherActor(AActor* Actor);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	virtual FRotator GetCreatureAngle() { return FRotator::ZeroRotator; }

	FORCEINLINE bool GetCanAttack();
	FORCEINLINE bool GetIsRunning() { return IsRunning; }
	FORCEINLINE bool GetIsSprint() {return IsSprint;}
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }

	void SetIsSprint(bool value);
protected:
	virtual void MoveForward(float NewAxisValue);
	virtual void MoveRight(float NewAxisValue);
	virtual void LookUp(float NewAxisValue);
	virtual void Turn(float NewAxisValue);

	virtual void ReloadButtonPressed();
	virtual void AttackButtonPressed();
	virtual void AttackButtonReleased();
	virtual void RunButtonPressed();
	virtual void CrouchButtonPressed();
	virtual void JumpingButtonPressed();
	virtual void JumpingButtonReleased();

	virtual void AimOffset(float DeltaTime);

protected:
	UFUNCTION()
		virtual void OnEquipWeaponSuccess(AWeapon* Weapon);
	UFUNCTION()
		virtual void OnUnEquipWeaponSuccess(AWeapon* Weapon);
	UFUNCTION(Server, reliable)
		virtual void Server_OnEquipWeaponSuccess(AWeapon* Weapon);
	UFUNCTION(Server, reliable)
		virtual void Server_OnUnEquipWeaponSuccess(AWeapon* Weapon);

public:
	UServerComponent* ServerComponent;
	UCombatComponent* CombatComponent;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CAMERA, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* Camera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CAMERA, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* SpringArm;

	float AO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CREATURE, meta = (AllowPrivateAccess = "true"))
		bool IsJumping = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CREATURE, meta = (AllowPrivateAccess = "true"))
		bool IsRunning = false;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = CREATURE, meta = (AllowPrivateAccess = "true"))
		bool IsSprint = false;
	UFUNCTION(Server, reliable)
		void OnServer_SetIsSprint(bool value);
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CREATURE, meta = (AllowPrivateAccess = "true"))
		float SprintSpeed = 1200.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CREATURE, meta = (AllowPrivateAccess = "true"))
		float RunSpeed = 800.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CREATURE, meta = (AllowPrivateAccess = "true"))
		float WalkSpeed = 800.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CREATURE, meta = (AllowPrivateAccess = "true"))
		float CrouchSpeed = 200.f;

}; 
