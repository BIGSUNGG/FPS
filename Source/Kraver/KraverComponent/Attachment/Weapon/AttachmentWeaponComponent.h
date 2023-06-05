// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/KraverComponent/Attachment/AttachmentComponent.h"
#include "AttachmentWeaponComponent.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API UAttachmentWeaponComponent : public UAttachmentComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAttachmentWeaponComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void OnMakeWeaponPrimitiveInfoDeleEvent(FWeaponPrimitiveInfo& WeaponPrimitiveInfo);
	
protected:
	class AWeapon* OwnerWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|First person", meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* ScopeMesh;
};
