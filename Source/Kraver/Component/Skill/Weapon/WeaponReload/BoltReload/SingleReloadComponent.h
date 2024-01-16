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
	virtual void OnLocal_AddOnDelegateEvent(UObject* Object) override;
	virtual void OnLocal_RemoveOnDelegateEvent(UObject* Object) override;

	UFUNCTION()
	virtual void OnReload_OpenFinishEvent();
	UFUNCTION()
	virtual void OnReload_InsertFinishEvent();

	// Rpc
	UFUNCTION(Server, Reliable)
	virtual void Server_ReloadInstert();
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_ReloadInstert();

	UFUNCTION(Server, Reliable)
	virtual void Server_ReloadClose();
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_ReloadClose();

	// Function
	virtual void ReloadInstert();
	virtual void ReloadInstertEvent();

	virtual void ReloadClose();
	virtual void ReloadCloseEvent();

	virtual bool RefillAmmo() override;
public:
	virtual bool IsReloading() override;

protected:
	// Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", Meta = (AllowPrivateAccess = true))
	UAnimMontage* ReloadInsertMontageTpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", Meta = (AllowPrivateAccess = true))
	UAnimMontage* ReloadInsertMontageFpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", Meta = (AllowPrivateAccess = true))
	UAnimMontage* ReloadInsertMontageWep;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", Meta = (AllowPrivateAccess = true))
	UAnimMontage* ReloadCloseMontageTpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", Meta = (AllowPrivateAccess = true))
	UAnimMontage* ReloadCloseMontageFpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", Meta = (AllowPrivateAccess = true))
	UAnimMontage* ReloadCloseMontageWep;

	// Sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", Meta = (AllowPrivateAccess = true))
	USoundCue* ReloadInsertSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", Meta = (AllowPrivateAccess = true))
	USoundCue* ReloadCloseSound;
};
