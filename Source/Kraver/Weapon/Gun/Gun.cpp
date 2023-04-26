// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "Components/DecalComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kraver/Creature/Creature.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"

AGun::AGun() : AWeapon()
{
	WeaponType = EWeaponType::GUN;

	FireEffect = CreateDefaultSubobject<UNiagaraComponent>("FireEffect");
	FireEffect->SetupAttachment(WeaponMesh, FireEffectSocketName);
	FireEffect->bAutoActivate = false;
	
	ImpactEffect = CreateDefaultSubobject<UNiagaraComponent>("ImpactEffect");
	ImpactEffect->bAutoActivate = false;
}

void AGun::Tick(float DeltaTime)
{
	AWeapon::Tick(DeltaTime);

	if (OwnerCreature)
	{
		if (IsAttacking == false)
		{
			CurBulletSpread -= SpreadForceBack * DeltaTime;
			if (CurBulletSpread < 0)
				CurBulletSpread = 0;
		}

		if (AttackDelay > 0)
		{
			AddSpread(SpreadPerTime * DeltaTime);
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

void AGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	AWeapon::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGun, MaxAmmo);
	DOREPLIFETIME(AGun, CurAmmo);
	DOREPLIFETIME(AGun, TotalAmmo);
	DOREPLIFETIME(AGun, BulletDistance);
	DOREPLIFETIME(AGun, BulletRadius);
}

void AGun::PostInitializeComponents()
{
	AWeapon::PostInitializeComponents();

}

int32 AGun::MakeAdditiveWeaponMesh()
{
	int32 Index = AWeapon::MakeAdditiveWeaponMesh();
	UNiagaraComponent* MakeFireEffect = NewObject<UNiagaraComponent>(this, UNiagaraComponent::StaticClass(), TEXT("Additive Fire Effect"));
	MakeFireEffect->bAutoActivate = false;
	MakeFireEffect->RegisterComponent();
	MakeFireEffect->SetAsset(FireEffect->GetAsset());
	MakeFireEffect->AttachToComponent(AdditiveWeaponMesh[Index], FAttachmentTransformRules::SnapToTargetIncludingScale, FireEffectSocketName);
	MakeFireEffect->SetRelativeRotation(FireEffect->GetRelativeRotation());
	AdditiveFireEffect.Push(MakeFireEffect);
	return Index;
}

int32 AGun::RemoveAdditiveWeaponMesh(USkeletalMeshComponent* Mesh)
{
	int32 Index = AWeapon::RemoveAdditiveWeaponMesh(Mesh);
	if (Index == -1)
		return Index;

	AdditiveFireEffect[Index]->DestroyComponent();
	AdditiveFireEffect.RemoveAt(Index);
	return Index;
}

bool AGun::RefillAmmo()
{
	AWeapon::RefillAmmo();

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
	AttackEndEvent();
	return true;
}

void AGun::Attack()
{
	if (!IsAttacking)
		return;

	if (OwnerCreature == nullptr)
	{
		KR_LOG(Error, TEXT("OwnerCreature is nullptr"),*GetName());
		return;
	}

	if(CurAmmo > 0)
	{ 
		AWeapon::Attack();

		--CurAmmo;		
		if(!HasAuthority())
			Server_SetCurAmmo(CurAmmo);

		float SpreadX;
		float SpreadY;
		float SpreadZ;
		if (IsSubAttacking)
		{
			SpreadX = 0.f;
			SpreadY = 0.f;
			SpreadZ = 0.f;
		}
		else
		{
			float Spread = CalculateCurSpread();
			SpreadX = FMath::RandRange(-Spread, Spread);
			SpreadY = FMath::RandRange(-Spread, Spread);
			SpreadZ = FMath::RandRange(-Spread, Spread);
		}

		TArray<FHitResult> BulletHitResults = CalculateFireHit(ECC_BULLET,FVector(SpreadX,SpreadY,SpreadZ));

		for (auto& Result : BulletHitResults)
		{
			auto Creature = Cast<ACreature>(Result.GetActor());
			if (IsValid(Result.GetActor()))
			{
				FPointDamageEvent damageEvent;
				damageEvent.HitInfo = Result;
				damageEvent.ShotDirection = OwnerCreature->GetCamera()->GetForwardVector();
				OwnerCreature->CombatComponent->GiveDamage(Result.GetActor(), AttackDamage, damageEvent, OwnerCreature->GetController(), this);
				if (Result.bBlockingHit)
				{
					FVector ImpaceEffectPos = Result.ImpactPoint - OwnerCreature->GetCamera()->GetForwardVector() * 15.f;
					OwnerCreature->RpcComponent->SpawnNiagaraAtLocation(GetWorld(), ImpactEffect->GetAsset(), ImpaceEffectPos);
				}
			}
		}
		AddRecoil();
		ShowFireEffect();
	}
	else
		AttackEndEvent();
}

TArray<FHitResult> AGun::CalculateFireHit(ECollisionChannel BulletChannel, FVector Spread /*= FVector(0,0,0)*/)
{
	FVector EndPoint = OwnerCreature->GetCamera()->GetForwardVector() + Spread;
	EndPoint.Normalize();
	EndPoint *= BulletDistance;
	EndPoint += OwnerCreature->GetCamera()->GetComponentLocation();

	TArray<FHitResult> BulletHitResults;
	FCollisionQueryParams BulletParams(NAME_None, false, OwnerCreature);
	bool bResult = GetWorld()->SweepMultiByChannel(
		BulletHitResults,
		OwnerCreature->GetCamera()->GetComponentLocation(),
		EndPoint,
		FQuat::Identity,
		BulletChannel,
		FCollisionShape::MakeSphere(BulletRadius),
		BulletParams
	);

	return BulletHitResults;
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
	FireEffect->Activate(true);
	for (auto& TempFireEffect : AdditiveFireEffect)
	{
		if (TempFireEffect)
		{
			TempFireEffect->Activate(true);
		}
	}
}

bool AGun::GetCanReload()
{
	if (CurAmmo == MaxAmmo)
		return false;

	if (TotalAmmo == 0)
		return false;

	return true;
}

void AGun::AddSpread(float Spread)
{
	CurBulletSpread += Spread;
	if(CurBulletSpread > MaxSpread)
		CurBulletSpread = MaxSpread;
}
