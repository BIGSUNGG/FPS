// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Kraver/Weapon/Weapon.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Gun.generated.h"

/**
 * 
 */

UCLASS()
class KRAVER_API AGun : public AWeapon
{
	GENERATED_BODY()
	
public:
	AGun();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual int32 AddAdditiveWeaponMesh(USkeletalMeshComponent* Mesh) override;
protected:
	virtual void Attack() override;
	virtual void ShowFireEffect();
	UFUNCTION(Server, Reliable)
		virtual void Server_ShowFireEffect();
	UFUNCTION(NetMulticast, Reliable)
		virtual void Multicast_ShowFireEffect();

public:
	UNiagaraComponent* GetFireEffect() { return FireEffect; }
	TArray<UNiagaraComponent*> GetAdditiveFireEffect() { return AdditiveFireEffect; }
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireEffect", meta = (AllowPrivateAccess = "true"))
		UNiagaraComponent* FireEffect;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Additive", meta = (AllowPrivateAccess = "true"))
		TArray<UNiagaraComponent*> AdditiveFireEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireEffect", meta = (AllowPrivateAccess = "true"))
		FName FireEffectSocketName = "SOCKET_Muzzle";

};
