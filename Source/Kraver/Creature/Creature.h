// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "GameFramework/Character.h"
#include "Kraver/KraverComponent/Combat/CombatComponent.h"
#include "Kraver/KraverComponent/Rpc/RpcComponent.h"
#include "Kraver/Weapon/Weapon.h"
#include "Engine/DamageEvents.h"
#include "Creature.generated.h"

struct FAssassinateInfo;

UCLASS()
class KRAVER_API ACreature : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACreature();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	virtual void Assassinated(ACreature* Attacker, FAssassinateInfo AssassinateInfo);

	void OwningOtherActor(AActor* Actor);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void CameraTick(float DeltaTime);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Delegate
	UFUNCTION()
		virtual void OnAssassinateEvent(AActor* AssassinatedActor);
	UFUNCTION()
		virtual void OnAssassinateEndEvent();

public:
	// Getter Setter
	FORCEINLINE FRotator GetCreatureAngle() { return Camera->GetComponentRotation() - GetMesh()->GetComponentRotation(); }
	FORCEINLINE UCameraComponent* GetCamera() { return Camera; }
	virtual bool GetCanAttack();
	FORCEINLINE bool GetIsRunning() { return IsRunning; }
	FORCEINLINE bool GetIsJumping() { return IsJumping; }
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE EMovementState GetMovementState() { return MovementState; }
	float CalculateForwardSpeed();
	float CalculateRightSpeed();
	float CalculateCurrentFloorSlope();
	FVector CaclulateCurrentFllorSlopeVector();
	virtual USkeletalMeshComponent* GetCurMainMesh() { return GetMesh(); }

protected:
	// Axis Input
	virtual void MoveForward(float NewAxisValue);
	virtual void MoveRight(float NewAxisValue);
	virtual void LookUp(float NewAxisValue);
	virtual void Turn(float NewAxisValue);

	// Button Input
	UFUNCTION(BlueprintCallable)
		virtual void ReloadButtonPressed();
	UFUNCTION(BlueprintCallable)
		virtual void AttackButtonPressed();
	UFUNCTION(BlueprintCallable)
		virtual void AttackButtonReleased();
	UFUNCTION(BlueprintCallable)
		virtual void SubAttackButtonPressed();
	UFUNCTION(BlueprintCallable)
		virtual void SubAttackButtonReleased();
	UFUNCTION(BlueprintCallable)
		virtual void RunButtonPressed();
	UFUNCTION(BlueprintCallable)
		virtual void RunButtonReleased();
	UFUNCTION(BlueprintCallable)
		virtual void CrouchButtonPressed();
	UFUNCTION(BlueprintCallable)
		virtual void CrouchButtonReleased();
	UFUNCTION(BlueprintCallable)
		virtual void JumpingButtonPressed();
	UFUNCTION(BlueprintCallable)
		virtual void JumpingButtonReleased();
	UFUNCTION(BlueprintCallable)
		virtual void HolsterWeaponPressed();

	// Tick함수에서 호출될 함수
	virtual void AimOffset(float DeltaTime);

	// Delegate Event
	UFUNCTION()
		virtual void OnEquipWeaponSuccessEvent(AWeapon* Weapon); // 무기 장착 성공할때 호출되는 함수
	UFUNCTION()
		virtual void OnUnEquipWeaponSuccessEvent(AWeapon* Weapon); // 무기 장착해제 성공할때 호출되는 함수
	UFUNCTION()
		virtual void OnHoldWeaponEvent(AWeapon* Weapon); // 무기를 들때 호출되는 함수
	UFUNCTION()
		virtual void OnHolsterWeaponEvent(AWeapon* Weapon); // 무기를 들때 호출되는 함수
	UFUNCTION()
		virtual void OnDeathEvent(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser); // Hp가 0이하가 되었을때 호출되는 함수
	UFUNCTION()
		void Landed(const FHitResult& Hit) override; // 착지했을때 호출되는 함수
	UFUNCTION()
		virtual void OnAfterTakePointDamageEvent(float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	void OnEndCrouch(float HeightAdjust, float ScaledHeightAdjust) override; // 일어났을때 호출되는 함수
	UFUNCTION()
		virtual void OnPlayWeaponTppMontageEvent(UAnimMontage* PlayedMontage, float Speed);
	UFUNCTION()
		virtual void OnPlayWeaponFppMontageEvent(UAnimMontage* PlayedMontage, float Speed);

	// RPC
	UFUNCTION(Server, reliable)
		void Server_OnEquipWeaponSuccessEvent(AWeapon* Weapon); // 무기 장착 성공할때 서버에서 호출되는 함수
	UFUNCTION(Server, reliable)
		void Server_OnUnEquipWeaponSuccessEvent(AWeapon* Weapon); // 무기 장착해제 성공할때 서버에서 호출되는 함수
	UFUNCTION(Server, reliable)
		void Server_OnDeathEvent(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser); // Hp가 0이하가 되었을때 서버에서 호출되는 함수
	UFUNCTION(NetMulticast, reliable)
		void Multicast_OnDeathEvent(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser); // Hp가 0이하가 되었을때 모든 클라이언트에서 호출되는 함수

	// Function
	virtual void PlayLandedMontage();

	virtual void Jump();

public:
	// Component
	URpcComponent* RpcComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Camera", meta = (AllowPrivateAccess = "true"))
		UCombatComponent* CombatComponent;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Camera", meta = (AllowPrivateAccess = "true"))
		UCameraComponent* Camera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|SprintArm", meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* SpringArm;

	UPROPERTY(Replicated)
		float AO_Yaw;
	void SetAO_Yaw(float value);
	UFUNCTION(Server, reliable)
		void Server_SetAO_Yaw(float value);
	UPROPERTY(Replicated)
		float AO_Pitch;
	void SetAO_Pitch(float value);
	UFUNCTION(Server, reliable)
		void Server_SetAO_Pitch(float value);
	FRotator StartingAimRotation;
	FVector TargetCameraRelativeLocation;

	// Movement
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Data|State", meta = (AllowPrivateAccess = "true"))
		EMovementState MovementState = EMovementState::WALK;
	virtual void SetMovementState(EMovementState value) final;
	UFUNCTION(Server, reliable)
		void Server_SetMovementState(EMovementState value);

	UPROPERTY(Replicated , VisibleAnywhere, BlueprintReadWrite, Category = "Data|Movement", meta = (AllowPrivateAccess = "true"))
		bool IsJumping = false;
	void SetIsJumping(bool value);
	UFUNCTION(Server, reliable)
		void Server_SetIsJumping(bool value);
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data|Movement", meta = (AllowPrivateAccess = "true"))
		bool IsRunning = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data|Movement", meta = (AllowPrivateAccess = "true"))
		float SprintSpeed = 1200.f; // EMovementState가 SPRINT가 되었을때 설정할 캐릭터 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data|Movement", meta = (AllowPrivateAccess = "true"))
		float RunSpeed = 600.f; // EMovementState가 RUN가 되었을때 설정할 캐릭터 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data|Movement", meta = (AllowPrivateAccess = "true"))
		float WalkSpeed = 600.f; // EMovementState가 WALK가 되었을때 설정할 캐릭터 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data|Movement", meta = (AllowPrivateAccess = "true"))
		float CrouchWalkSpeed = 200.f; // 앉았을때 설정할 캐릭터 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data|Movement", meta = (AllowPrivateAccess = "true"))
		float CrouchRunSpeed = 200.f; // 앉았을때 설정할 캐릭터 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data|Movement", meta = (AllowPrivateAccess = "true"))
		float CrouchSprintSpeed = 450.f; // 앉았을때 설정할 캐릭터 속도

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data|Combat", meta = (AllowPrivateAccess = "true"))
		float ImpulseResistanceRatio = 1.f;

	// InputState
	float CurrentInputForward = 0.f;
	float CurrentInputRight = 0.f;
	float InputForwardRatio = 1.f;
	float InputRightRatio = 1.f;
	bool bJumpButtonPress = false;
	bool bCrouchButtonPress = false;

	// Default Value
	float DefaultGravity = 0.f;
	float DefaultGroundFriction = 0.f;
	float DefaultBrakingDecelerationWalking = 0.f;
	FVector DefaultCameraLocation;
}; 
