// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileGun.h"
#include "Kraver/Character/Creature/Creature.h"
#include "Kraver/Character/Creature/Soldier/KraverPlayer/KraverPlayer.h"
#include "Kraver/Actor/Bullet/Bullet.h"

void AProjectileGun::FireBullet()
{
	AGun::FireBullet();

	FVector Spread;
	if (GetbApplySpread())
	{
		float CurSpread = CalculateCurSpread();
		Spread.X = FMath::RandRange(-CurSpread, CurSpread);
		Spread.Y = FMath::RandRange(-CurSpread, CurSpread);
		Spread.Z = FMath::RandRange(-CurSpread, CurSpread);
	}

	FVector SpreadDirection = OwnerCreature->GetCamera()->GetForwardVector() + Spread;
	SpreadDirection.Normalize();

	FVector EndPoint = OwnerCreature->GetCamera()->GetComponentLocation() + (SpreadDirection * 10000.f);

	FHitResult HitResult;
	FCollisionQueryParams HitParams(NAME_None, false, OwnerCreature);
	bool bHitSuccess = GetWorld()->LineTraceSingleByProfile(HitResult, OwnerCreature->GetCamera()->GetComponentLocation(), EndPoint, "BlockAll", HitParams);

	FVector BulletDirection;
	if (bHitSuccess)
	{
		BulletDirection = HitResult.ImpactPoint - OwnerCreature->GetCamera()->GetComponentLocation();
		BulletDirection.Normalize();
	}
	else
		BulletDirection = SpreadDirection;
		
	FVector MuzzleLocation;

	AKraverPlayer* Player = Cast<AKraverPlayer>(OwnerCreature);
	if (Player)
		MuzzleLocation = Player->GetArmWeaponMeshes()[this]->operator[]("Root")->GetSocketLocation("SOCKET_Muzzle");
	else
		MuzzleLocation = WeaponPrimitiveInfo["Root"]->GetSocketLocation("SOCKET_Muzzle");

	GetWorld()->SpawnActor<ABullet>(BulletClass, MuzzleLocation, BulletDirection.Rotation());
}
