// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Actor/Weapon/Weapon.h"
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
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

	virtual bool RefillAmmo(); // CurAmmo를 보충함

protected:
	virtual void ShowFireEffect(); // FireEffect를 실행하는 함수
	virtual void AddRecoil();
	virtual void FireBullet();

protected:
	// Delegate
	virtual void OnAttackEvent() override;
	virtual void OnMakeNewPrimitiveInfoEvent(int Index) override;

	// Rpc
	UFUNCTION(Server, Reliable)
		virtual void Server_ShowFireEffect();
	UFUNCTION(NetMulticast, Reliable)
		virtual void Multicast_ShowFireEffect();
	UFUNCTION(Server, Reliable)
		virtual void Server_SpawnImpactEffect(FVector ImpactPos);
	UFUNCTION(NetMulticast, Reliable)
		virtual void Multicast_SpawnImpactEffect(FVector ImpactPos);
public:
	// Getter Setter
	virtual bool GetbApplySpread() { return !IsSubAttacking; }
	int32 GetCurAmmo() { return CurAmmo; }
	int32 GetMaxAmmo() { return MaxAmmo; }
	int32 GetTotalAmmo() { return TotalAmmo; }
	float CalculateCurSpread() { return CurBulletSpread + AdditiveSpreadInAir + AdditiveSpreadPerSpeed; }

	void AddSpread(float Spread);
protected:
	// Additive
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Effect", meta = (AllowPrivateAccess = "true"))
		UNiagaraComponent* ImpactEffect; // Bullet이 Block되었을때 실행되는 이펙트

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
