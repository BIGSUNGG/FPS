// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Character.h"
#include "Kraver/KraverComponent/CombatComponent.h"
#include "Kraver/KraverComponent/ServerComponent.h"
#include "Engine/DamageEvents.h"
#include "Creature.generated.h"

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	WALK		UMETA(DisplayName = "WALK"),
	RUN			UMETA(DisplayName = "RUN"),
	SPRINT		UMETA(DisplayName = "SPRINT"),
};

UCLASS()
class KRAVER_API ACreature : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACreature();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

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
	// Getter Setter
	FORCEINLINE FRotator GetCreatureAngle() { return Camera->GetComponentRotation() - GetMesh()->GetComponentRotation(); }
	FORCEINLINE UCameraComponent* GetCamera() { return Camera; }

	FORCEINLINE bool GetCanAttack();
	FORCEINLINE bool GetIsRunning() { return IsRunning; }
	FORCEINLINE bool GetIsJumping() { return IsJumping; }
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE EMovementState GetMovementState() { return MovementState; }

	void SetIsSprint(bool value);
protected:
	// Axis Input
	virtual void MoveForward(float NewAxisValue);
	virtual void MoveRight(float NewAxisValue);
	virtual void LookUp(float NewAxisValue);
	virtual void Turn(float NewAxisValue);

	// Button Input
	virtual void ReloadButtonPressed();
	virtual void AttackButtonPressed();
	virtual void AttackButtonReleased();
	virtual void RunButtonPressed();
	virtual void CrouchButtonPressed();
	virtual void JumpingButtonPressed();
	virtual void JumpingButtonReleased();

	virtual void AimOffset(float DeltaTime);

protected:
	// Delegate Event
	UFUNCTION()
		virtual void OnEquipWeaponSuccessEvent(AWeapon* Weapon);
	UFUNCTION()
		virtual void OnUnEquipWeaponSuccessEvent(AWeapon* Weapon);
	UFUNCTION()
		virtual void OnDeathEvent(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	UFUNCTION()
		void Landed(const FHitResult& Hit) override;
	void OnEndCrouch(float HeightAdjust, float ScaledHeightAdjust) override;

	// RPC
	UFUNCTION(Server, reliable)
		void Server_OnEquipWeaponSuccessEvent(AWeapon* Weapon);
	UFUNCTION(Server, reliable)
		void Server_OnUnEquipWeaponSuccessEvent(AWeapon* Weapon);
	UFUNCTION(Server, reliable)
		void Server_OnDeathEvent(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	UFUNCTION(NetMulticast, reliable)
		void Multicast_OnDeathEvent(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	UFUNCTION(Server, reliable)
		void Server_Landed(const FHitResult& Hit);
	UFUNCTION(NetMulticast, reliable)
		void Multicast_Landed(const FHitResult& Hit);
public:
	// Component
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

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = CREATURE, meta = (AllowPrivateAccess = "true"))
		EMovementState MovementState = EMovementState::WALK;
	virtual void SetMovementState(EMovementState value);
	UFUNCTION(Server, reliable)
		void Server_SetMovementState(EMovementState value);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CREATURE, meta = (AllowPrivateAccess = "true"))
		bool IsJumping = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CREATURE, meta = (AllowPrivateAccess = "true"))
		bool IsRunning = false;
	bool IsCrouching = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CREATURE, meta = (AllowPrivateAccess = "true"))
		float SprintSpeed = 1200.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CREATURE, meta = (AllowPrivateAccess = "true"))
		float RunSpeed = 800.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CREATURE, meta = (AllowPrivateAccess = "true"))
		float WalkSpeed = 800.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CREATURE, meta = (AllowPrivateAccess = "true"))
		float CrouchSpeed = 200.f;

}; 
