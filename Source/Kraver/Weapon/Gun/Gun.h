// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Kraver/Weapon/Weapon.h"
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

protected:
	virtual void Attack() override;
	virtual void ShowFireEffect();
	UFUNCTION(Server, Reliable)
		virtual void Server_ShowFireEffect();
	UFUNCTION(NetMulticast, Reliable)
		virtual void Multicast_ShowFireEffect();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireEffect", meta = (AllowPrivateAccess = "true"))
		UParticleSystem* FireEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireEffect", meta = (AllowPrivateAccess = "true"))
		FName FireEffectSocketName = "SOCKET_Muzzle";

};
