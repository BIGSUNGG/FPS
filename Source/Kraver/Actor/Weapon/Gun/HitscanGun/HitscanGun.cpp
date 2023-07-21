// Fill out your copyright notice in the Description page of Project Settings.


#include "HitscanGun.h"
#include "Kraver/Character/Creature/Creature.h"

void AHitscanGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, BulletDistance);
	DOREPLIFETIME(ThisClass, BulletRadius);
}

void AHitscanGun::FireBullet()
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

	TArray<FHitResult> BulletHitResults = CalculateFireHit(PROFILE_Bullet, Spread);

	for (auto& Result : BulletHitResults)
	{
		auto Creature = Cast<ACreature>(Result.GetActor());
		if (IsValid(Result.GetActor()))
		{
			FVector Direction = Result.TraceEnd - Result.TraceStart;
			Direction.Normalize();

			//FPointDamageEvent DamageEvent(AttackDamage, Result, Direction, AttackDamageType);
			//OwnerCreature->CombatComponent->GiveDamage(Result.GetActor(), AttackDamage, DamageEvent, OwnerCreature->GetController(), this);

			FDamageEvent DamageEvent(AttackDamageType);
			OwnerCreature->CombatComponent->GiveDamage(Result.GetActor(), AttackDamage, DamageEvent, OwnerCreature->GetController(), this);
			if (Result.bBlockingHit)
			{
				FVector ImpaceEffectPos = Result.ImpactPoint - OwnerCreature->GetCamera()->GetForwardVector() * 15.f;
				Server_SpawnImpactEffect(ImpaceEffectPos);
			}
		}
	}
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
