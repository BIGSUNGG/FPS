// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"

AGun::AGun() : AWeapon()
{
	FireEffect = CreateDefaultSubobject<UNiagaraComponent>("FireEffect");
	FireEffect->AttachToComponent(WeaponMesh,FAttachmentTransformRules::SnapToTargetIncludingScale, FireEffectSocketName);
	FireEffect->bAutoActivate =false;
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
}
