// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include Weapon_h
#include "Gun.generated.h"

/**
 * 
 */

UCLASS()
class KRAVER_API AGun : public AWeapon
{
	GENERATED_BODY()
	friend class USingleReloadComponent;
	friend class UWeaponReloadComponent;

public:
	AGun();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;


protected:
	// Delegate
	virtual void Attack() override;
	virtual void OnServer_ImpactBullet(FVector ImpactPos);

	// Rpc
	UFUNCTION(Server, Reliable)
	virtual void Server_Fire();

	virtual void Multicast_Attack_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_ImpactBullet(FVector ImpactPos);

	// Func
	virtual void IncreaseRecoil(); // 반동 추가
	virtual void IncreaseSpread(float InValue); // 스프레드 반동 늘이기
	virtual void DecreaseSpread(float InValue); // 스프레드 반동 줄이기

	virtual void Fire();
	virtual void FireBullet(); // 총을 발사할 때 호출 (공격 범위에 있는적을 트레이스할때 사용, 여러번 호출 가능)
	virtual void FireEvent(); // 총을 발사한 후 이벤트
	virtual void ImpactBulletEvent(FVector ImpactPos); // 총알이 Block되었을때 호출

public:
	// Getter Setter
	virtual bool CanAttack() override;
	virtual bool CanSubAttack() override;
	virtual bool ShouldApplySpread() { return !IsSubAttacking; }
	virtual float CalculateCurSpread() { return CurBulletSpread + AdditiveSpreadInAir + AdditiveSpreadPerSpeed; }
	int32 GetCurAmmo() { return CurAmmo; }
	int32 GetMaxAmmo() { return MaxAmmo; }
	int32 GetTotalAmmo() { return TotalAmmo; }

public:
	FAttackDele OnFireBullet; // 총알을 발사할 때마다 호출 (여러번 호출가능)
	FAttackDele OnAfterFire; // 총알을 발사한 후 호출 (한번만 호출)

protected:
	// Component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ads", Meta = (AllowPrivateAccess = true))
	class UWeaponAdsComponent* AdsComponent;

	// Sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Sound", meta = (AllowPrivateAccess = "true"))
	class USoundCue* ImpactSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Effect", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraSystem> ImpactEffect; // Bullet이 Block되었을때 실행되는 이펙트

	// Ammo
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Ammo", meta = (AllowPrivateAccess = "true"))
	int32 MaxAmmo = 10.f; // 최대 총알 갯수
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Ammo", meta = (AllowPrivateAccess = "true"))
	int32 CurAmmo = 10.f; // 현재 총알 갯수
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Ammo", meta = (AllowPrivateAccess = "true"))
	int32 TotalAmmo = 30.f; // 총 총알 갯수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Ammo", meta = (AllowPrivateAccess = "true"))
	bool bInfinityAmmo = false; // 발사할 때 총알을 소비할지 

	// Recoil
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Spread", meta = (AllowPrivateAccess = "true"))
	float MinSpread; // 발사에 인한 최소 스프레드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Spread", meta = (AllowPrivateAccess = "true"))
	float MaxSpread; // 발사에 인한 최대 스프레드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Spread", meta = (AllowPrivateAccess = "true"))
	float SpreadPerTime; // 발사 중 얼마나 빠르게 스프레드가 늘어날지 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Spread", meta = (AllowPrivateAccess = "true"))
	float SpreadForceBack; // 발사 중이 아닐때 얼마나 빠르게 스프레드가 줄어들지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Spread", meta = (AllowPrivateAccess = "true"))
	float SpreadInAir; // 공중에서 스프레드가 얼마나 늘어날지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Spread", meta = (AllowPrivateAccess = "true"))
	float SpreadPerSpeed; // 속도에 따라 스프레드가 얼마나 늘어날지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Spread", meta = (AllowPrivateAccess = "true"))
	float SpreadMaxSpeed; // 속도에 따라 늘어나는 스프레드가 어느 속도까지 늘어날지

	float CurBulletSpread; // 발사에 의한 스프레드
	float AdditiveSpreadInAir; // 공중에 있으면서 생긴 스프레드
	float AdditiveSpreadPerSpeed; // 속도에 따른 스프레드

	float TargetRecoilPitch; // 현재 세로 반동
	float TargetRecoilYaw; // 현재 가로 반동

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Recoil", meta = (AllowPrivateAccess = "true"))
	float MaxRecoilPitch; // 세로 반동 최대치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Recoil", meta = (AllowPrivateAccess = "true"))
	float MinRecoilPitch; // 세로 반동 최소치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Recoil", meta = (AllowPrivateAccess = "true"))
	float MaxRecoilYaw; // 가로 반동 최대치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Recoil", meta = (AllowPrivateAccess = "true"))
	float MinRecoilYaw; // 가로 반동 최소치
};
