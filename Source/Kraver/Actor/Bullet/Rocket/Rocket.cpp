// Fill out your copyright notice in the Description page of Project Settings.


#include "Rocket.h"
#include CombatComponent_h

void ARocket::GiveDamage(AActor* OtherActor, UPrimitiveComponent* OtherComponent, const FHitResult& Hit)
{
	if (!IS_SERVER())
	{
		KR_LOG(Error, TEXT("Function called on client"));
		return;
	}

	Super::GiveDamage(OtherActor, OtherComponent, Hit);

	TArray<AActor*> IgnoreActors;
	UCombatComponent* CombatComp = nullptr;
	AActor* CurActor = GetOwner();
	while (CurActor)
	{
		if (!CombatComp)
			CombatComp = CurActor->FindComponentByClass<UCombatComponent>();

		IgnoreActors.Add(CurActor);
		CurActor = CurActor->GetOwner();
	}

	ECollisionChannel TraceChannel;
	FCollisionResponseParams ResponseParam;
	UCollisionProfile::GetChannelAndResponseParams("Explosion", TraceChannel, ResponseParam);

	UGameplayStatics::ApplyRadialDamageWithFalloff(
		this, // World context object
		ExplosionMaxDamage, // BaseDamage
		ExplosionMinDamage, // MinimumDamage
		GetActorLocation(), // Origin
		ExplosionInnerRadius, // DamageInnerRadius
		ExplosionOuterRadius, // DamageOuterRadius
		ExplosionFalloff, // DamageFalloff
		ExplosionDamageType, // DamageTypeClass
		IgnoreActors, // IgnoreActors
		this, // DamageCauser
		GetInstigatorController(),
		TraceChannel
		);
}
