// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include AttachmentComponent_h
#include "AttachmentWeaponComponent.generated.h"

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
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
	// Delegate
	UFUNCTION()
		virtual void OnAddOnDelegateEvent(UObject* Object);
	UFUNCTION()
		virtual void OnRemoveOnDelegateEvent(UObject* Object);
protected:
	class AWeapon* OwnerWeapon;

};
