// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "GameFramework/Character.h"
#include CombatComponent_h
#include WeaponAssassinateComponent_h
#include "Creature.generated.h"

class UWeaponAssassinateComponent;

UCLASS()
class KRAVER_API ACreature : public ACharacter
{
	GENERATED_BODY()

public:
	virtual void OnServer_Assassinated(ACreature* Attacker, FAssassinateInfo AssassinateInfo);
	virtual void OnServer_AssassinatedEnd();

public:
	// Sets default values for this character's properties
	ACreature(const FObjectInitializer& ObjectInitializer);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void OwnOtherActor(AActor* Actor);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	virtual void PostInitializeComponents() override;

	virtual void OnRep_Controller();
	virtual void PossessedBy(AController* NewController) override;

	// 서버와 클라이언트에서 빙의 시 호출
	virtual void OnPossessed(AController* NewController);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void RootTick(float DeltaTime);
	virtual void CameraTick(float DeltaTime);
	virtual void Jump() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Delegate
	UFUNCTION()
	virtual void OnAssassinateEvent(AActor* AssassinatedActor); // 캐릭터가 암살을 시작할때 호출
	UFUNCTION()
	virtual void OnAssassinateEndEvent(); // 캐릭터가 암살을 당하기 시작할때 호출

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
	virtual void TurnInPlace(float DeltaTime);

	// Delegate Event
		// Equip Success
	UFUNCTION()
	virtual void OnEquipWeaponSuccessEvent(AWeapon* Weapon); // 무기 장착 성공할때 호출되는 함수
	UFUNCTION()
	virtual void OnUnEquipWeaponSuccessEvent(AWeapon* Weapon); // 무기 장착해제 성공할때 호출되는 함수
	UFUNCTION()
	virtual void OnServerEquipWeaponSuccessEvent(AWeapon* Weapon); // 무기 장착 성공할때 서버에서 호출되는 함수
	UFUNCTION()
	virtual void OnServerUnEquipWeaponSuccessEvent(AWeapon* Weapon); // 무기 장착해제 성공할때 서버에서 호출되는 함수
			
		// Holster
	UFUNCTION()
	virtual void OnUnholsterWeaponEvent(AWeapon* Weapon); // 무기를 들때 호출되는 함수
	UFUNCTION()
	virtual void OnServerUnholsterWeaponEvent(AWeapon* Weapon); // 무기를 들때 호출되는 함수
	UFUNCTION()
	virtual void OnHolsterWeaponEvent(AWeapon* Weapon); // 무기를 들때 호출되는 함수
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
	
	// RPC
	UFUNCTION(Server, Reliable)
	void Server_OwnOtherActor(AActor* Actor);
		
		// Assassinate
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_Assassinated(ACreature* Attacker, FAssassinateInfo AssassinateInfo);
	UFUNCTION(Client, Reliable)
	virtual void Client_Assassinated(ACreature* Attacker, FAssassinateInfo AssassinateInfo);

		// Jump
	UFUNCTION(Server, Reliable)
	virtual void Server_Jump();
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_Jump();

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

	virtual void OnServer_SimulateMesh();
	
	UFUNCTION()
	void StartDissolve(); 
	void UpdateDissolveMaterial(float DissolveValue);

public:
	// Getter Setter
	virtual bool CanAttack(); // 공격할 수 있는지
	virtual bool CanSubAttack(); // 보조 공격할 수 있는지
	virtual bool CanRun(); // 달릴 수 있는지

	FORCEINLINE FRotator GetCreatureAngle() { return Camera ? Camera->GetComponentRotation() - GetMesh()->GetComponentRotation() : FRotator::ZeroRotator; }
	FORCEINLINE UCameraComponent* GetCamera() { return Camera; }
	FORCEINLINE bool GetbRunButtonPress() { return bRunButtonPress; }
	bool GetbJumpButtonPress() { return bJumpButtonPress; }
	bool GetbCrouchButtonPress() { return bCrouchButtonPress; }
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FRotator GetStartingAimRotation() { return TppRootRotation; }
	float CalculateForwardSpeed(); // 컨트롤러 방향 기준으로 앞으로 전진하는 속도
	float CalculateRightSpeed(); // 컨트롤러 방향 기준으로 옆으로 이동하는 속도
	float CalculateCurrentFloorSlope(); // 현재 캐릭터가 밟고 있는 바닥의 각도 구하기
	FVector CaclulateCurrentFloorSlopeVector(); // 현재 캐릭터가 밟고 있는 바닥의 각도를 벡터로 구함
	virtual USkeletalMeshComponent* GetCurMainMesh() { return GetMesh(); }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }

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
	// Default
	float DefaultCapsuleHalfHeight;

	// Component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Camera", meta = (AllowPrivateAccess = "true"))
	USceneCaptureComponent2D* ScopeCaptureComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Camera", meta = (AllowPrivateAccess = "true"))
	UTextureRenderTarget2D* ScopeRenderTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|First person", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|First person", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Fp_Root;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|First person", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|First person", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* Fp_SpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Widget", meta = (AllowPrivateAccess = "true"))
	class ULookCameraWidgetComponent* HpBarWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|AimOffset")
	float AO_Pitch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|AimOffset")
	float AO_Yaw;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|AimOffset")
	ETurningInPlace TurningInPlace;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|AimOffset")
	FRotator TppRootRotation;
	float InterpRoot_Yaw;
	
	// Crouch
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Camera", meta = (AllowPrivateAccess = "true"))
	float CurCrouchRootHeight = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Camera", meta = (AllowPrivateAccess = "true"))
	float CrouchRootHeight = -45.f; // 앉았을때 목표 카메라 높이
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Camera", meta = (AllowPrivateAccess = "true"))
	float UnCrouchRootHeight = 0.f; // 일어났을때 목표 카메라 높이

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Resist", meta = (AllowPrivateAccess = "true"))
	float ImpulseResistanceRatio = 1.f;

	// InputState
	bool bAttackButtonPress = false;
	bool bSubAttackButtonPress = false;
	bool bJumpButtonPress = false;
	bool bCrouchButtonPress = false;
	bool bRunButtonPress = false;

	// Disolve
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Dissolve", meta = (AllowPrivateAccess = "true"))
	UTimelineComponent* DissolveTimeline;
	FOnTimelineFloat DissolveTrack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Dissolve", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* DissolveCurve;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data|Combat|Dissolve", meta = (AllowPrivateAccess = "true"))
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Dissolve", meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* DissolveMaterialInstance;

}; 
