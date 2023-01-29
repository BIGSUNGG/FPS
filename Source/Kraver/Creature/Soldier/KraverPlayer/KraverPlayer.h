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

public:
	virtual FRotator GetCreatureAngle() override { return Camera->GetComponentRotation() - GetMesh()->GetComponentRotation(); }

protected:
	virtual void CrouchButtonPressed() override;
	virtual void EquipButtonPressed();
	virtual void CheckCanInteractionWeapon();
	virtual void ChangeView();
	virtual void RefreshSpringArm();
	virtual void RefreshCurViewType();

	virtual void OnEquipWeaponSuccess(AWeapon* Weapon) override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CAMERA, meta = (AllowPrivateAccess = "true"))
		EViewType ViewType = EViewType::FIRST_PERSON;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Arm, meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* ArmMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Arm, meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* ArmWeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interaction, meta = (AllowPrivateAccess = "true"))
		AWeapon* CanInteractWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interaction, meta = (AllowPrivateAccess = "true"))
		float InteractionDistance = 225.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interaction, meta = (AllowPrivateAccess = "true"))
		UInteractionWidget* InteractionWidget;

	TArray<UPrimitiveComponent*> ShowOnlyFirstPerson;
	TArray<UPrimitiveComponent*> ShowOnlyThirdPerson;
	FVector SpringArmBasicLocation;
	FVector SpringArmAdditiveLocation;
};
