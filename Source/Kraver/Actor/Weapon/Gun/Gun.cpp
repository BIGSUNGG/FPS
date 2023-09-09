// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include Creature_h
#include AttachmentSilencerComponent_h
#include WeaponReloadComponent_h

AGun::AGun() : Super()
{
	WeaponType = EWeaponType::GUN;
	
	bAttackWhileSprint = false;
	bSubAttackWhileSprint = false;
}

void AGun::Tick(float DeltaTime)
{
	if (OwnerCreature)
	{
		if (CurAttackDelay > 0)
		{
			AddSpread(SpreadPerTime * DeltaTime);
		}
		else
		{
			CurBulletSpread -= SpreadForceBack * DeltaTime;
			if (CurBulletSpread < MinSpread)
				CurBulletSpread = MinSpread;
		}

		if (OwnerCreature->GetMovementComponent()->IsFalling())
			AdditiveSpreadInAir = FMath::FInterpTo(AdditiveSpreadInAir, SpreadInAir, DeltaTime, 10);
		else
			AdditiveSpreadInAir = FMath::FInterpTo(AdditiveSpreadInAir, 0, DeltaTime, 10);

		FVector Velocity = OwnerCreature->GetVelocity();
		Velocity.Z = 0.f;
		float Speed =  Velocity.Size();
		if(Speed > SpreadMaxSpeed)
			Speed = SpreadMaxSpeed;

		AdditiveSpreadPerSpeed = Speed * SpreadPerSpeed;

		float TempRecoilPitch = FMath::FInterpTo(TargetRecoilPitch, 0.f, DeltaTime, 15.f);
		float TempRecoilYaw = FMath::FInterpTo(TargetRecoilYaw, 0.f, DeltaTime, 15.f);

		float AddRecoilPitch = TargetRecoilPitch - TempRecoilPitch;
		float AddRecoilYaw = TargetRecoilYaw - TempRecoilYaw;

		OwnerCreature->AddControllerPitchInput(AddRecoilPitch);
		OwnerCreature->AddControllerYawInput(AddRecoilYaw);

		TargetRecoilPitch -= AddRecoilPitch;
		TargetRecoilYaw -= AddRecoilYaw;
	}

	Super::Tick(DeltaTime);
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

bool AGun::RefillAmmo()
{
	if(CurAmmo == MaxAmmo)
		return false;

	int32 NeedAmmo = MaxAmmo - CurAmmo;
	if (TotalAmmo >= NeedAmmo)
	{
		TotalAmmo -= NeedAmmo;
		CurAmmo = MaxAmmo;
	}
	else
	{
		CurAmmo += TotalAmmo;
		TotalAmmo = 0;
	}
	if (!HasAuthority())	
	{
		Server_RefillAmmo();
	}

	return true;
}

void AGun::OnAttackEvent()
{
	if (!IsAttacking)
		return;

	if (OwnerCreature == nullptr)
	{
		KR_LOG(Error, TEXT("OwnerCreature is nullptr"), *GetName());
		return;
	}

	if (CurAmmo > 0)
	{
		if (!IS_SERVER() && !bInfinityAmmo)
			--CurAmmo;

		FireBullet();
		FireEvent();
		AddRecoil();

		Super::OnAttackEvent();
	}
	else
		OnAttackEndEvent();

}

void AGun::Server_OnAttackEvent_Implementation()
{
	Super::Server_OnAttackEvent_Implementation();

	--CurAmmo;
}

void AGun::Multicast_OnAttackEvent_Implementation()
{	
	Super::Multicast_OnAttackEvent_Implementation();

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

void AGun::Server_RefillAmmo_Implementation()
{
	RefillAmmo();
}

void AGun::Multicast_ImpactBullet_Implementation(FVector ImpactPos)
{
	if(OwnerCreature && OwnerCreature->IsLocallyControlled())
		return;

	ImpactBulletEvent(ImpactPos);
}

void AGun::AddRecoil()
{
	TargetRecoilPitch += FMath::RandRange(MinRecoilPitch, MaxRecoilPitch);
	TargetRecoilYaw += FMath::RandRange(MinRecoilYaw, MaxRecoilYaw);
}

void AGun::FireBullet()
{

	OnFire.Broadcast();
}

void AGun::FireEvent()
{
	OnPlayTppMontage.Broadcast(AttackMontageTpp, 1.f);
	OnPlayFppMontage.Broadcast(AttackMontageFpp, 1.f);

	if (AttackMontageWep)
	{
		if (GetTppWeaponMesh()->GetAnimInstance())
			GetTppWeaponMesh()->GetAnimInstance()->Montage_Play(AttackMontageWep);
	}

	UAttachmentSilencerComponent* Silencer = FindComponentByClass<UAttachmentSilencerComponent>();
	if ((!Silencer || Silencer->GetbFireEffect()) && WeaponTppPrimitiveInfo.Contains("FireEffect"))
	{
		WeaponTppPrimitiveInfo["FireEffect"]->Activate(true);
		WeaponFppPrimitiveInfo["FireEffect"]->Activate(true);
	}

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
	if (ImpactEffect)
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactEffect, ImpactPos);
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

	if(CurAmmo <= 0)
		return false;

	UWeaponReloadComponent* ReloadComp = this->GetComponentByClass<UWeaponReloadComponent>();
	if(ReloadComp && ReloadComp->IsReloading())
		return false;

	return true;
}

bool AGun::CanSubAttack()
{
	if (!Super::CanSubAttack())
		return false;

	UWeaponReloadComponent* ReloadComp = this->GetComponentByClass<UWeaponReloadComponent>();
	if (ReloadComp && ReloadComp->IsReloading())
		return false;

	return true;
}

void AGun::AddSpread(float Spread)
{
	CurBulletSpread += Spread;
	if(CurBulletSpread > MaxSpread)
		CurBulletSpread = MaxSpread;
}
