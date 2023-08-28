// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "GameFramework/Character.h"
#include "Kraver/Component/Combat/CombatComponent.h"
#include "Kraver/Component/Skill/Weapon/WeaponAssassinate/WeaponAssassinateComponent.h"
#include "Creature.generated.h"

class UWeaponAssassinateComponent;

UCLASS()
class KRAVER_API ACreature : public ACharacter
{
	GENERATED_BODY()

	// Friend
	friend void UWeaponAssassinateComponent::Server_Assassinate_Implementation(AActor* Actor);
	friend void UWeaponAssassinateComponent::Server_OnAssassinateEndEvent_Implementation();

public:
	// Sets default values for this character's properties
	ACreature();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void OwnOtherActor(AActor* Actor);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void TurnInPlace(float DeltaTime);
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
	virtual bool CanAttack();
	virtual bool CanRun();

	FORCEINLINE FRotator GetCreatureAngle() { return Camera ? Camera->GetComponentRotation() - GetMesh()->GetComponentRotation() : FRotator::ZeroRotator; }
	FORCEINLINE UCameraComponent* GetCamera() { return Camera; }
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

	virtual void SetWeaponVisibility(class AWeapon* Weapon, bool Value);

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

	// Tick함수에서 호출될 함수
	virtual void AimOffset(float DeltaTime);

	// Delegate Event
		// Equip Success
	UFUNCTION()
		virtual void OnClientEquipWeaponSuccessEvent(AWeapon* Weapon); // 무기 장착 성공할때 호출되는 함수
	UFUNCTION()
		virtual void OnClientUnEquipWeaponSuccessEvent(AWeapon* Weapon); // 무기 장착해제 성공할때 호출되는 함수
	UFUNCTION()
		virtual void OnServerEquipWeaponSuccessEvent(AWeapon* Weapon); // 무기 장착 성공할때 서버에서 호출되는 함수
	UFUNCTION()
		virtual void OnServerUnEquipWeaponSuccessEvent(AWeapon* Weapon); // 무기 장착해제 성공할때 서버에서 호출되는 함수
			
		// Holster
	UFUNCTION()
		virtual void OnClientUnholsterWeaponEvent(AWeapon* Weapon); // 무기를 들때 호출되는 함수
	UFUNCTION()
		virtual void OnServerUnholsterWeaponEvent(AWeapon* Weapon); // 무기를 들때 호출되는 함수
	UFUNCTION()
		virtual void OnClientHolsterWeaponEvent(AWeapon* Weapon); // 무기를 들때 호출되는 함수
	UFUNCTION()
		virtual void OnServerHolsterWeaponEvent(AWeapon* Weapon); // 무기를 들때 호출되는 함수

		// Death
	UFUNCTION()
		virtual void OnClientDeathEvent(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult); // Hp가 0이하가 되었을때 호출되는 함수
	UFUNCTION()
		virtual void OnServerDeathEvent(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult); // Hp가 0이하가 되었을때 서버에서 호출되는 함수
	UFUNCTION()
		virtual void OnMulticastDeathEvent(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult); // Hp가 0이하가 되었을때 서버에서 호출되는 함수

	virtual void Landed(const FHitResult& Hit) override; // 착지했을때 호출되는 함수
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HeightAdjust, float ScaledHeightAdjust) override; // 일어났을때 호출되는 함수
	virtual void OnJumped_Implementation() override;

		// Take Damage
	UFUNCTION()
		virtual void OnClientAfterTakePointDamageEvent(float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);
	UFUNCTION()
		virtual void OnClientAfterTakeRadialDamageEvent(float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);

	UFUNCTION()
		virtual void OnServerAfterTakePointDamageEvent(float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);
	UFUNCTION()
		virtual void OnMulticastAfterTakePointDamageEvent(float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);

	UFUNCTION()
		virtual void OnServerAfterTakeRadialDamageEvent(float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);
	UFUNCTION()
		virtual void OnMulticastAfterTakeRadialDamageEvent(float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);

		// Montage
	UFUNCTION()
		virtual void OnPlayWeaponTppMontageEvent(UAnimMontage* PlayedMontage, float Speed);
	UFUNCTION()
		virtual void OnPlayWeaponFppMontageEvent(UAnimMontage* PlayedMontage, float Speed);
	
		// OnRep
	UFUNCTION() 
		virtual void OnRepCurWeaponEvent(AWeapon* PrevWeapon, AWeapon* CurWeapon);
	UFUNCTION()
		virtual void OnRepWeaponSlotEvent(const TArray<AWeapon*>& PrevWeaponSlot, const TArray<AWeapon*>& CurWeaponSlot);

	// RPC
	UFUNCTION(Server, Reliable)
		void Server_OwnOtherActor(AActor* Actor);
		
		// Assassinate
	UFUNCTION(Server, Reliable)
		virtual void Server_Assassinated(ACreature* Attacker, FAssassinateInfo AssassinateInfo);
	UFUNCTION(NetMulticast, Reliable)
		virtual void Multicast_Assassinated(ACreature* Attacker, FAssassinateInfo AssassinateInfo);
	UFUNCTION(Client, Reliable)
		virtual void Client_Assassinated(ACreature* Attacker, FAssassinateInfo AssassinateInfo);
	UFUNCTION(Server, Reliable)
		virtual void Server_OnAssassinatedEndEvent();

		// Jump
	UFUNCTION(Server, Reliable)
		virtual void Server_Jump();
	UFUNCTION(NetMulticast, Reliable)
		virtual void Multicast_Jump();

		// Montage
	UFUNCTION(Server, Reliable)
		virtual void Server_OnPlayWeaponTppMontageEvent(UAnimMontage* PlayedMontage, float Speed);
	UFUNCTION(NetMulticast, Reliable)
		virtual void Multicast_OnPlayWeaponTppMontageEvent(UAnimMontage* PlayedMontage, float Speed);

	UFUNCTION(Server, Reliable)
		virtual void Server_OnPlayWeaponFppMontageEvent(UAnimMontage* PlayedMontage, float Speed);
	UFUNCTION(NetMulticast, Reliable)
		virtual void Multicast_OnPlayWeaponFppMontageEvent(UAnimMontage* PlayedMontage, float Speed);

		// SimulateMesh
	UFUNCTION(Client, Reliable)
		virtual void Client_SimulateMesh();
	UFUNCTION(NetMulticast, Reliable)
		virtual void Multicast_SimulateMesh();

	// Function
	virtual void AttackStart();
	virtual void AttackEnd();
	virtual void SubAttackStart();
	virtual void SubAttackEnd();

	virtual void PlayLandedMontage();

	virtual void SimulateMesh();

	virtual void Assassinated(ACreature* Attacker, FAssassinateInfo AssassinateInfo);
	virtual void AssassinatedEnd();

	virtual void EquipEvent(AWeapon* Weapon);
	virtual void UnEquipEvent(AWeapon* Weapon);
	virtual void HolsterEvent(AWeapon* Weapon);
	virtual void UnholsterEvent(AWeapon* Weapon);
	
	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);
	void StartDissolve();
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

	float AO_Yaw;
	float AO_Pitch;

	FRotator StartingAimRotation;
	FVector TargetCameraRelativeLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data|Combat|Resist", meta = (AllowPrivateAccess = "true"))
		float ImpulseResistanceRatio = 1.f;

	// InputState
	float CurrentInputForward = 0.f;
	float CurrentInputRight = 0.f;
	bool bAttackButtonPress = false;
	bool bSubAttackButtonPress = false;
	bool bJumpButtonPress = false;
	bool bCrouchButtonPress = false;
	bool bRunButtonPress = false;

	ETurningInPlace TurningInPlace;
	float InterpAO_Yaw;

	// Disolve
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Dissolve", meta = (AllowPrivateAccess = "true"))
	UTimelineComponent* DissolveTimeline;
	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Dissolve", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* DissolveCurve;

	// Dynamic instance that we can change at runtime
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data|Combat|Dissolve", meta = (AllowPrivateAccess = "true"))
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;

	// Material instance set on the Blueprint, used with the dynamic material instance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Dissolve", meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* DissolveMaterialInstance;
}; 
