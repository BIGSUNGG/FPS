// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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

	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

	virtual int32 MakeAdditiveWeaponMesh() override;
	virtual int32 RemoveAdditiveWeaponMesh(USkeletalMeshComponent* Mesh) override;

	virtual bool RefillAmmo(); // CurAmmo를 보충함
protected:
	TArray<FHitResult>  CalculateFireHit(ECollisionChannel BulletChannel, FVector Spread = FVector(0,0,0));
	virtual void ShowFireEffect(); // FireEffect를 실행하는 함수
	virtual void AddRecoil();

protected:
	// Delegate
	virtual void OnAttackEvent() override;

	// Rpc
	UFUNCTION(Server, Reliable)
		virtual void Server_ShowFireEffect();
	UFUNCTION(NetMulticast, Reliable)
		virtual void Multicast_ShowFireEffect();

public:
	// Getter Setter
	int32 GetCurAmmo() { return CurAmmo; }
	int32 GetMaxAmmo() { return MaxAmmo; }
	int32 GetTotalAmmo() { return TotalAmmo; }
	float CalculateCurSpread() { return CurBulletSpread + AdditiveSpreadInAir + AdditiveSpreadPerSpeed; }
	UNiagaraComponent* GetFireEffect() { return FireEffect; }
	TArray<UNiagaraComponent*> GetAdditiveFireEffect() { return AdditiveFireEffect; }

	void AddSpread(float Spread);
protected:
	// Additive
	TArray<UNiagaraComponent*> AdditiveFireEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Effect", meta = (AllowPrivateAccess = "true"))
		UNiagaraComponent* FireEffect; // 공격하였을때 실행되는 이펙트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Effect", meta = (AllowPrivateAccess = "true"))
		UNiagaraComponent* ImpactEffect; // Bullet이 Block되었을때 실행되는 이펙트

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Effect", meta = (AllowPrivateAccess = "true"))
		FName FireEffectSocketName = "SOCKET_Muzzle"; // FireEffect가 붙을 WeaponMesh의 소켓이름


	// Ammo
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Ammo", meta = (AllowPrivateAccess = "true"))
		int32 MaxAmmo = 10.f; // 최대 총알 갯수
	UFUNCTION(Server, Reliable)
		void Server_SetMaxAmmo(int32 Ammo);
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Ammo", meta = (AllowPrivateAccess = "true"))
		int32 CurAmmo = 10.f; // 현재 총알 갯수
	UFUNCTION(Server, Reliable)
		void Server_SetCurAmmo(int32 Ammo);
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Ammo", meta = (AllowPrivateAccess = "true"))
		int32 TotalAmmo = 30.f; // 총 총알 갯수
	UFUNCTION(Server, Reliable)
		void Server_SetTotalAmmo(int32 Ammo);
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Ammo", meta = (AllowPrivateAccess = "true"))
		float BulletDistance = 10000.f; // 총알 길이
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Ammo", meta = (AllowPrivateAccess = "true"))
		float BulletRadius = 0.f; // 총알 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Ammo", meta = (AllowPrivateAccess = "true"))
		bool bInfinityAmmo = false;

	// Recoil
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Spread", meta = (AllowPrivateAccess = "true"))
		float MinSpread;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Spread", meta = (AllowPrivateAccess = "true"))
		float MaxSpread;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Spread", meta = (AllowPrivateAccess = "true"))
		float SpreadPerTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Spread", meta = (AllowPrivateAccess = "true"))
		float SpreadForceBack;		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Spread", meta = (AllowPrivateAccess = "true"))
		float SpreadInAir;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Spread", meta = (AllowPrivateAccess = "true"))
		float SpreadPerSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Spread", meta = (AllowPrivateAccess = "true"))
		float SpreadMaxSpeed;

	float CurBulletSpread;
	float AdditiveSpreadInAir;
	float AdditiveSpreadPerSpeed;

	float TargetRecoilPitch;
	float TargetRecoilYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Recoil", meta = (AllowPrivateAccess = "true"))
		float MaxRecoilPitch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Recoil", meta = (AllowPrivateAccess = "true"))
		float MinRecoilPitch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Recoil", meta = (AllowPrivateAccess = "true"))
		float MaxRecoilYaw;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Recoil", meta = (AllowPrivateAccess = "true"))
		float MinRecoilYaw;
};
