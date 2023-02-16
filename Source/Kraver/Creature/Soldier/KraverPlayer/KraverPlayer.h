// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Kraver/Creature/Soldier/Soldier.h"
#include "Kraver/Hud/InteractionWidget.h"
#include "KraverPlayer.generated.h"

/**
 * 
 */

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
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void CrouchButtonPressed() override;
	virtual void EquipButtonPressed();
	virtual void CheckCanInteractionWeapon();
	virtual void ChangeView();

	virtual void RefreshSpringArm();
	UFUNCTION(Server, reliable)
		void Server_RefreshSpringArm(FVector Vector, float Length);
	virtual void RefreshCurViewType();

	virtual void OnEquipWeaponSuccess(AWeapon* Weapon) override;
	virtual void Server_OnEquipWeaponSuccess_Implementation(AWeapon* Weapon) override;
	virtual void OnUnEquipWeaponSuccess(AWeapon* Weapon) override;
	virtual void Server_OnUnEquipWeaponSuccess_Implementation(AWeapon* Weapon) override;

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

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = CAMERA, meta = (AllowPrivateAccess = "true"))
		EViewType ViewType = EViewType::FIRST_PERSON;
	void SetViewType(EViewType Type);
	UFUNCTION(Server, reliable)
		void Server_SetViewType(EViewType Type);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Arm, meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* ArmMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Arm, meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* ArmWeaponMesh;

	AWeapon* CanInteractWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interaction, meta = (AllowPrivateAccess = "true"))
		float InteractionDistance = 225.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interaction, meta = (AllowPrivateAccess = "true"))
		float InteractionRadius = 25.f;

	TArray<UPrimitiveComponent*> ShowOnlyFirstPerson;
	TArray<UPrimitiveComponent*> ShowOnlyThirdPerson;
	FVector SpringArmBasicLocation;
	FVector SpringArmAdditiveLocation;

	FTimerHandle UnEquipWeaponTimerHandle;
	float UnEquipWeaponThrowPower = 500.f;
};
