// Fill out your copyright notice in the Description page of Project Settings.


#include "HitscanGun.h"
#include Creature_h

void AHitscanGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void AHitscanGun::Server_FireBulletResult_Implementation(const TArray<FHitResult>& BulletHitResults)
{
	for (auto& Result : BulletHitResults)
	{
		if (IsValid(Result.GetActor()))
		{
			FVector Direction = Result.TraceEnd - Result.TraceStart;
			Direction.Normalize();

			FPointDamageEvent DamageEvent(AttackDamage, Result, Direction, AttackDamageType);
			OwnerCreature->CombatComponent->OnServer_GiveDamage(Result.GetActor(), AttackDamage, DamageEvent, OwnerCreature->GetController(), this);
			if (Result.bBlockingHit)
			{
				FVector ImpaceEffectPos = Result.ImpactPoint - OwnerCreature->GetCamera()->GetForwardVector() * 15.f;
				OnServer_ImpactBullet(ImpaceEffectPos);
			}
		}
	}
}

void AHitscanGun::FireBullet()
{
	Super::FireBullet();

	FVector Spread;
	if (ShouldApplySpread())
	{
		float CurSpread = CalculateCurSpread();
		Spread.X = FMath::RandRange(-CurSpread, CurSpread);
		Spread.Y = FMath::RandRange(-CurSpread, CurSpread);
		Spread.Z = FMath::RandRange(-CurSpread, CurSpread);
	}

	TArray<FHitResult> BulletHitResults = CalculateFireHit(PROFILE_Bullet, Spread);
	FireBulletResult(BulletHitResults);

}

void AHitscanGun::FireBulletResult(const TArray<FHitResult>& BulletHitResults)
{
	for (auto& Result : BulletHitResults)
	{
		if (IsValid(Result.GetActor()))
		{
			FVector Direction = Result.TraceEnd - Result.TraceStart;
			Direction.Normalize();

			if (Result.bBlockingHit)
			{
				FVector ImpaceEffectPos = Result.ImpactPoint - OwnerCreature->GetCamera()->GetForwardVector() * 15.f;
				ImpactBulletEvent(ImpaceEffectPos);
			}
		}
	}

	Server_FireBulletResult(BulletHitResults);
}

TArray<FHitResult> AHitscanGun::CalculateFireHit(FName ProfileName, FVector Spread /*= FVector(0, 0, 0)*/)
{
	FVector EndPoint = OwnerCreature->GetCamera()->GetForwardVector() + Spread;
	EndPoint.Normalize();
	EndPoint *= BulletDistance;
	EndPoint += OwnerCreature->GetCamera()->GetComponentLocation();

	TArray<FHitResult> BulletHitResults;
	FCollisionQueryParams BulletParams(NAME_None, false, OwnerCreature);
	bool bResult = GetWorld()->SweepMultiByProfile(
		BulletHitResults,
		OwnerCreature->GetCamera()->GetComponentLocation(),
		EndPoint,
		FQuat::Identity,
		ProfileName,
		FCollisionShape::MakeSphere(BulletRadius),
		BulletParams
	);

	return BulletHitResults;
}
