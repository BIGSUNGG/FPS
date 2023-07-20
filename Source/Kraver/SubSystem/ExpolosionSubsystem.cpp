// Fill out your copyright notice in the Description page of Project Settings.


#include "ExpolosionSubsystem.h"
#include "Kraver/Component/Combat/CombatComponent.h"

void UExpolosionSubsystem::Explosion(AActor* ExplosionCauser, const FVector& ExplosionLocation, float ExplosionMaxDamage, float ExplosionRadius, FName ExplosionProfileName, const TArray<AActor*>& ExplosionIgnoreActors, UCombatComponent* CombatComponent, UCurveFloat* ExplosionDecreaseCurve /*= nullptr*/)
{
	TArray<FHitResult> OutHits;
	FCollisionQueryParams Params;
	Params.AddIgnoredActors(ExplosionIgnoreActors);

	bool bSuccess = ExplosionCauser->GetWorld()->SweepMultiByProfile
	(
		OutHits, 
		ExplosionLocation, 
		ExplosionLocation, 
		FQuat::Identity,
		ExplosionProfileName,
		FCollisionShape::MakeSphere(ExplosionRadius),
		Params
	);

	if(bSuccess)
	{
		for (auto& Result : OutHits)
		{
			UE_LOG(LogTemp, Log, TEXT("Give to %s"), *Result.GetActor()->GetName());

			FKraverDamageEvent DamageEvent;
			DamageEvent.DamageType = EKraverDamageType::EXPLOSION;

			if (CombatComponent)
			{
				CombatComponent->GiveDamage(Result.GetActor(), ExplosionMaxDamage, DamageEvent, ExplosionCauser->GetInstigatorController(), ExplosionCauser);

			}
			else
				Result.GetActor()->TakeDamage(ExplosionMaxDamage, DamageEvent, ExplosionCauser->GetInstigatorController(), ExplosionCauser);
		}
	}
}	
