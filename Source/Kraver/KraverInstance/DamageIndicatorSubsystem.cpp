// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageIndicatorSubsystem.h"
#include "Kraver/Creature/Soldier/KraverPlayer/KraverPlayer.h"
#include "Kraver/FloatingDamage/FloatingDamage.h"
#include "Kraver/Widget/DamageDirection/DamageDirection.h"

UDamageIndicatorSubsystem::UDamageIndicatorSubsystem()
{
	static ConstructorHelpers::FClassFinder<UDamageDirection> DamagedDirectionFinder(TEXT("/Game/ProjectFile/Widget/WBP_DamagedDirection.WBP_DamagedDirection_C"));
	if(DamagedDirectionFinder.Succeeded())
		DamagedDirectionClass = DamagedDirectionFinder.Class;
}

void UDamageIndicatorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

}

void UDamageIndicatorSubsystem::OnLocalPlayerBeginPlay(AKraverPlayer* Player)
{
	KraverPlayer = Player;
	Player->CombatComponent->OnClientGiveDamageSuccess.AddDynamic(this, &UDamageIndicatorSubsystem::OnClientGiveDamageSuccessEvent);
	Player->CombatComponent->OnClientAfterTakeDamageSuccess.AddDynamic(this, &UDamageIndicatorSubsystem::OnClientAfterTakeDamageEvent);
}

void UDamageIndicatorSubsystem::OnClientGiveDamageSuccessEvent(AActor* DamagedActor, float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	if(DamageResult.bAlreadyDead)
		return;

	ACreature* Creature = Cast<ACreature>(DamagedActor);
	if(!Creature)
		return;

	AFloatingDamage* DamageWidget;

	if (!DuplicationDamage && FloatingWidgets.Contains(Creature))
		DamageWidget = FloatingWidgets[Creature];
	else
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = DamageCauser;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		DamageWidget = GetWorld()->SpawnActor<AFloatingDamage>(SpawnParams);
		DamageWidget->OnDestroy.AddDynamic(this, &UDamageIndicatorSubsystem::OnDestroyEvent);

		if(!DuplicationDamage)
			FloatingWidgets.Add(Creature, DamageWidget);	
	}
	
	DamageWidget->ShowFloatingDamage(DamagedActor, DamageEvent, DamageResult);
}

void UDamageIndicatorSubsystem::OnClientAfterTakeDamageEvent(float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	UDamageDirection* Widget = Cast<UDamageDirection>(CreateWidget<UDamageDirection>(GetWorld(), DamagedDirectionClass));
	Widget->Initialize(KraverPlayer, DamageEvent.HitInfo.TraceStart);	
	Widget->AddToViewport();
}

void UDamageIndicatorSubsystem::OnDestroyEvent(AActor* Actor)
{
	AFloatingDamage* FloatingDamage = Cast<AFloatingDamage>(Actor);
	if(!FloatingDamage)
		return;

	if(!DuplicationDamage)
		FloatingWidgets.Remove(FloatingDamage->GetInchargeActor());
}
