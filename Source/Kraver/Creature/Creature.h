// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "GameFramework/Character.h"
#include "Kraver/KraverComponent/Combat/CombatComponent.h"
#include "Creature.generated.h"

UCLASS()
class KRAVER_API ACreature : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACreature();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Assassinated(ACreature* Attacker, FAssassinateInfo AssassinateInfo);
	virtual void AssassinatedEnd();

	void OwnOtherActor(AActor* Actor);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void CameraTick(float DeltaTime);
	virtual void Jump() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Delegate
	UFUNCTION()
		virtual void OnAssassinateEvent(AActor* AssassinatedActor);
	UFUNCTION()
		virtual void OnAssassinateEndEvent();

public:
	// Getter Setter
	FORCEINLINE FRotator GetCreatureAngle() { return Camera ? Camera->GetComponentRotation() - GetMesh()->GetComponentRotation() : FRotator::ZeroRotator; }
	FORCEINLINE UCameraComponent* GetCamera() { return Camera; }
	virtual bool GetCanAttack();
	FORCEINLINE bool GetbRunButtonPress() { return bRunButtonPress; }
	bool GetbJumpButtonPress() { return bJumpButtonPress; }
	bool GetbCrouchButtonPress() { return bCrouchButtonPress; }
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	float CalculateForwardSpeed();
	float CalculateRightSpeed();
	float CalculateCurrentFloorSlope();
	FVector CaclulateCurrentFllorSlopeVector();
	virtual USkeletalMeshComponent* GetCurMainMesh() { return GetMesh(); }
	float GetCurrentInputForward() { return CurrentInputForward; }
	float GetCurrentInputRight() { return CurrentInputRight; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }

protected:
	// Axis Input
	virtual void MoveForward(float NewAxisValue);
	virtual void MoveRight(float NewAxisValue);
	virtual void LookUp(float NewAxisValue);
	virtual void Turn(float NewAxisValue);

	// Button Input
	UFUNCTION(BlueprintCallable)
		virtual void ReloadButtonPressed() final;
	UFUNCTION(BlueprintCallable)
		virtual void AttackButtonPressed() final;
	UFUNCTION(BlueprintCallable)
		virtual void AttackButtonReleased() final;
	UFUNCTION(BlueprintCallable)
		virtual void SubAttackButtonPressed() final;
	UFUNCTION(BlueprintCallable)
		virtual void SubAttackButtonReleased() final;
	UFUNCTION(BlueprintCallable)
		virtual void RunButtonPressed() final;
	UFUNCTION(BlueprintCallable)
		virtual void RunButtonReleased() final;
	UFUNCTION(BlueprintCallable)
		virtual void CrouchButtonPressed() final;
	UFUNCTION(BlueprintCallable)
		virtual void CrouchButtonReleased() final;
	UFUNCTION(BlueprintCallable)
		virtual void JumpingButtonPressed() final;
	UFUNCTION(BlueprintCallable)
		virtual void JumpingButtonReleased() final;
	UFUNCTION(BlueprintCallable)
		virtual void HolsterWeaponPressed() final;

	// Tick함수에서 호출될 함수
	virtual void AimOffset(float DeltaTime);

	// Delegate Event
	UFUNCTION()
		virtual void OnClientEquipWeaponSuccessEvent(AWeapon* Weapon); // 무기 장착 성공할때 호출되는 함수
	UFUNCTION()
		virtual void OnClientUnEquipWeaponSuccessEvent(AWeapon* Weapon); // 무기 장착해제 성공할때 호출되는 함수
	UFUNCTION()
		virtual void OnServerEquipWeaponSuccessEvent(AWeapon* Weapon); // 무기 장착 성공할때 서버에서 호출되는 함수
	UFUNCTION()
		virtual void OnServerUnEquipWeaponSuccessEvent(AWeapon* Weapon); // 무기 장착해제 성공할때 서버에서 호출되는 함수

	UFUNCTION()
		virtual void OnClientHoldWeaponEvent(AWeapon* Weapon); // 무기를 들때 호출되는 함수
	UFUNCTION()
		virtual void OnServerHoldWeaponEvent(AWeapon* Weapon); // 무기를 들때 호출되는 함수
	UFUNCTION()
		virtual void OnClientHolsterWeaponEvent(AWeapon* Weapon); // 무기를 들때 호출되는 함수
	UFUNCTION()
		virtual void OnServerHolsterWeaponEvent(AWeapon* Weapon); // 무기를 들때 호출되는 함수
	UFUNCTION()
		virtual void OnClientDeathEvent(float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult); // Hp가 0이하가 되었을때 호출되는 함수
	UFUNCTION()
		virtual void OnServerDeathEvent(float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult); // Hp가 0이하가 되었을때 서버에서 호출되는 함수

	virtual void Landed(const FHitResult& Hit) override; // 착지했을때 호출되는 함수
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HeightAdjust, float ScaledHeightAdjust) override; // 일어났을때 호출되는 함수
	virtual void OnJumped_Implementation() override;

	UFUNCTION()
		virtual void OnClientAfterTakeDamageEvent(float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);
	UFUNCTION()
		virtual void OnPlayWeaponTppMontageEvent(UAnimMontage* PlayedMontage, float Speed);
	UFUNCTION()
		virtual void OnPlayWeaponFppMontageEvent(UAnimMontage* PlayedMontage, float Speed);

	// RPC
	UFUNCTION(Server, Reliable)
		void Server_OwnOtherActor(AActor* Actor);
	UFUNCTION(Server, Reliable)
		virtual void Server_Assassinated(ACreature* Attacker, FAssassinateInfo AssassinateInfo);
	UFUNCTION(NetMulticast, Reliable)
		virtual void Multicast_Assassinated(ACreature* Attacker, FAssassinateInfo AssassinateInfo);
	UFUNCTION(Client, Reliable)
		virtual void Client_Assassinated(ACreature* Attacker, FAssassinateInfo AssassinateInfo);
	UFUNCTION(NetMulticast, reliable)
		void Multicast_OnUnEquipWeaponSuccessEvent(AWeapon* Weapon); // 무기 장착해제 성공할때 서버에서 호출되는 함수
	UFUNCTION(NetMulticast, reliable)
		void Multicast_OnDeathEvent(float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser); // Hp가 0이하가 되었을때 모든 클라이언트에서 호출되는 함수
	UFUNCTION(Server, reliable)
		virtual void Server_OnAfterTakeDamageEvent(float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	UFUNCTION(NetMulticast, reliable)
		virtual void Multicast_OnAfterTakeDamageEvent(float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	UFUNCTION(Server, reliable)
		virtual void Server_Jump();
	UFUNCTION(NetMulticast, reliable)
		virtual void Multicast_Jump();
	UFUNCTION(Server, reliable)
		virtual void Server_OnPlayWeaponTppMontageEvent(UAnimMontage* PlayedMontage, float Speed);
	UFUNCTION(NetMulticast, reliable)
		virtual void Multicast_OnPlayWeaponTppMontageEvent(UAnimMontage* PlayedMontage, float Speed);
	UFUNCTION(Server, reliable)
		virtual void Server_OnPlayWeaponFppMontageEvent(UAnimMontage* PlayedMontage, float Speed);
	UFUNCTION(NetMulticast, reliable)
		virtual void Multicast_OnPlayWeaponFppMontageEvent(UAnimMontage* PlayedMontage, float Speed);
	UFUNCTION(NetMulticast, Reliable)
		virtual void Multicast_HoldWeaponEvent(AWeapon* Weapon);
	UFUNCTION(NetMulticast, Reliable)
		virtual void Multicast_HolsterWeaponEvent(AWeapon* Weapon);
	UFUNCTION(Client, reliable)
		virtual void Client_SimulateMesh();
	UFUNCTION(NetMulticast, reliable)
		virtual void Multicast_SimulateMesh();
	UFUNCTION(Server, reliable)
		virtual void Server_OnAssassinatedEndEvent();

	// Function
	virtual void PlayLandedMontage();

	virtual void SimulateMesh();

public:
	// Delegate
	FCrouchDele OnCrouchStart;
	FCrouchDele OnCrouchEnd;
	FLandedDele OnLand;
	FJumpDele OnJump;

	// Component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Camera", meta = (AllowPrivateAccess = "true"))
		class UCombatComponent* CombatComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Movement", meta = (AllowPrivateAccess = "true"))
		class UCreatureMovementComponent* CreatureMovementComponent;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|First person", meta = (AllowPrivateAccess = "true"))
		USceneComponent* Fp_Root;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|First person", meta = (AllowPrivateAccess = "true"))
		UCameraComponent* Camera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|First person", meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* Fp_SpringArm;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data|Combat", meta = (AllowPrivateAccess = "true"))
		float ImpulseResistanceRatio = 1.f;

	// InputState
	float CurrentInputForward = 0.f;
	float CurrentInputRight = 0.f;
	bool bJumpButtonPress = false;
	bool bCrouchButtonPress = false;
	bool bRunButtonPress = false;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);
	float InterpAO_Yaw;

}; 
