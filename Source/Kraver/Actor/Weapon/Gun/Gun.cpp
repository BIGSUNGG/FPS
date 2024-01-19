// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include Creature_h
#include AttachmentSilencerComponent_h
#include WeaponReloadComponent_h
#include WeaponAdsComponent_h
#include CreatureMovementComponent_h

AGun::AGun() : Super()
{
	AdsComponent = CreateDefaultSubobject<UWeaponAdsComponent>("AdsComponent");

	WeaponType = EWeaponType::GUN;
	
	bAttackWhileSprint = false;
	bSubAttackWhileSprint = false;
}

void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SpreadUpdate(DeltaTime);
	RecoilUpdate(DeltaTime);

	// 선입력 처리
	if (CurAttackDelay > 0.f)
	{
		CurAttackDelay -= DeltaTime;
		if (CurAttackDelay <= 0.f)
		{
			CurAttackDelay = 0.f;
			AttackDelayFinish();
		}
	}
}

void AGun::SpreadUpdate(float DeltaTime)
{
	if (OwnerCreature)
	{
		if (CurAttackDelay > 0 || (bIsAttacking && bAutomaticAttack)) // 발사중일 때 
			IncreaseSpread(SpreadPerTime * DeltaTime);
		else // 발사중이 아닐 때
			DecreaseSpread(SpreadForceBack * DeltaTime);

		if (OwnerCreature->CreatureMovementComponent->IsFalling()) // 공중에 떠있는지
			AdditiveSpreadInAir = FMath::FInterpTo(AdditiveSpreadInAir, SpreadInAir, DeltaTime, 10);
		else
			AdditiveSpreadInAir = FMath::FInterpTo(AdditiveSpreadInAir, 0, DeltaTime, 10);

		// 속도에 따른 스프레드
		FVector Velocity = OwnerCreature->GetVelocity();
		Velocity.Z = 0.f;
		float Speed = Velocity.Size();
		if (Speed > SpreadMaxSpeed)
			Speed = SpreadMaxSpeed;

		AdditiveSpreadPerSpeed = Speed * SpreadPerSpeed;
	}
}

void AGun::RecoilUpdate(float DeltaTime)
{
	if (OwnerCreature && OwnerCreature->IsLocallyControlled())
	{
		float TempRecoilPitch = FMath::FInterpTo(TargetRecoilPitch, 0.f, DeltaTime, 15.f);
		float TempRecoilYaw = FMath::FInterpTo(TargetRecoilYaw, 0.f, DeltaTime, 15.f);

		float AddRecoilPitch = TargetRecoilPitch - TempRecoilPitch;
		float AddRecoilYaw = TargetRecoilYaw - TempRecoilYaw;

		OwnerCreature->AddControllerPitchInput(AddRecoilPitch);
		OwnerCreature->AddControllerYawInput(AddRecoilYaw);

		FRotator TargetRot = FRotator(TargetRecoilYaw, TargetRecoilYaw, TargetRecoilPitch);
		RecoilSwayRot = FMath::RInterpTo(RecoilSwayRot, TargetRot, DeltaTime, 3.f);

		TargetRecoilPitch -= AddRecoilPitch;
		TargetRecoilYaw -= AddRecoilYaw;
	}
}

void AGun::BeginPlay()
{
	Super::BeginPlay();
	CurBulletSpread = MinSpread;

}

void AGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AGun, MaxAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AGun, CurAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AGun, TotalAmmo, COND_OwnerOnly);
}

void AGun::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void AGun::OnLocalAttackStartEvent()
{
	if (CurAttackDelay > 0 && bCanFirstInputAttack) // 공격 딜레이 중이고 선입력이 공격이 가능하면
	{
		bFirstInputAttack = true;
		return;
	}

	if (bIsAttacking == true) // 이미 공격중이면 
		return;

	if (!CanAttack()) // 공격이 불가능하면
		return;

	Super::OnLocalAttackStartEvent();
	if (bCanAttack)
	{
		if (!bFirstAttackDelay) // 첫공격 딜레이가 없으면
			TryAttack();

		if (bAutomaticAttack) // 자동공격이 가능하면
			GetWorldTimerManager().SetTimer(AutomaticAttackHandle, this, &ThisClass::TryAttack, AttackDelay, true, AttackDelay);

		if (bFirstAttackDelay && !bAutomaticAttack) // 첫공격 딜레이도 있고 자동공격이 불가능하면
			GetWorldTimerManager().SetTimer(AutomaticAttackHandle, this, &ThisClass::TryAttack, AttackDelay, false, AttackDelay);

	}
}

void AGun::OnLocalAttackEndEvent()
{
	bFirstInputAttack = false; // 선입력 취소

	if (bIsAttacking == false)
		return;

	Super::OnLocalAttackEndEvent();

	GetWorldTimerManager().ClearTimer(AutomaticAttackHandle); // 자동공격 취소

}

void AGun::Multicast_Attack_Implementation()
{	
	Super::Multicast_Attack_Implementation();

	if(!OwnerCreature || !OwnerCreature->IsLocallyControlled())
		FireEvent();
}

void AGun::OnServer_ImpactBullet(FVector ImpactPos)
{
	if (!IS_SERVER())
	{
		KR_LOG(Error, TEXT("Called on client"));
		return;
	}

	Multicast_ImpactBullet(ImpactPos);
}

void AGun::OnServer_Fire()
{
	if (OwnerCreature->IsLocallyControlled() == false && bInfinityAmmo == false)
		--CurAmmo;

	OnServer_FireBullet(); // 총알 발사
}

void AGun::Multicast_ImpactBullet_Implementation(FVector ImpactPos)
{
	ImpactBulletEvent(ImpactPos);
}

void AGun::IncreaseRecoil()
{
	TargetRecoilPitch += FMath::RandRange(MinRecoilPitch, MaxRecoilPitch);
	TargetRecoilYaw += FMath::RandRange(MinRecoilYaw, MaxRecoilYaw);
}

void AGun::IncreaseSpread(float InValue)
{
	CurBulletSpread += InValue;
	if (CurBulletSpread > MaxSpread)
		CurBulletSpread = MaxSpread;
}

void AGun::DecreaseSpread(float InValue)
{
	CurBulletSpread -= InValue;
	if (CurBulletSpread < MinSpread)
		CurBulletSpread = MinSpread;
}

void AGun::Fire()
{
	if (!bInfinityAmmo)
		--CurAmmo;

	FireEvent(); // 발사 이벤트
	IncreaseRecoil(); // 반동추가

	OnAfterFire.Broadcast();
}

void AGun::OnServer_FireBullet()
{
	OnFireBullet.Broadcast();
}

void AGun::TryAttack()
{
	CurAttackDelay = AttackDelay;

	Super::TryAttack();
}

void AGun::Attack()
{
	if (!bIsAttacking) // 공격중이 아닌지
		return;

	if (OwnerCreature == nullptr)
	{
		KR_LOG(Error, TEXT("OwnerCreature is nullptr"), *GetName());
		return;
	}

	if (CurAmmo > 0) // 남아있는 총알이 있는지
	{
		Fire();
		Super::Attack();
	}
	else // 남아있는 총알이 없으면
		OnLocalAttackEndEvent(); // 공격 중지

}

void AGun::OnServer_Attack()
{
	ERROR_IF_CALLED_ON_CLIENT();
	Super::OnServer_Attack();

	OnServer_Fire();
}

void AGun::AttackDelayFinish()
{
	if (bCanFirstInputAttack && bFirstInputAttack == true) // 선입력이 입력되어있고 선입력이 공격이 가능할 때 
	{
		bFirstInputAttack = false;
		OnLocalAttackStartEvent(); // 공격 실행
	}
}

void AGun::FireEvent()
{
	// 공격 몽타주
	OnPlayTppMontage.Broadcast(AttackMontageTpp, 1.f);
	OnPlayFppMontage.Broadcast(AttackMontageFpp, 1.f);

	if (AttackMontageWep)
	{
		if (GetTppWeaponMesh()->GetAnimInstance())
			GetTppWeaponMesh()->GetAnimInstance()->Montage_Play(AttackMontageWep);
	}

	// 발사 이펙트
	UAttachmentSilencerComponent* Silencer = FindComponentByClass<UAttachmentSilencerComponent>();
	if ((!Silencer || Silencer->GetbFireEffect()) && WeaponTppPrimitiveInfo.Contains("FireEffect"))
	{
		WeaponTppPrimitiveInfo["FireEffect"]->Activate(true);
		WeaponFppPrimitiveInfo["FireEffect"]->Activate(true);
	}

	// 발사 소리
	if (AttackSound)
	{
		UGameplayStatics::PlaySoundAtLocation
		(
			this,
			Silencer ? Silencer->GetSilencerFireSound() : AttackSound,
			OwnerCreature->GetActorLocation()
		);
	}

}

void AGun::ImpactBulletEvent(FVector ImpactPos)
{
	// 명중 소리
	if (ImpactEffect)
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactEffect, ImpactPos);

	// 명중 이펙트
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ImpactSound,
			ImpactPos
		);
	}
}

bool AGun::CanAttack()
{
	if(!Super::CanAttack())
		return false;

	if(CurAmmo <= 0) // 남아있는 총알이 있는지
		return false;

	// 재장전 중인지
	UWeaponReloadComponent* ReloadComp = this->GetComponentByClass<UWeaponReloadComponent>();
	if(ReloadComp && ReloadComp->IsReloading())
		return false;

	return true;
}

bool AGun::CanSubAttack()
{
	if (!Super::CanSubAttack())
		return false;

	// 재장전 중인지
	UWeaponReloadComponent* ReloadComp = this->GetComponentByClass<UWeaponReloadComponent>();
	if (ReloadComp && ReloadComp->IsReloading())
		return false;

	return true;
}
