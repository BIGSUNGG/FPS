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
	bool bHitSuccess = GetWorld()->LineTraceSingleByProfile(HitResult, OwnerCreature->GetCamera()->GetComponentLocation(), EndPoint, "BulletShape", HitParams);

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
		MuzzleLocation = WeaponFppPrimitiveInfo["Root"]->GetSocketLocation("SOCKET_Muzzle");
	else
		MuzzleLocation = WeaponPrimitiveInfo["Root"]->GetSocketLocation("SOCKET_Muzzle");

	Server_SpawnBullet(MuzzleLocation, BulletDirection.Rotation());

}

void AProjectileGun::OnBulletImpactEvent(AActor* Bullet, AActor* OtherActor, UPrimitiveComponent* OtherComponent, const FHitResult& Hit)
{
	if(!Hit.bBlockingHit)
		return;

	FVector Direction = Bullet->GetVelocity();
	Direction.Normalize();

	Server_SpawnImpactEffect(Hit.ImpactPoint - Direction * 15.f);
}

void AProjectileGun::Server_SpawnBullet_Implementation(FVector Location, FRotator Rotation)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = OwnerCreature;
	ABullet* Bullet = GetWorld()->SpawnActor<ABullet>(BulletClass, Location, Rotation, SpawnParams);
	Bullet->OnImpact.AddDynamic(this, &ThisClass::OnBulletImpactEvent);

}

