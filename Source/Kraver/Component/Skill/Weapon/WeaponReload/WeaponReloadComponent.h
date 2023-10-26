// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include WeaponComponent_h
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

	// Rpc
	UFUNCTION(Server, Reliable)
	virtual void Server_ReloadStart();
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_ReloadStart();
	
	UFUNCTION(Server, Reliable)
	virtual void Server_RefillAmmo();

	// Delegate
	UFUNCTION()
	virtual void OnSkillFirstEvent(); // R버튼 눌렸을때 호출
	UFUNCTION()
	virtual void OnReload_Insert_MagazineEvent();
	UFUNCTION()
	virtual void OnFireEvent(); // Gun이 총을 발사했을때 호출

	// Func
	virtual void ReloadStart(); // 재장전 시작
	virtual void ReloadStartEvent();

	virtual bool RefillAmmo(); // CurAmmo를 보충함

public:
	virtual bool IsReloading();

	bool CanReload();

	FORCEINLINE bool GetbReloadWhileSprint() { return bReloadWhileSprint; }

	FORCEINLINE const FVector& GetGrabRelativeLocation() { return GrabRelativeLocation; }
	FORCEINLINE const FRotator& GetGrabRelativeRotation() { return GrabRelativeRotation; }

	FORCEINLINE const UAnimMontage* GetReloadMontageTpp() { return ReloadMontageTpp; }
	FORCEINLINE const UAnimMontage* GetReloadMontageFpp() { return ReloadMontageFpp; }

protected:
	AGun* OwnerGun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reload", Meta = (AllowPrivateAccess = true))
	bool bReloadWhileSprint = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attachment", Meta = (AllowPrivateAccess = true))
	FVector GrabRelativeLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attachment", Meta = (AllowPrivateAccess = true))
	FRotator GrabRelativeRotation;

	// Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", Meta = (AllowPrivateAccess = true))
	UAnimMontage* ReloadMontageTpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", Meta = (AllowPrivateAccess = true))
	UAnimMontage* ReloadMontageFpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", Meta = (AllowPrivateAccess = true))
	UAnimMontage* ReloadMontageWep;

	// Sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", Meta = (AllowPrivateAccess = true))
	class USoundCue* ReloadSound;

};
