// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"

class AWeapon;
class ACreature;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAVER_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Delegate
	UFUNCTION()
		virtual void OnAddOnDelegateEvent(UObject* Objcet);
	UFUNCTION()
		virtual void OnRemoveOnDelegateEvent(UObject* Objcet);
	UFUNCTION()
		virtual void OnBeforeAttackEvent();

public:
	FPlayMontageDele OnAttackDele;

protected:
	AWeapon* OwnerWeapon = nullptr;
	ACreature* OwnerCreature = nullptr;
};
