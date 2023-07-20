// Fill out your copyright notice in the Description page of Project Settings.


#include "Rocket.h"
#include "Kraver/Component/Combat/CombatComponent.h"
#include "Kraver/Subsystem/ExpolosionSubsystem.h"

void ARocket::GiveDamage(AActor* OtherActor, UPrimitiveComponent* OtherComponent, const FHitResult& Hit)
{
	Super::GiveDamage(OtherActor, OtherComponent, Hit);

	UExpolosionSubsystem* ExpolosionSubsystem = GetGameInstance()->GetSubsystem<UExpolosionSubsystem>();

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


	UGameplayStatics::ApplyRadialDamageWithFalloff(
		this, // World context object
		ExplosionDamage, // BaseDamage
		10.f, // MinimumDamage
		GetActorLocation(), // Origin
		200.f, // DamageInnerRadius
		500.f, // DamageOuterRadius
		1.f, // DamageFalloff
		UDamageType::StaticClass(), // DamageTypeClass
		TArray<AActor*>(), // IgnoreActors
		this // DamageCauser
		);
}
