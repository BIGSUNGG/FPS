// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include WeaponReloadComponent_h
#include "SingleReloadComponent.generated.h"

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAVER_API USingleReloadComponent : public UWeaponReloadComponent
{
	GENERATED_BODY()
	
protected:
	// Delegate
	virtual void OnAddOnDelegateEvent(UObject* Object) override;
	virtual void OnRemoveOnDelegateEvent(UObject* Object) override;

	UFUNCTION()
	virtual void OnReload_OpenFinishEvent();
	UFUNCTION()
	virtual void OnReload_InsertFinishEvent();

	// Function
	virtual void ReloadInstert();
	virtual void ReloadClose();

public:
	virtual bool IsReloading() override;

protected:
	// Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", Meta = (AllowPrivateAccess = true))
	UAnimMontage* ReloadInsertMontageTpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", Meta = (AllowPrivateAccess = true))
	UAnimMontage* ReloadInsertMontageFpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", Meta = (AllowPrivateAccess = true))
	UAnimMontage* ReloadCloseMontageTpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", Meta = (AllowPrivateAccess = true))
	UAnimMontage* ReloadCloseMontageFpp;

	// Sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", Meta = (AllowPrivateAccess = true))
	USoundCue* ReloadInsertSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", Meta = (AllowPrivateAccess = true))
	USoundCue* ReloadCloseSound;
};
