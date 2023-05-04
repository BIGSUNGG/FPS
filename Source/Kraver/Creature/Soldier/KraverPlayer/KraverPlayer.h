// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Creature/Soldier/Soldier.h"
#include "KraverPlayer.generated.h"

/**
 * 
 */

// 캐릭터의 AdvancedMovement 상태를 가지는 enum class
UENUM(BlueprintType)
enum class EWallRunState : uint8
{
	NONE		UMETA(DisplayName = "NONE"),
	WALLRUN_RIGHT UMETA(DisplayName = "WALLRUN_RIGHT"),
	WALLRUN_LEFT UMETA(DisplayName = "WALLRUN_LEFT"),
	WALLRUN_VERTICAL UMETA(DisplayName = "WALLRUN_VERTICAL"),
};

// 카메라 시점 종류를 가지는 enum class
UENUM(BlueprintType)
enum class EViewType : uint8
{
	FIRST_PERSON   UMETA(DisplayName = "FIRST_PERSON"),
	THIRD_PERSON   UMETA(DisplayName = "THIRD_PERSON"),
};

UCLASS()
class KRAVER_API AKraverPlayer : public ASoldier
{
	GENERATED_BODY()
public:
	AKraverPlayer();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	virtual void CameraTick(float DeletaTime) override;
	virtual void CameraTilt(float TargetRoll);
	virtual void ClientTick(float DeltaTime);
	virtual void ServerClientTick(float DeltaTime);
	virtual void LocallyControlTick(float DeltaTime);
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Getter Setter
	FORCEINLINE AWeapon* GetCanInteractWeapon() { return CanInteractWeapon; }
	FORCEINLINE USkeletalMeshComponent* GetArmMesh() { return ArmMesh; }
	FORCEINLINE TMap<AWeapon*, USkeletalMeshComponent*> GetArmWeaponMeshes() { return ArmWeaponMeshes; }
	const bool GetIsWallRunning() { return CurWallRunState != EWallRunState::NONE; }
	virtual bool GetCanAttack() override;
	EWallRunState GetCurWallRunState() { return CurWallRunState; }
	bool GetIsSliding() { return IsSliding; }
	virtual USkeletalMeshComponent* GetCurMainMesh() override;

protected:
	// input event
	virtual void SubAttackButtonPressed() override;
	virtual void SubAttackButtonReleased() override;
	virtual void EquipButtonPressed();
	virtual void UnEquipButtonPressed();
	virtual void ChangeWeapon1Pressed();
	virtual void ChangeWeapon2Pressed();
	virtual void ChangeWeapon3Pressed();

	virtual void CheckCanInteractionWeapon(); // 장착가능한 무기를 찾는 함수
	virtual void ChangeView(); // 현재 카메라 시점을 변경하는 함수
	virtual void ThrowWeapon(AWeapon* Weapon);

 	virtual void OnDeathEvent(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void RefreshArm();
	virtual void RefreshSpringArm(); // SpringArm의 RelativeLocation을 새로고침하는 함수
	UFUNCTION(Server, reliable)
		void Server_RefreshSpringArm(FVector Vector, float Length); // SpringArm의 RelativeLocation을 서버에서 새로고침하는 함수
	UFUNCTION(NetMulticast, reliable)
		void Multicast_RefreshSpringArm(FVector Vector, float Length); // SpringArm의 RelativeLocation을 서버에서 새로고침하는 함수
	virtual void RefreshCurViewType(); // 현재 카메라 시점으로 새로고침하는 함수

	// Delegate Event
	void Landed(const FHitResult& Hit) override; // 착지했을때 호출되는 함수

	virtual void OnEquipWeaponSuccessEvent(AWeapon* Weapon) override;
	virtual void OnUnEquipWeaponSuccessEvent(AWeapon* Weapon) override;
	virtual void OnHoldWeaponEvent(AWeapon* Weapon); // 무기를 들때 호출되는 함수
	virtual void OnHolsterWeaponEvent(AWeapon* Weapon); // 무기를 넣을때 호출되는 함수

	virtual void OnAssassinateEvent(AActor* AssassinatedActor) override;
	virtual void OnAssassinateEndEvent() override;

	// Function
	void Crouch(bool bClientSimulation = false) override;
	void UnCrouch(bool bClientSimulation = false) override;

	virtual void PlayReloadMontage() override;
	virtual void PlayAttackMontage() override;
	virtual void PlayLandedMontage() override;

	virtual void StopReloadMontage() override;

	void WeaponADS(float DeltaTime);
	void SpringArmTick(float DeltaTime);

	virtual void Jump() override;

	// Double Jump
	void DoubleJump();

	// Wall Run
	void WallRunJump();
	bool WallRunUpdate();
	bool WallRunHorizonUpdate();
	bool WallRunVerticalUpdate();
	bool WallRunHorizonMovement(FVector Start, FVector End, float WallRunDirection);
	bool WallRunVerticalMovement(FVector Start, FVector End);
	void WallRunStart();
	void WallRunEnd(float ResetTime);
	void WallRunHorizonEnd(float ResetTime);
	void WallRunVerticalEnd(float ResetTime);
	void ResetWallRunHorizonSuppression();
	void ResetWallRunVerticalSuppression();
	void SuppressWallRunHorizion(float Delay);
	void SuppressWallRunVertical(float Delay);
	FVector CalculateRightWallRunEndVector();
	FVector CalculateLeftWallRunEndVector();
	FVector CalculateVerticaltWallRunEndVector();
	bool CalculateValidWallVector(FVector InVec);
	FVector CalculatePlayerToWallVector();

	// Slide
	bool CanSlide();
	void SlideUpdate();
	void SlideStart();
	void SlideEnd(bool DoUncrouch);
	void SuppressSlide(float Delay);
	void ResetSlideSuppression();
public:
	UPROPERTY(EditAnywhere, Category = Crosshairs)
		class UTexture2D* CrosshairsCenter;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* CrosshairsLeft;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* CrosshairsRight;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* CrosshairsTop;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* CrosshairsBottom;

protected:
	class AKraverPlayerController* KraverController;
	class AKraverHUD* HUD;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Camera", meta = (AllowPrivateAccess = "true"))
		EViewType ViewType = EViewType::FIRST_PERSON;
	void SetViewType(EViewType Type);
	UFUNCTION(Server, reliable)
		void Server_SetViewType(EViewType Type);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Mesh", meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* ArmMesh;

	TMap<AWeapon*, USkeletalMeshComponent*> ArmWeaponMeshes; // 각 Weapon들의 WeaponMesh를 1인칭 시점에 맞게 복사한 매쉬들의 맵

	AWeapon* CanInteractWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Interaction", meta = (AllowPrivateAccess = "true"))
		float InteractionDistance = 300.f; // 장착가능한 무기를 찾는 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Interaction", meta = (AllowPrivateAccess = "true"))
		float InteractionRadius = 25.f; // 장착가능한 무기를 찾는 범위의 반지름

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Camera", meta = (AllowPrivateAccess = "true"))
		float CrouchCameraHeight = -40.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Camera", meta = (AllowPrivateAccess = "true"))
		float UnCrouchCameraHeight = 0.f;


	TArray<UPrimitiveComponent*> ShowOnlyFirstPerson; // 1인칭 시점일때만 보이는 컴포넌트
	TArray<UPrimitiveComponent*> ShowOnlyThirdPerson; // 3인칭 시점일때만 보이는 컴포넌트
	FVector SpringArmBasicLocation; // 기본적으로 적용할 SprintArm의 RelativeLocation
	FVector SpringArmCrouchLocation; // 추가적으로 적용할 SprintArm의 RelativeLocation

	FTimerHandle UnEquipWeaponTimerHandle;
	float WeaponThrowPower = 700.f; // 장착해제된 무기를 던지는 힘
	FVector WeaponThrowAngularPower = FVector(100, 100, 0); // 장착해제된 무기를 던지는 힘

	FVector BasicArmLocation;
	FRotator BasicArmRotation;
	FRotator WeaponAdsRotation;
	FVector WeaponAdsLocation;

	// Movement
	bool bWantToJump = false;

	// Advanced Movement / Double Jump
	bool bCanDoubleJump = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Movement|AdvancedMovement", meta = (AllowPrivateAccess = "true"))
		FVector DobuleJumpPower;

	// Advanced Movement / Wall Run
	UPROPERTY(Replicated)
		EWallRunState CurWallRunState;
	void SetCurWallRunState(EWallRunState Value);
	UFUNCTION(Server, reliable)
		void Server_SetCurWallRunState(EWallRunState Value);

	bool bWallRunHorizonSupressed = false;
	bool bWallRunVerticalSupressed = false;
	bool bWallRunGravity = true;
	float WallRunHorizonSpeed = 1200.f;
	float WallRunVerticalSpeed = 600.f;
	float WallRunJumpHeight = 400.f;
	float WallRunJumpOffForce  = 400.f;
	float WallRunTargetGravity = 0.75f;
	FVector WallRunNormal;
	FTimerHandle SuppressWallRunHorizonTimer;
	FTimerHandle SuppressWallRunVerticalTimer;

	// Advanced Movement / Slide
	UPROPERTY(Replicated)
		bool IsSliding = false;
	void SetIsSliding(bool Value);
	UFUNCTION(Server, Reliable)
		void Server_SetIsSliding(bool Value);

	bool bSlideSupressed = false;
	float MinSlideRequireSpeed = 900.f;
	float SlideSlopeSpeed = 20.f;
	float SlideSpeed = 2000.f;
	float SlideDuration = 2.f;
	float SlideGroundFriction = 0.f;
	float SlideBrakingDecelerationWalking = 1024.f;
	FTimerHandle SuppressSlideTimer;
};
