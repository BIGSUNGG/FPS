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
	virtual void PostInitializeComponents() override;

	virtual int32 AddAdditiveWeaponMesh(USkeletalMeshComponent* Mesh) override;
	virtual int32 RemoveAdditiveWeaponMesh(USkeletalMeshComponent* Mesh) override;

	virtual bool Reload();
protected:
	virtual void Attack() override;
	virtual void ShowFireEffect();

protected:
	UFUNCTION(Server, Reliable)
		virtual void Server_ShowFireEffect();
	UFUNCTION(NetMulticast, Reliable)
		virtual void Multicast_ShowFireEffect();

public:
	UNiagaraComponent* GetFireEffect() { return FireEffect; }
	TArray<UNiagaraComponent*> GetAdditiveFireEffect() { return AdditiveFireEffect; }
protected:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Additive", meta = (AllowPrivateAccess = "true"))
		TArray<UNiagaraComponent*> AdditiveFireEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireEffect", meta = (AllowPrivateAccess = "true"))
		UNiagaraComponent* FireEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireEffect", meta = (AllowPrivateAccess = "true"))
		FName FireEffectSocketName = "SOCKET_Muzzle";

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		int32 MaxAmmo = 10.f;
	UFUNCTION(Server, Reliable)
		void Server_SetMaxAmmo(int32 Ammo);
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		int32 CurAmmo = 10.f;
	UFUNCTION(Server, Reliable)
		void Server_SetCurAmmo(int32 Ammo);
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		int32 TotalAmmo = 30.f;
	UFUNCTION(Server, Reliable)
		void Server_SetTotalAmmo(int32 Ammo);
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		float BulletDistance = 10000.f;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		float BulletRadius = 0.f;
};
