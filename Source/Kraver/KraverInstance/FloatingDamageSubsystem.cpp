// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingDamageSubsystem.h"
#include "Kraver/Creature/Soldier/KraverPlayer/KraverPlayer.h"
#include "Kraver/FloatingDamage/FloatingDamage.h"

UFloatingDamageSubsystem::UFloatingDamageSubsystem()
{

}

void UFloatingDamageSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

}

void UFloatingDamageSubsystem::OnLocalPlayerBeginPlay(AKraverPlayer* Player)
{
	Player->CombatComponent->OnClientGiveDamageSuccess.AddDynamic(this, &UFloatingDamageSubsystem::OnClientGiveDamageSuccessEvent);
}

void UFloatingDamageSubsystem::OnClientGiveDamageSuccessEvent(AActor* DamagedActor, float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	if(DamageResult.bAlreadyDead)
		return;

	ACreature* Creature = Cast<ACreature>(DamagedActor);
	if(!Creature)
		return;

	AFloatingDamage* DamageWidget;

	if (DuplicationDamage && FloatingWidgets.Contains(Creature))
		DamageWidget = FloatingWidgets[Creature];
	else
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = DamageCauser;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		DamageWidget = GetWorld()->SpawnActor<AFloatingDamage>(SpawnParams);
		DamageWidget->OnDestroy.AddDynamic(this, &UFloatingDamageSubsystem::OnDestroyEvent);

		if(DuplicationDamage)
			FloatingWidgets.Add(Creature, DamageWidget);	
	}
	
	DamageWidget->ShowFloatingDamage(DamagedActor, DamageEvent, DamageResult);
}

void UFloatingDamageSubsystem::OnDestroyEvent(AActor* Actor)
{
	AFloatingDamage* FloatingDamage = Cast<AFloatingDamage>(Actor);
	if(!FloatingDamage)
		return;

	if(DuplicationDamage)
		FloatingWidgets.Remove(FloatingDamage->GetInchargeActor());
}
