// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "GameFramework/Character.h"
#include "Kraver/KraverComponent/CombatComponent.h"
#include "Kraver/KraverComponent/ServerComponent.h"
#include "Engine/DamageEvents.h"
#include "Creature.generated.h"

// 캐릭터의 움직임 상태를 가지는 enum class
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
	FORCEINLINE FName GetWeaponAttachSocketName() { return WeaponAttachSocketName; }

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

	// Tick함수에서 호출될 함수
	virtual void AimOffset(float DeltaTime);

protected:
	// Delegate Event
	UFUNCTION()
		virtual void OnEquipWeaponSuccessEvent(AWeapon* Weapon); // 무기 장착 성공할때 호출되는 함수
	UFUNCTION()
		virtual void OnUnEquipWeaponSuccessEvent(AWeapon* Weapon); // 무기 장착해제 성공할때 호출되는 함수
	UFUNCTION()
		virtual void OnDeathEvent(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser); // Hp가 0이하가 되었을때 호출되는 함수
	UFUNCTION()
		void Landed(const FHitResult& Hit) override; // 착지했을때 호출되는 함수
	UFUNCTION()
		virtual void OnAfterTakePointDamageEvent(float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	void OnEndCrouch(float HeightAdjust, float ScaledHeightAdjust) override; // 일어났을때 호출되는 함수

	// RPC
	UFUNCTION(Server, reliable)
		void Server_OnEquipWeaponSuccessEvent(AWeapon* Weapon); // 무기 장착 성공할때 서버에서 호출되는 함수
	UFUNCTION(Server, reliable)
		void Server_OnUnEquipWeaponSuccessEvent(AWeapon* Weapon); // 무기 장착해제 성공할때 서버에서 호출되는 함수
	UFUNCTION(Server, reliable)
		void Server_OnDeathEvent(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser); // Hp가 0이하가 되었을때 서버에서 호출되는 함수
	UFUNCTION(NetMulticast, reliable)
		void Multicast_OnDeathEvent(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser); // Hp가 0이하가 되었을때 모든 클라이언트에서 호출되는 함수
	UFUNCTION(Server, reliable)
		void Server_Landed(const FHitResult& Hit); // 착지했을때 서버에서 호출되는 함수 
	UFUNCTION(NetMulticast, reliable)
		void Multicast_Landed(const FHitResult& Hit); // 착지했을때 모든 클라이언트에서 호출되는 함수

public:
	// Component
	UServerComponent* ServerComponent;
	UCombatComponent* CombatComponent;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CAMERA, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* Camera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CAMERA, meta = (AllowPrivateAccess = "true"))
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

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = CREATURE, meta = (AllowPrivateAccess = "true"))
		EMovementState MovementState = EMovementState::WALK;
	virtual void SetMovementState(EMovementState value);
	UFUNCTION(Server, reliable)
		void Server_SetMovementState(EMovementState value);

	UPROPERTY(Replicated , VisibleAnywhere, BlueprintReadWrite, Category = CREATURE, meta = (AllowPrivateAccess = "true"))
		bool IsJumping = false;
	void SetIsJumping(bool value);
	UFUNCTION(Server, reliable)
		void Server_SetIsJumping(bool value);
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CREATURE, meta = (AllowPrivateAccess = "true"))
		bool IsRunning = false;
	bool IsCrouching = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CREATURE, meta = (AllowPrivateAccess = "true"))
		float SprintSpeed = 1200.f; // EMovementState가 SPRINT가 되었을때 설정할 캐릭터 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CREATURE, meta = (AllowPrivateAccess = "true"))
		float RunSpeed = 800.f; // EMovementState가 RUN가 되었을때 설정할 캐릭터 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CREATURE, meta = (AllowPrivateAccess = "true"))
		float WalkSpeed = 800.f; // EMovementState가 WALK가 되었을때 설정할 캐릭터 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CREATURE, meta = (AllowPrivateAccess = "true"))
		float CrouchSpeed = 200.f; // 앉았을때 설정할 캐릭터 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CREATURE, meta = (AllowPrivateAccess = "true"))
		float ImpulseResistanceRatio = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowPrivateAccess = "true"))
		FName WeaponAttachSocketName = "RightHandWeapon"; // Weapon을 Attach할 스켈레탈 본 이름
}; 
