// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "Kraver/Character/Creature/Creature.h"

AGun::AGun() : Super()
{
	WeaponType = EWeaponType::GUN;
	
	ImpactEffect = CreateDefaultSubobject<UNiagaraComponent>("ImpactEffect");
	ImpactEffect->bAutoActivate = false;

}

void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
			AdditiveSpreadInAir = FMath::FInterpTo(AdditiveSpreadInAir, 0, DeltaTime, 40);

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
}

void AGun::BeginPlay()
{
	Super::BeginPlay();
	CurBulletSpread = MinSpread;

}

void AGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGun, MaxAmmo);
	DOREPLIFETIME(AGun, CurAmmo);
	DOREPLIFETIME(AGun, TotalAmmo);
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
		Server_SetCurAmmo(CurAmmo);
		Server_SetTotalAmmo(TotalAmmo);
	}
	OnAttackEndEvent();
	return true;
}

void AGun::ShowFireEffect()
{
	if(HasAuthority())
		Multicast_ShowFireEffect();
	else
		Server_ShowFireEffect();
}

void AGun::AddRecoil()
{
	TargetRecoilPitch += FMath::RandRange(MinRecoilPitch,MaxRecoilPitch);
	TargetRecoilYaw += FMath::RandRange(MinRecoilYaw, MaxRecoilYaw);
}

void AGun::FireBullet()
{
	OnFire.Broadcast();
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
		if (!bInfinityAmmo)
		{
			--CurAmmo;
			if (!HasAuthority())
				Server_SetCurAmmo(CurAmmo);
		}

		FireBullet();
		AddRecoil();
		ShowFireEffect();
		OnPlayTppMontage.Broadcast(AttackMontageTpp, 1.f);
		OnPlayFppMontage.Broadcast(AttackMontageFpp, 1.f);
	}
	else
	{
		OnAttackEndEvent();
	}

	Super::OnAttackEvent();
}

void AGun::Server_SetTotalAmmo_Implementation(int32 Ammo)
{
	TotalAmmo = Ammo;
}

void AGun::Server_SetCurAmmo_Implementation(int32 Ammo)
{
	CurAmmo = Ammo;
}

void AGun::Server_SetMaxAmmo_Implementation(int32 Ammo)
{
	MaxAmmo = Ammo;
}

void AGun::Server_ShowFireEffect_Implementation()
{
	Multicast_ShowFireEffect();
}

void AGun::Multicast_ShowFireEffect_Implementation()
{	
	if(!WeaponPrimitiveInfo.Contains("FireEffect"))
		return;

	WeaponPrimitiveInfo["FireEffect"]->Activate(true);
	WeaponFppPrimitiveInfo["FireEffect"]->Activate(true);
}

void AGun::Server_SpawnImpactEffect_Implementation(FVector ImpactPos)
{
	Multicast_SpawnImpactEffect(ImpactPos);
}

void AGun::Multicast_SpawnImpactEffect_Implementation(FVector ImpactPos)
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactEffect->GetAsset(), ImpactPos);
}

void AGun::AddSpread(float Spread)
{
	CurBulletSpread += Spread;
	if(CurBulletSpread > MaxSpread)
		CurBulletSpread = MaxSpread;
}
