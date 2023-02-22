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

	virtual bool RefillAmmo();
protected:
	virtual void Attack() override;
	virtual void ShowFireEffect(); // FireEffect를 실행하는 함수

protected:
	UFUNCTION(Server, Reliable)
		virtual void Server_ShowFireEffect();
	UFUNCTION(NetMulticast, Reliable)
		virtual void Multicast_ShowFireEffect();

public:
	// Getter Setter
	UNiagaraComponent* GetFireEffect() { return FireEffect; }
	TArray<UNiagaraComponent*> GetAdditiveFireEffect() { return AdditiveFireEffect; }
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Additive", meta = (AllowPrivateAccess = "true"))
		TArray<UNiagaraComponent*> AdditiveFireEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireEffect", meta = (AllowPrivateAccess = "true"))
		UNiagaraComponent* FireEffect; // 공격하였을때 실행되는 이펙트

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireEffect", meta = (AllowPrivateAccess = "true"))
		FName FireEffectSocketName = "SOCKET_Muzzle"; // FireEffect가 붙을 WeaponMesh의 소켓이름

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		int32 MaxAmmo = 10.f; // 최대 총알 갯수
	UFUNCTION(Server, Reliable)
		void Server_SetMaxAmmo(int32 Ammo);
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		int32 CurAmmo = 10.f; // 현재 총알 갯수
	UFUNCTION(Server, Reliable)
		void Server_SetCurAmmo(int32 Ammo);
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		int32 TotalAmmo = 30.f; // 총 총알 갯수
	UFUNCTION(Server, Reliable)
		void Server_SetTotalAmmo(int32 Ammo);
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		float BulletDistance = 10000.f; // 총알 길이
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		float BulletRadius = 0.f; // 총알 범위
};
