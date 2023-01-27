// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"

AGun::AGun() : AWeapon()
{
	FireEffect = CreateDefaultSubobject<UParticleSystem>("FireEffect");
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
	FRotator Rotation = WeaponMesh->GetSocketRotation(FireEffectSocketName) + FRotator(-WeaponMesh->GetSocketRotation(FireEffectSocketName).Roll, 90, 0);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireEffect, WeaponMesh->GetSocketLocation(FireEffectSocketName), Rotation);
}
