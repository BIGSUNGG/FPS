// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "Components/DecalComponent.h"
#include "Net/UnrealNetwork.h"

AGun::AGun() : AWeapon()
{
	WeaponType = EWeaponType::GUN;

	FireEffect = CreateDefaultSubobject<UNiagaraComponent>("FireEffect");
	FireEffect->AttachToComponent(WeaponMesh,FAttachmentTransformRules::SnapToTargetIncludingScale, FireEffectSocketName);
	FireEffect->SetRelativeRotation(FRotator(0,0,90));
	FireEffect->bAutoActivate =false;
}

void AGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	AWeapon::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGun, AdditiveFireEffect);
}

int32 AGun::AddAdditiveWeaponMesh(USkeletalMeshComponent* Mesh)
{
	int32 Index = AWeapon::AddAdditiveWeaponMesh(Mesh);
	UNiagaraComponent* TempFireEffect = NewObject<UNiagaraComponent>(this, UNiagaraComponent::StaticClass(), TEXT("Additive Fire Effect"));
	TempFireEffect->bAutoActivate = false;
	TempFireEffect->RegisterComponent();
	TempFireEffect->SetAsset(FireEffect->GetAsset());
	TempFireEffect->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FireEffectSocketName);
	TempFireEffect->SetRelativeRotation(FRotator(0, 90, 0));
	AdditiveFireEffect.Push(TempFireEffect);
	return Index;
}

void AGun::Attack()
{
	AWeapon::Attack();

	ShowFireEffect();
}

void AGun::ShowFireEffect()
{
	Server_ShowFireEffect();
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
