// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Character/Creature/Soldier/Soldier.h"
#include "KraverPlayer.generated.h"

/**
 * 
 */

UCLASS()
class KRAVER_API AKraverPlayer : public ASoldier
{
	GENERATED_BODY()
public:
	AKraverPlayer();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	virtual void CameraTick(float DeletaTime) override;
	virtual void ArmMeshTick(float DeletaTime);
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
	FORCEINLINE TMap<AWeapon*, TMap<FString, UPrimitiveComponent*>*> GetArmWeaponMeshes() { return ArmWeaponMeshes; }
	virtual bool GetCanAttack() override;
	virtual USkeletalMeshComponent* GetCurMainMesh() override;
	FRotator GetWeaponSwayResultRot() { return WeaponSwayResultRot; }

protected:
	// input event
	UFUNCTION(BlueprintCallable)
		virtual void EquipButtonPressed() final;
	UFUNCTION(BlueprintCallable)
		virtual void UnEquipButtonPressed() final;
	UFUNCTION(BlueprintCallable)
		virtual void ChangeWeapon1Pressed() final;
	UFUNCTION(BlueprintCallable)
		virtual void ChangeWeapon2Pressed() final;
	UFUNCTION(BlueprintCallable)
		virtual void ChangeWeapon3Pressed() final;

	virtual void CheckCanInteractionWeapon(); // 장착가능한 무기를 찾는 함수
	virtual void ChangeView(); // 현재 카메라 시점을 변경하는 함수
	virtual void ThrowWeapon(AWeapon* Weapon);

	virtual void RefreshArm();
	virtual void RefreshSpringArm(); // SpringArm의 RelativeLocation을 새로고침하는 함수

	// Rpc
	UFUNCTION(Server, reliable)
		void Server_RefreshSpringArm(FVector Vector, float Length); // SpringArm의 RelativeLocation을 서버에서 새로고침하는 함수
	UFUNCTION(NetMulticast, reliable)
		void Multicast_RefreshSpringArm(FVector Vector, float Length); // SpringArm의 RelativeLocation을 서버에서 새로고침하는 함수
	UFUNCTION(Server, reliable)
		void Server_ThrowWeapon(AWeapon* Weapon, FTransform Transform, FVector Direction);
	UFUNCTION(NetMulticast, reliable)
		void Multicast_ThrowWeapon(AWeapon* Weapon, FTransform Transform, FVector Direction);
	virtual void Multicast_OnPlayWeaponFppMontageEvent_Implementation(UAnimMontage* PlayedMontage, float Speed);

	virtual void RefreshCurViewType(); // 현재 카메라 시점으로 새로고침하는 함수

	// Delegate Event
	virtual void OnClientEquipWeaponSuccessEvent(AWeapon* Weapon) override;
	virtual void OnClientUnEquipWeaponSuccessEvent(AWeapon* Weapon) override;
	virtual void OnClientHoldWeaponEvent(AWeapon* Weapon); // 무기를 들때 호출되는 함수
	virtual void OnClientHolsterWeaponEvent(AWeapon* Weapon); // 무기를 넣을때 호출되는 함수
	
	virtual void Client_SimulateMesh_Implementation() override;

	virtual void OnAssassinateEvent(AActor* AssassinatedActor) override;
	virtual void OnAssassinateEndEvent() override;
	virtual void OnPlayWeaponFppMontageEvent(UAnimMontage* PlayedMontage, float Speed) override;

	UFUNCTION()
		virtual void OnFP_Reload_Grab_MagazineEvent();
	UFUNCTION()
		virtual void OnFP_Reload_Insert_MagazineEvent();

	// Function
	virtual void PlayLandedMontage() override;

	void WeaponADS(float DeltaTime);
	void WeaponSway(float DeltaTime);
	void SpringArmTick(float DeltaTime);

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Third person", meta = (AllowPrivateAccess = "true"))
		USceneComponent* Tp_Root;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Third person", meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* Tp_SpringArm;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Camera", meta = (AllowPrivateAccess = "true"))
		EViewType ViewType = EViewType::FIRST_PERSON;
	void SetViewType(EViewType Type);
	UFUNCTION(Server, reliable)
		void Server_SetViewType(EViewType Type);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Mesh", meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* ArmMesh;

	TMap<AWeapon*, TMap<FString, UPrimitiveComponent*>*> ArmWeaponMeshes; // 각 Weapon들의 WeaponMesh를 1인칭 시점에 맞게 복사한 매쉬들의 맵

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Interaction", meta = (AllowPrivateAccess = "true"))
		AWeapon* CanInteractWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Interaction", meta = (AllowPrivateAccess = "true"))
		float InteractionDistance = 500.f; // 장착가능한 무기를 찾는 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Interaction", meta = (AllowPrivateAccess = "true"))
		float InteractionRadius = 25.f; // 장착가능한 무기를 찾는 범위의 반지름

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Camera", meta = (AllowPrivateAccess = "true"))
		float CrouchCameraHeight = -40.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Camera", meta = (AllowPrivateAccess = "true"))
		float UnCrouchCameraHeight = 0.f;


	TArray<UPrimitiveComponent*> ShowOnlyFirstPerson; // 1인칭 시점일때만 보이는 컴포넌트
	TArray<UPrimitiveComponent*> ShowOnlyThirdPerson; // 3인칭 시점일때만 보이는 컴포넌트
	FVector Fp_SpringArmBasicLocation; // 기본적으로 적용할 SprintArm의 RelativeLocation
	FVector FP_SpringArmCrouchLocation; // 추가적으로 적용할 SprintArm의 RelativeLocation

	FTimerHandle UnEquipWeaponTimerHandle;
	float WeaponThrowPower = 700.f; // 장착해제된 무기를 던지는 힘
	FVector WeaponThrowAngularPower = FVector(100, 100, 0); // 장착해제된 무기를 던지는 힘

	FVector BasicArmLocation;
	FRotator BasicArmRotation;
	FVector AdsArmLocation;

	// Weapon Sway
	FRotator WeaponSwayResultRot;
	float SwayValue = 1.5f;
	float MaxSwayDegree = 2.f;
	float MinSwayDegree = -2.f;

};
