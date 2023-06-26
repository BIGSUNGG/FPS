// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Component/Skill/Weapon/WeaponComponent.h"
#include "WeaponReloadComponent.generated.h"

/**
 * 
 */
 class AGun;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAVER_API UWeaponReloadComponent : public UWeaponComponent
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void OnAddOnDelegateEvent(UObject* Object) override;
	virtual void OnRemoveOnDelegateEvent(UObject* Object) override;
	// Delegate
	UFUNCTION()
		virtual void OnSkillFirstEvent();
	UFUNCTION()
		virtual void OnRefiilAmmoEvent();

	virtual void OnBeforeAttackEvent() override;
public:
	bool GetCanReload();

protected:
	AGun* OwnerGun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Montage", Meta = (AllowPrivateAccess = true))
		UAnimMontage* ReloadMontageTpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Montage", Meta = (AllowPrivateAccess = true))
		UAnimMontage* ReloadMontageFpp;
};
