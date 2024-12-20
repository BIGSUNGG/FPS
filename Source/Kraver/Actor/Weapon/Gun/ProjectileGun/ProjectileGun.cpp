// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileGun.h"
#include Creature_h
#include KraverPlayer_h
#include Bullet_h

void AProjectileGun::OnServer_FireBullet()
{
	Super::OnServer_FireBullet();

	// 스프레드 구하기
	FVector Spread;
	if (ShouldApplySpread())
	{
		float CurSpread = CalculateCurSpread();
		Spread.X = FMath::RandRange(-CurSpread, CurSpread);
		Spread.Y = FMath::RandRange(-CurSpread, CurSpread);
		Spread.Z = FMath::RandRange(-CurSpread, CurSpread);
	}
	else
	{
		Spread = FVector::ZeroVector;
	}

	// 끝지점 구하기
	FVector SpreadDirection = OwnerCreature->GetCamera()->GetForwardVector() + Spread;
	SpreadDirection.Normalize();
	FVector EndPoint = OwnerCreature->GetCamera()->GetComponentLocation() + (SpreadDirection * 10000.f);

	FHitResult HitResult;
	FCollisionQueryParams HitParams(NAME_None, false, OwnerCreature);
	HitParams.AddIgnoredActor(this);
	bool bHitSuccess = GetWorld()->LineTraceSingleByProfile(HitResult, OwnerCreature->GetCamera()->GetComponentLocation(), EndPoint, "BulletShape", HitParams);

	FVector BulletDirection;
	if (bHitSuccess)
	{
		BulletDirection = HitResult.ImpactPoint - OwnerCreature->GetCamera()->GetComponentLocation();
		BulletDirection.Normalize();
	}
	else
		BulletDirection = SpreadDirection;
	
	// 총구 위치 구하기
	FVector MuzzleLocation;

	AKraverPlayer* Player = Cast<AKraverPlayer>(OwnerCreature);
	if (Player)
		MuzzleLocation = FppWeaponMesh->GetSocketLocation("SOCKET_Muzzle");
	else
		MuzzleLocation = TppWeaponMesh->GetSocketLocation("SOCKET_Muzzle");

	OnServer_SpawnBullet(MuzzleLocation, BulletDirection.Rotation());

}

void AProjectileGun::OnServer_SpawnBullet(FVector Location, FRotator Rotation)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = OwnerCreature;
	ABullet* Bullet = GetWorld()->SpawnActor<ABullet>(BulletClass, Location, Rotation, SpawnParams);
	Bullet->OnImpact.AddDynamic(this, &ThisClass::OnBulletImpactEvent);
	Bullet->OnServer_FireBullet(Rotation);

}

void AProjectileGun::OnBulletImpactEvent(AActor* Bullet, AActor* OtherActor, UPrimitiveComponent* OtherComponent, const FHitResult& Hit)
{
	if(!Hit.bBlockingHit)
		return;

	FVector Direction = Bullet->GetVelocity();
	Direction.Normalize();

	OnServer_ImpactBullet(Hit.ImpactPoint - Direction * 15.f);
}
