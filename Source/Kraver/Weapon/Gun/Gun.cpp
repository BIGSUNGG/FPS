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
}

void AGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	AWeapon::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGun, MaxAmmo);
	DOREPLIFETIME(AGun, CurAmmo);
	DOREPLIFETIME(AGun, TotalAmmo);
}

void AGun::PostInitializeComponents()
{
	AWeapon::PostInitializeComponents();

}

int32 AGun::AddAdditiveWeaponMesh(USkeletalMeshComponent* Mesh)
{
	int32 Index = AWeapon::AddAdditiveWeaponMesh(Mesh);
	UNiagaraComponent* TempFireEffect = NewObject<UNiagaraComponent>(this, UNiagaraComponent::StaticClass(), TEXT("Additive Fire Effect"));
	TempFireEffect->bAutoActivate = false;
	TempFireEffect->RegisterComponent();
	TempFireEffect->SetAsset(FireEffect->GetAsset());
	TempFireEffect->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FireEffectSocketName);
	TempFireEffect->SetRelativeRotation(FireEffect->GetRelativeRotation());
	AdditiveFireEffect.Push(TempFireEffect);
	return Index;
}

int32 AGun::RemoveAdditiveWeaponMesh(USkeletalMeshComponent* Mesh)
{
	int32 Index = AWeapon::RemoveAdditiveWeaponMesh(Mesh);
	AdditiveFireEffect[Index]->SetHiddenInGame(true);
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
	if(IsAttacking == false)
		return;

	if(CurAmmo > 0)
	{ 
		AWeapon::Attack();

		--CurAmmo;		
		if(!HasAuthority())
			Server_SetCurAmmo(CurAmmo);

		FHitResult BulletHitResult;
		FCollisionQueryParams BulletParams(NAME_None, false, OwnerCreature);
		bool bResult = GetWorld()->SweepSingleByChannel(
			BulletHitResult,
			OwnerCreature->GetCamera()->GetComponentLocation(),
			OwnerCreature->GetCamera()->GetComponentLocation() + OwnerCreature->GetCamera()->GetForwardVector() * BulletDistance,
			FQuat::Identity,
			ECollisionChannel::ECC_GameTraceChannel3,
			FCollisionShape::MakeSphere(BulletRadius),
			BulletParams
		);

		if (bResult && IsValid(BulletHitResult.GetActor()))
		{
			FPointDamageEvent damageEvent;
			damageEvent.HitInfo = BulletHitResult;
			damageEvent.ShotDirection = OwnerCreature->GetCamera()->GetForwardVector();
			OwnerCreature->CombatComponent->GiveDamage(BulletHitResult.GetActor(), AttackDamage, damageEvent, OwnerCreature->GetController(), this);
		}
		ShowFireEffect();
	}
	else
		AttackEndEvent();
}

void AGun::ShowFireEffect()
{
	if(HasAuthority())
		Multicast_ShowFireEffect();
	else
		Server_ShowFireEffect();
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
