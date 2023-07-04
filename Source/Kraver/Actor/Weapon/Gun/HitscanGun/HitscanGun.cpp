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

	float SpreadX;
	float SpreadY;
	float SpreadZ;
	if (GetbApplySpread())
	{
		float Spread = CalculateCurSpread();
		SpreadX = FMath::RandRange(-Spread, Spread);
		SpreadY = FMath::RandRange(-Spread, Spread);
		SpreadZ = FMath::RandRange(-Spread, Spread);
	}
	else
	{
		SpreadX = 0.f;
		SpreadY = 0.f;
		SpreadZ = 0.f;
	}

	TArray<FHitResult> BulletHitResults = CalculateFireHit(ECC_BULLET, FVector(SpreadX, SpreadY, SpreadZ));

	for (auto& Result : BulletHitResults)
	{
		auto Creature = Cast<ACreature>(Result.GetActor());
		if (IsValid(Result.GetActor()))
		{
			FKraverDamageEvent DamageEvent;
			DamageEvent.bCanHeadShot = true;
			DamageEvent.bCanParried = true;
			DamageEvent.DamageImpulse = AttackImpulse;
			DamageEvent.HitInfo = Result;
			OwnerCreature->CombatComponent->GiveDamage(Result.GetActor(), AttackDamage, DamageEvent, OwnerCreature->GetController(), this);
			if (Result.bBlockingHit)
			{
				FVector ImpaceEffectPos = Result.ImpactPoint - OwnerCreature->GetCamera()->GetForwardVector() * 15.f;
				Server_SpawnImpactEffect(ImpaceEffectPos);
			}
		}
	}
}

TArray<FHitResult> AHitscanGun::CalculateFireHit(ECollisionChannel BulletChannel, FVector Spread /*= FVector(0, 0, 0)*/)
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
