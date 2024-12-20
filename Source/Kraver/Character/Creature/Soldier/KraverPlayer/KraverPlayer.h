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
	AKraverPlayer(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	// Tick
	virtual void Tick(float DeltaTime) override;
	virtual void ServerClientTick(float DeltaTime);
	virtual void LocallyControlTick(float DeltaTime);
	virtual void ClientTick(float DeltaTime);

	virtual void CameraTilt(float TargetRoll);
	virtual void CameraTick(float DeletaTime) override;
	virtual void ArmMeshTick(float DeletaTime);
	virtual void WeaponADS(float DeltaTime);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	// input event
	UFUNCTION(BlueprintCallable)
	virtual void HolsterButtonPressed() final;
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

	// Ect Function
	virtual void CheckCanInteractionWeapon(); // 장착가능한 무기를 찾는 함수
	virtual void ChangeView(); // 현재 카메라 시점을 변경하는 함수
	virtual void ThrowWeapon(AWeapon* Weapon);

	virtual void RefreshCurViewType(); // 현재 카메라 시점으로 새로고침하는 함수

	// Rpc
	UFUNCTION(Server, Reliable)
	void Server_ThrowWeapon(AWeapon* Weapon, FTransform Transform, FVector Direction);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ThrowWeapon(AWeapon* Weapon, FTransform Transform, FVector Direction);
		
	virtual void Client_Assassinated_Implementation(ACreature* Attacker, FAssassinateInfo AssassinateInfo) override;

		// Holster
	UFUNCTION(Server, Reliable)
	virtual void Server_HolsterWeapon();
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_HolsterWeapon();
	UFUNCTION(Server, Reliable)
	virtual void Server_UnholsterWeapon();
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_UnholsterWeapon();

	// Delegate Event
		// Equip
	virtual void OnEquipWeaponSuccessEvent(AWeapon* Weapon) override;
	virtual void OnUnEquipWeaponSuccessEvent(AWeapon* Weapon) override;
		
		// Holster Unholster
	virtual void OnUnholsterWeaponEvent(AWeapon* Weapon) override; // 무기를 들때 호출되는 함수
	virtual void OnHolsterWeaponEvent(AWeapon* Weapon) override; // 무기를 넣을때 호출되는 함수
	
		// Simulate Mesh
	virtual void Client_SimulateMesh_Implementation() override;

		// Assassinate
	virtual void OnAssassinateEvent(AActor* AssassinatedActor) override;
	virtual void OnAssassinateEndEvent() override;

		// Montage
	virtual void OnPlayWeaponFppMontageEvent(UAnimMontage* PlayedMontage, float Speed) override;

	virtual void OnReload_Grab_MagazineEvent() override;
	virtual void OnReload_Insert_MagazineEvent() override;
	UFUNCTION()
	virtual void OnTp_Weapon_HolsterEvent();

	// Function
	virtual void PlayLandedMontage() override;
	void ChangeWeapon(int8 Index); // 무기 변경

		// Holster
	void HolsterWeapon();
	void HolsterWeaponEvent();
	void UnholsterWeapon();
	void UnholsterWeaponEvent();

public:
	// Getter Setter
	FORCEINLINE AWeapon* GetCanInteractWeapon() { return CanInteractWeapon; }
	FORCEINLINE USkeletalMeshComponent* GetArmMesh() { return ArmMesh; }
	FORCEINLINE USpringArmComponent* GetTp_SpringArm() { return Tp_SpringArm; }
	virtual USkeletalMeshComponent* GetCurMainMesh() override;

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
	class AKraverHud* HUD;

	// Component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Third person", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Tp_Root;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Third person", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* Tp_SpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Mesh", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ArmMesh;
		
	// BeginPlay
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|BeginPlay", meta = (AllowPrivateAccess = "true"))
	ULevelSequence* LevelFadeSquence;	

	// ViewType
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Camera", meta = (AllowPrivateAccess = "true"))
	EViewType CurViewType = EViewType::FIRST_PERSON; // 현재 1인칭인지 3인칭인지

	TArray<UPrimitiveComponent*> ShowOnlyFirstPerson; // 1인칭 시점일때만 보이는 컴포넌트
	TArray<UPrimitiveComponent*> ShowOnlyThirdPerson; // 3인칭 시점일때만 보이는 컴포넌트

	// Interaction
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Interaction", meta = (AllowPrivateAccess = "true"))
	AWeapon* CanInteractWeapon; // 장착가능한 무기
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Interaction", meta = (AllowPrivateAccess = "true"))
	float InteractionDistance = 700.f; // 장착가능한 무기를 찾는 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Interaction", meta = (AllowPrivateAccess = "true"))
	float InteractionRadius = 5.f; // 장착가능한 무기를 찾는 범위의 반지름

	float CameraBasicFov = 110.f; // 기본 Fov값

	float WeaponThrowPower = 700.f; // 장착해제된 무기를 던지는 힘
	FVector WeaponThrowAngularPower = FVector(100, 100, 0); // 장착해제된 무기를 던지는 힘

	FVector BasicArmLocation;
	FRotator BasicArmRotation;
	FVector AdsArmLocation;

	// Holster
	int8 UnholsterIndex = -1; // 다음에 들 무기 인덱스 값

};
